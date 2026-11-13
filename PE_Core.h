#pragma once
#pragma once
#ifndef PE_CORE_H
#define PE_CORE_H

#include <systemc.h>

// 数据包结构体
struct DataPacket {
    int src_id;    // [路由头部] 源核 ID
    int dst_id;    // [路由头部] 目标核 ID (现在单核填0，未来 NoC 必用)
    int input_val; // [计算载荷] 输入脉冲强度 / ANN 特征值
    int sram_idx;  // [计算载荷] 本地 SRAM 权重索引

    // 构造函数
    DataPacket(int s = 0, int d = 0, int v = 0, int idx = 0)
        : src_id(s), dst_id(d), input_val(v), sram_idx(idx) {}
};

// 重载打印运算符 (防坑)
inline std::ostream& operator<<(std::ostream& os, const DataPacket& pkt) {
    os << "Pkt(Src:" << pkt.src_id << " Dst:" << pkt.dst_id
        << " Val:" << pkt.input_val << " Idx:" << pkt.sram_idx << ")";
    return os;
}

// 严格使用 SC_MODULE
SC_MODULE(PE_Core) {
    // 信号端口保持不变
    sc_in<bool> clk;
    sc_in<int> config_mode;

    //完全仿照例程写显式 sc_port
    sc_port<sc_fifo_in_if<DataPacket>> rx_port;
    sc_port<sc_fifo_out_if<DataPacket>>  tx_port;

    int membrane_potential;
    int ann_accumulator;
    int my_id;

    SC_HAS_PROCESS(PE_Core);
    PE_Core(sc_module_name name, int id);
    void process();

private:
    //模拟单核私有的近存 SRAM(比如存 16 个突触权重)
    static const int SRAM_SIZE = 16;
    int local_weight_sram[SRAM_SIZE];
};

#endif