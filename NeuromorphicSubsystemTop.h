#pragma once
#ifndef NEUROMORPHIC_SUBSYSTEM_TOP_H
#define NEUROMORPHIC_SUBSYSTEM_TOP_H

// 只放需要的头文件
#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include "PE_Core.h"
#include "NoC_Router.h" 
#include "PPA_Monitor.h" 

using namespace sc_core;
using namespace tlm;

// 类 声明
// ==========================================
// 核心外壳：神经形态子系统顶层 
// ==========================================
class NeuromorphicSubsystemTop : public sc_module {
public:
    // --------------------------------------------------
    // 🎯 这是未来的“物理插座”！gem5 的 AXI 总线将来就接在这个 socket 上！
    // --------------------------------------------------
    tlm_utils::simple_target_socket<NeuromorphicSubsystemTop> axi_soc_socket;

    // 构造函数声明
    SC_HAS_PROCESS(NeuromorphicSubsystemTop);
    NeuromorphicSubsystemTop(sc_module_name name);
    sc_fifo<DataPacket> pe_rx_fifo; // 喂给 PE 数据的 FIFO
    sc_fifo<DataPacket> pe_tx_dummy_fifo; // 接收 PE 输出的虚拟 FIFO (防止报错)

    PPA_Monitor perf_monitor;
    static const int PE_NUM = 4;


    // 函数声明    // 将 socket 的读写请求，绑定到我们的处理函数上
    void b_transport(tlm_generic_payload& trans, sc_time& delay);

     void executeTlmTransaction(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay) {
        b_transport(trans, delay); // 内部调用 private 函数
    }

private:
    int current_mode;
    int pending_input_data;
    unsigned char packet_staging[sizeof(DataPacket)];
    sc_clock sys_clk;
    PE_Core* pe_array[PE_NUM];
    NoC_Router* router_array[PE_NUM];

    // 连线通道指针 (12根线：4本地 + 4东西 + 4南北)
    sc_fifo<DataPacket>* soc_inject_fifo; // SoC 接入点

    // 路由器到本地 PE 的连线 (4根)
    sc_fifo<DataPacket>* pe_local_fifos[PE_NUM];
    sc_fifo<DataPacket>* pe_dummy_tx[PE_NUM];

    sc_fifo<DataPacket>* dummy_rx_local[3];


    // 真正的 Torus 全双工连线：8根东西向 + 8根南北向 
    // --- X 轴 (东西向) ---
    sc_fifo<DataPacket>* x_0_e_to_1_w; // 节点0东出 -> 节点1西入
    sc_fifo<DataPacket>* x_1_w_to_0_e; // 节点1西出 -> 节点0东入
    sc_fifo<DataPacket>* x_2_e_to_3_w;
    sc_fifo<DataPacket>* x_3_w_to_2_e;
    // 环绕线
    sc_fifo<DataPacket>* x_0_w_to_1_e; // 节点0西出(环绕) -> 节点1东入
    sc_fifo<DataPacket>* x_1_e_to_0_w;
    sc_fifo<DataPacket>* x_2_w_to_3_e;
    sc_fifo<DataPacket>* x_3_e_to_2_w;

    // --- Y 轴 (南北向) ---
    sc_fifo<DataPacket>* y_0_n_to_2_s; // 节点0北出 -> 节点2南入
    sc_fifo<DataPacket>* y_2_s_to_0_n;
    sc_fifo<DataPacket>* y_1_n_to_3_s;
    sc_fifo<DataPacket>* y_3_s_to_1_n;
    // 环绕线
    sc_fifo<DataPacket>* y_0_s_to_2_n;
    sc_fifo<DataPacket>* y_2_n_to_0_s;
    sc_fifo<DataPacket>* y_1_s_to_3_n;
    sc_fifo<DataPacket>* y_3_n_to_1_s;

    sc_signal<int> pe_mode_sig;             // 驱动 PE 模式的信号线

    

    void monitor_clock();
};

#endif