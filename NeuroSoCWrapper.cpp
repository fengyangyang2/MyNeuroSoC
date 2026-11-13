#include "MyNeuroSoC/NeuroSoCWrapper.h"
#include "MyNeuroSoC/NeuromorphicSubsystemTop.h"
#include "tlm.h"

namespace gem5 {

NeuroSoCWrapper::NeuroSoCWrapper(const NeuroSoCWrapperParams &p)
    : SimObject(p),
      cpuPort(name() + ".cpu_side_port", this),
      addrRanges(p.addr_ranges.begin(), p.addr_ranges.end())
{
    mySoc = new NeuromorphicSubsystemTop("gem5_wrapped_soc");
}

NeuroSoCWrapper::~NeuroSoCWrapper() {
    if (mySoc) delete mySoc;
}

Addr
NeuroSoCWrapper::deviceOffset(Addr addr) const
{
    for (const auto &range : addrRanges) {
        if (range.contains(addr))
            return addr - range.start();
    }
    return addr;
}

Tick
NeuroSoCWrapper::handlePacket(PacketPtr pkt)
{
    if (!pkt->isRead() && !pkt->isWrite()) return 0;

    tlm::tlm_generic_payload trans;
    sc_core::sc_time delay = sc_core::SC_ZERO_TIME;

    trans.set_command(pkt->isWrite() ? tlm::TLM_WRITE_COMMAND : tlm::TLM_READ_COMMAND);
    trans.set_address(deviceOffset(pkt->getAddr()));
    trans.set_data_length(pkt->getSize());
    trans.set_data_ptr(pkt->getPtr<uint8_t>());
    trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

    mySoc->b_transport(trans, delay);

    Tick gem5_delay = delay.value() * 1000;
    pkt->makeResponse();
    return gem5_delay;
}

void NeuroSoCWrapper::init() {
    SimObject::init();
    cpuPort.sendRangeChange();
}

} // namespace gem5
