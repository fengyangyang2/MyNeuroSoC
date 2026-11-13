#include "PE_Core.h"
#include <iostream>

using namespace std;

PE_Core::PE_Core(sc_module_name name, int id) : sc_module(name), my_id(id) {
    SC_THREAD(process);
    sensitive << clk.pos();
    membrane_potential = 0;
    ann_accumulator = 0;

    //初始化近存 SRAM：随机生成一些权重(模拟预训练好的模型权重)
    for (int i = 0; i < SRAM_SIZE; i++) {
        local_weight_sram[i] = (rand() % 5) + 1; // 权重范围 1-5
    }
    cout << "    [PE Init] Core " << my_id << " 本地 SRAM 权重已加载。" << endl;
}

void PE_Core::process() {
    while (true) {
        wait();

        DataPacket in_pkt;
        // 必须使用 -> 调用接口方法
        if (rx_port->nb_read(in_pkt)) {



            int weight = local_weight_sram[in_pkt.sram_idx % SRAM_SIZE];
            int input = in_pkt.input_val;

            if (config_mode.read() == 0) {
                cout << "    [Time " << sc_time_stamp() << "] Core " << my_id
                    << " [SNN] 读SRAM[" << in_pkt.sram_idx << "]权重=" << weight
                    << ", 输入=" << input << endl;

                membrane_potential += input * weight;
                membrane_potential -= 1;

                if (membrane_potential >= 10) {
                    cout << "       >>> 核 " << my_id << " 膜电位(" << membrane_potential
                        << ")达到阈值！发放输出脉冲！" << endl;
                    tx_port->write(DataPacket(my_id, 0, 1, 0));
                    membrane_potential = 0;
                }
                else {
                    cout << "       >>> 核 " << my_id << " 未达阈值，当前电位=" << membrane_potential << endl;
                }
            }
            else if (config_mode.read() == 1) {
                cout << "    [Time " << sc_time_stamp() << "] Core " << my_id
                    << " [ANN模式] 收到特征值=" << input << endl;

                ann_accumulator += input * weight;

                if (ann_accumulator > 15) {
                    cout << "       >>> 核 " << my_id << " ANN计算完成，输出激活值=" << ann_accumulator << endl;
                    tx_port->write(DataPacket(my_id, 0, ann_accumulator,0 ));
                    ann_accumulator = 0;
                }
                else {
                    cout << "       >>> 核 " << my_id << " 累积中，当前值=" << ann_accumulator << endl;
                }
            }
        }
    }
}