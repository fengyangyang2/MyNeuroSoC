#ifndef __MY_NEURO_SOC_WRAPPER_HH__
#define __MY_NEURO_SOC_WRAPPER_HH__

#include "mem/port.hh"
#include "sim/sim_object.hh"
#include "params/NeuroSoCWrapper.hh"

namespace sc_core { class sc_module; }
class NeuromorphicSubsystemTop;

namespace gem5 {

class NeuroSoCWrapper : public SimObject {
public:
    NeuroSoCWrapper(const NeuroSoCWrapperParams &p);
    ~NeuroSoCWrapper();

    class CpuSidePort : public ResponsePort {
    private:
        NeuroSoCWrapper* owner;
    public:
        CpuSidePort(const std::string& name, NeuroSoCWrapper* _owner)
            : ResponsePort(name, _owner), owner(_owner) {}
        Tick recvAtomic(PacketPtr pkt) override { return owner->handlePacket(pkt); }
        void recvFunctional(PacketPtr pkt) override { owner->handlePacket(pkt); }
        bool recvTimingReq(PacketPtr pkt) override { return false; }
        void recvRespRetry() override {}
        AddrRangeList getAddrRanges() const override { return owner->addrRanges; }
    };

    Tick handlePacket(PacketPtr pkt);
    void init() override;

    Port &getPort(const std::string &if_name, PortID idx = 0) override {
        if (if_name == "cpu_side_port")
            return cpuPort;
        return SimObject::getPort(if_name, idx);
    }

private:
    Addr deviceOffset(Addr addr) const;

    CpuSidePort cpuPort;
    NeuromorphicSubsystemTop* mySoc;
    AddrRangeList addrRanges;
};

} // namespace gem5
#endif
