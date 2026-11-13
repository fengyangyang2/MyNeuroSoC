#include "NoC_Router.h"
#include <iostream>

using namespace std;

void NoC_Router::id_to_xy(int id, int& x, int& y) {
    x = id % MESH_SIZE;
    y = id / MESH_SIZE;
}

// 核心：支持 Torus 环绕的最短路径 XY 路由
RouteDir NoC_Router::decide_route(DataPacket pkt) {
    int dst_x, dst_y;
    id_to_xy(pkt.dst_id, dst_x, dst_y);

    if (pkt.dst_id == (my_y * MESH_SIZE + my_x)) return LOCAL;

    int dx = dst_x - my_x;
    int dy = dst_y - my_y;

    // 环绕判定逻辑 (贪吃蛇原理)
    if (dx > MESH_SIZE / 2) dx -= MESH_SIZE;
    if (dx < -MESH_SIZE / 2) dx += MESH_SIZE;
    if (dy > MESH_SIZE / 2) dy -= MESH_SIZE;
    if (dy < -MESH_SIZE / 2) dy += MESH_SIZE;

    // 先 X 后 Y
    if (dx > 0) return EAST;
    if (dx < 0) return WEST;
    if (dy > 0) return NORTH;
    if (dy < 0) return SOUTH;

    return LOCAL;
}
// 🌟 统一执行路由动作
void NoC_Router::execute_route(DataPacket pkt) {
    RouteDir dir = decide_route(pkt);
    if (dir == LOCAL) {
        cout << "    [Router(" << my_x << "," << my_y << ")] 到达目的地，交付本地 PE[" << pkt.dst_id << "]" << endl;
        tx_local->write(pkt);
    }
    else {
        cout << "    [Router(" << my_x << "," << my_y << ")] 路由包 Dst:" << pkt.dst_id << " -> ";
        if (dir == NORTH) { cout << "NORTH" << endl; tx_north->write(pkt); }
        if (dir == SOUTH) { cout << "SOUTH" << endl; tx_south->write(pkt); }
        if (dir == EAST) { cout << "EAST" << endl; tx_east->write(pkt); }
        if (dir == WEST) { cout << "WEST" << endl; tx_west->write(pkt); }
    }
}

// 🌟 5 个监听哨兵线程（逻辑完全一样，只是阻塞在不同的端口上）
void NoC_Router::route_from_north() { while (true) { DataPacket p; rx_north->read(p); execute_route(p); } }
void NoC_Router::route_from_south() { while (true) { DataPacket p; rx_south->read(p); execute_route(p); } }
void NoC_Router::route_from_east() { while (true) { DataPacket p; rx_east->read(p);  execute_route(p); } }
void NoC_Router::route_from_west() { while (true) { DataPacket p; rx_west->read(p);  execute_route(p); } }
void NoC_Router::route_from_local() { while (true) { DataPacket p; rx_local->read(p); execute_route(p); } }