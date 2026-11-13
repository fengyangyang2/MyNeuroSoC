#include "NeuromorphicSubsystemTop.h"
#include <iostream>
#include <cstring>

using namespace std;

NeuromorphicSubsystemTop::NeuromorphicSubsystemTop(sc_module_name name)
    : sc_module(name),
    sys_clk("sys_clk", 10, SC_NS),
    axi_soc_socket("axi_soc_socket"),
    pe_mode_sig("pe_mode_sig")
{
    //SOC
    soc_inject_fifo = new sc_fifo<DataPacket>("soc_inject", 8);


    for (int i = 0; i < 4; i++) {
        pe_local_fifos[i] = new sc_fifo<DataPacket>((string("loc_") + to_string(i)).c_str(), 4);
        pe_dummy_tx[i] = new sc_fifo<DataPacket>((string("dummy_") + to_string(i)).c_str(), 4);
    }

    // 实例化 16 根 Torus 通道
    x_0_e_to_1_w = new sc_fifo<DataPacket>("x_0_e_to_1_w", 4);
    x_1_w_to_0_e = new sc_fifo<DataPacket>("x_1_w_to_0_e", 4);
    x_2_e_to_3_w = new sc_fifo<DataPacket>("x_2_e_to_3_w", 4);
    x_3_w_to_2_e = new sc_fifo<DataPacket>("x_3_w_to_2_e", 4);
    x_0_w_to_1_e = new sc_fifo<DataPacket>("x_0_w_to_1_e", 4);
    x_1_e_to_0_w = new sc_fifo<DataPacket>("x_1_e_to_0_w", 4);
    x_2_w_to_3_e = new sc_fifo<DataPacket>("x_2_w_to_3_e", 4);
    x_3_e_to_2_w = new sc_fifo<DataPacket>("x_3_e_to_2_w", 4);

    y_0_n_to_2_s = new sc_fifo<DataPacket>("y_0_n_to_2_s", 4);
    y_2_s_to_0_n = new sc_fifo<DataPacket>("y_2_s_to_0_n", 4);
    y_1_n_to_3_s = new sc_fifo<DataPacket>("y_1_n_to_3_s", 4);
    y_3_s_to_1_n = new sc_fifo<DataPacket>("y_3_s_to_1_n", 4);
    y_0_s_to_2_n = new sc_fifo<DataPacket>("y_0_s_to_2_n", 4);
    y_2_n_to_0_s = new sc_fifo<DataPacket>("y_2_n_to_0_s", 4);
    y_1_s_to_3_n = new sc_fifo<DataPacket>("y_1_s_to_3_n", 4);
    y_3_n_to_1_s = new sc_fifo<DataPacket>("y_3_n_to_1_s", 4);


    //实例化 3 个哑巴 FIFO，专门用来堵住 1、2、3 号节点的本地口
    dummy_rx_local[0] = new sc_fifo<DataPacket>("dummy_rx_1", 4);
    dummy_rx_local[1] = new sc_fifo<DataPacket>("dummy_rx_2", 4);
    dummy_rx_local[2] = new sc_fifo<DataPacket>("dummy_rx_3", 4);

    // 2. 实例化节点并完成本地连线
// ==========================================
    for (int id = 0; id < 4; id++) {
        int x = id % 2;
        int y = id / 2;
        string rname = "router_" + to_string(id);
        string pname = "pe_" + to_string(id);

        pe_array[id] = new PE_Core(pname.c_str(), id);
        router_array[id] = new NoC_Router(rname.c_str(), x, y);

        router_array[id]->tx_local(*pe_local_fifos[id]);
        pe_array[id]->rx_port(*pe_local_fifos[id]);
        pe_array[id]->clk(sys_clk);
        pe_array[id]->config_mode(pe_mode_sig);
        pe_array[id]->tx_port(*pe_dummy_tx[id]);
    }

    // ==========================================
    // 终极 Torus 全双工物理连线 
    // ==========================================

    // 外部注入到节点 0
    router_array[0]->rx_local(*soc_inject_fifo);

    router_array[1]->rx_local(*dummy_rx_local[0]);
    router_array[2]->rx_local(*dummy_rx_local[1]);
    router_array[3]->rx_local(*dummy_rx_local[2]);

    // --- 节点 0 (左下) 的连线 ---
    router_array[0]->tx_east(*x_0_e_to_1_w);  router_array[1]->rx_west(*x_0_e_to_1_w);
    router_array[0]->tx_west(*x_0_w_to_1_e);  router_array[1]->rx_east(*x_0_w_to_1_e); // 环绕
    router_array[0]->tx_north(*y_0_n_to_2_s);  router_array[2]->rx_south(*y_0_n_to_2_s);
    router_array[0]->tx_south(*y_0_s_to_2_n);  router_array[2]->rx_north(*y_0_s_to_2_n); // 环绕

    // --- 节点 1 (右下) 的连线 ---
    router_array[1]->tx_west(*x_1_w_to_0_e);  router_array[0]->rx_east(*x_1_w_to_0_e);
    router_array[1]->tx_east(*x_1_e_to_0_w);  router_array[0]->rx_west(*x_1_e_to_0_w); // 环绕
    router_array[1]->tx_north(*y_1_n_to_3_s);  router_array[3]->rx_south(*y_1_n_to_3_s);
    router_array[1]->tx_south(*y_1_s_to_3_n);  router_array[3]->rx_north(*y_1_s_to_3_n); // 环绕

    // --- 节点 2 (左上) 的连线 ---
    router_array[2]->tx_east(*x_2_e_to_3_w);  router_array[3]->rx_west(*x_2_e_to_3_w);
    router_array[2]->tx_west(*x_2_w_to_3_e);  router_array[3]->rx_east(*x_2_w_to_3_e); // 环绕
    router_array[2]->tx_south(*y_2_s_to_0_n);  router_array[0]->rx_north(*y_2_s_to_0_n);
    router_array[2]->tx_north(*y_2_n_to_0_s);  router_array[0]->rx_south(*y_2_n_to_0_s); // 环绕

    // --- 节点 3 (右上) 的连线 ---
    router_array[3]->tx_west(*x_3_w_to_2_e);  router_array[2]->rx_east(*x_3_w_to_2_e);
    router_array[3]->tx_east(*x_3_e_to_2_w);  router_array[2]->rx_west(*x_3_e_to_2_w); // 环绕
    router_array[3]->tx_south(*y_3_s_to_1_n);  router_array[1]->rx_north(*y_3_s_to_1_n);
    router_array[3]->tx_north(*y_3_n_to_1_s);  router_array[1]->rx_south(*y_3_n_to_1_s); // 环绕


    cout << "[SoC Top] 2x2 Torus NoC 拓扑构建完成！16根全双工链路已打通！" << endl;


    SC_THREAD(monitor_clock); sensitive << sys_clk;

    axi_soc_socket.register_b_transport(this, &NeuromorphicSubsystemTop::b_transport);

    memset(packet_staging, 0, sizeof(packet_staging));
}

