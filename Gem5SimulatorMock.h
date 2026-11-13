#pragma once
#ifndef GEM5_SIMULATOR_MOCK_H
#define GEM5_SIMULATOR_MOCK_H

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>

using namespace sc_core;
using namespace tlm;

// 数据包结构体
struct Gem5DataPacket {
    int src_id;    // [路由头部] 源核 ID
    int dst_id;    // [路由头部] 目标核 ID (现在单核填0，未来 NoC 必用)
    int input_val; // [计算载荷] 输入脉冲强度 / ANN 特征值
    int sram_idx;  // [计算载荷] 本地 SRAM 权重索引

    // 构造函数
    Gem5DataPacket(int s = 0, int d = 0, int v = 0, int idx = 0)
        : src_id(s), dst_id(d), input_val(v), sram_idx(idx) {}
};

// 重载打印运算符 (防坑)
inline std::ostream& operator<<(std::ostream& os, const Gem5DataPacket& pkt) {
    os << "Gem5Pkt(Src:" << pkt.src_id << " Dst:" << pkt.dst_id
        << " Val:" << pkt.input_val << " Idx:" << pkt.sram_idx << ")";
    return os;
}

class Gem5SimulatorMock : public sc_module {
public:
    tlm_utils::simple_initiator_socket<Gem5SimulatorMock> init_socket;

    // 构造函数声明
    Gem5SimulatorMock(sc_module_name name);

private:
    // 线程函数声明
    void mock_gem5_behavior();
};

#endif