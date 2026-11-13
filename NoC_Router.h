#pragma once
#pragma once
#ifndef NOC_ROUTER_H
#define NOC_ROUTER_H

#include <systemc.h>
#include "PE_Core.h" // 引入 Gem5DataPacket

enum RouteDir { LOCAL, NORTH, SOUTH, EAST, WEST };

SC_MODULE(NoC_Router) {

    //改为 5 个独立的输入端口！
    sc_port<sc_fifo_in_if<DataPacket>> rx_local;  // 预留：接收本地PE的反馈
    sc_port<sc_fifo_in_if<DataPacket>> rx_north;  // 接收北方节点来的包
    sc_port<sc_fifo_in_if<DataPacket>> rx_south;  // 接收南方节点来的包
    sc_port<sc_fifo_in_if<DataPacket>> rx_east;   // 接收东方节点来的包
    sc_port<sc_fifo_in_if<DataPacket>> rx_west;   // 接收西方节点来的包
    // 5个绝对对称的输出端口
    sc_port<sc_fifo_out_if<DataPacket>> tx_local;
    sc_port<sc_fifo_out_if<DataPacket>> tx_north;
    sc_port<sc_fifo_out_if<DataPacket>> tx_south;
    sc_port<sc_fifo_out_if<DataPacket>> tx_east;
    sc_port<sc_fifo_out_if<DataPacket>> tx_west;

    int my_x;
    int my_y;
    //核心参数：网格大小。未来改成 4，就是 4x4=16核！
    static const int MESH_SIZE = 2;
    
    SC_HAS_PROCESS(NoC_Router);
    NoC_Router(sc_module_name name, int x, int y) : sc_module(name), my_x(x), my_y(y) {
        // 🌟 注册 4 个外部输入线程 + 1 个本地注入线程 (一共5个监听哨兵)
        SC_THREAD(route_from_north); //sensitive << rx_north;
        SC_THREAD(route_from_south); //sensitive << rx_south;
        SC_THREAD(route_from_east);  //sensitive << rx_east;
        SC_THREAD(route_from_west);  //sensitive << rx_west;
        SC_THREAD(route_from_local); //sensitive << rx_local;
    }

private:
    void id_to_xy(int id, int& x, int& y);
    RouteDir decide_route(DataPacket pkt);
    //统一的路由执行逻辑
    void execute_route(DataPacket pkt);
    // 5 个入口线程函数
    void route_from_north();
    void route_from_south();
    void route_from_east();
    void route_from_west();
    void route_from_local();
};

#endif