void NeuromorphicSubsystemTop::monitor_clock() {
    while (true) {
        wait();
        perf_monitor.tick(); // 🌟 委托给监控器去数
    }
}

void NeuromorphicSubsystemTop::b_transport(tlm_generic_payload& trans, sc_time& delay) {
    tlm_command cmd = trans.get_command();
    uint64_t addr = trans.get_address();
    unsigned char* data_ptr = trans.get_data_ptr();
    unsigned int len = trans.get_data_length();

    delay += sc_time(20, SC_NS); // 模拟总线延迟

    // 🌟 修改地址映射逻辑
    if (addr == 0x0000) {
        // 【地址 0x0000】：专门用于切换 PE 模式
        if (cmd == TLM_WRITE_COMMAND) {
            int received_mode = *((int*)data_ptr);
            cout << "[SoC Top] 解析 AXI 指令 -> 向 PE阵列下发模式切换: " << received_mode << endl;
            pe_mode_sig.write(received_mode); //真正改变 PE 的状态！
        }
    }
    else if (addr >= 0x0200 && addr < 0x0210) {
        // 数据 FIFO：支持 ARM 拆成多次 32-bit store 的写入
        if (cmd == TLM_WRITE_COMMAND) {
            unsigned off = addr - 0x0200;
            if (off + len > sizeof(DataPacket)) {
                trans.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
                return;
            }
            memcpy(packet_staging + off, data_ptr, len);
            if (off + len >= sizeof(DataPacket)) {
                DataPacket hw_pkt;
                memcpy(&hw_pkt, packet_staging, sizeof(DataPacket));
                cout << "[SoC Top] 数据包(Dst_id:" << hw_pkt.dst_id
                     << " Val:" << hw_pkt.input_val
                     << " Idx:" << hw_pkt.sram_idx
                     << ") 注入 NoC 网络..." << endl;
                if (!soc_inject_fifo->nb_write(hw_pkt)) {
                    cout << "[SoC Top] WARN: inject FIFO 已满，丢弃数据包" << endl;
                } else {
                    // 从 Gem5 MMIO 上下文写入，用 async 通知 SystemC 调度器
                    soc_inject_fifo->async_request_update();
                }
                memset(packet_staging, 0, sizeof(packet_staging));
                perf_monitor.record_compute();
            }
        }
    }
    else if (addr == 0x0100) {
        // 【地址 0x0100】：保留的 DMA 权重区 (暂不深入)
        if (cmd == TLM_WRITE_COMMAND) {
            cout << "[SoC Top] 解析 AXI 指令 -> 写入 SRAM 权重区 [0x" << hex << addr << dec << "]" << endl;
        }
    }
    else {
        trans.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
        return;
    }

    trans.set_response_status(TLM_OK_RESPONSE);
}

