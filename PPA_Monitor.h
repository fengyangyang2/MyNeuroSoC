#pragma once
#pragma once
#ifndef PPA_MONITOR_H
#define PPA_MONITOR_H

#include <iostream>

class PPA_Monitor {
public:
    PPA_Monitor() {
        total_clock_cycles = 0;
        total_mac_ops = 0;
        total_sram_reads = 0;
    }

    // 提供给外部调用的“记账”接口
    void record_compute() {
        total_mac_ops++;
        total_sram_reads++;
    }

    // 提供给外部调用的“数时钟”接口
    void tick() {
        total_clock_cycles++;
    }

    // 🌟 仿真结束时打印报告
    void report() {
        std::cout << "\n=================================================" << std::endl;
        std::cout << "          [性能分析] PPA Monitor 报告" << std::endl;
        std::cout << "=================================================" << std::endl;
        std::cout << "[Monitor] 总仿真时间:   " << total_clock_cycles << " 时钟周期" << std::endl;
        std::cout << "[Monitor] 等效计算量:     " << total_mac_ops << " 次 MAC 操作" << std::endl;
        std::cout << "[Monitor] 片内SRAM读取:   " << total_sram_reads << " 次 (无外部DRAM访问)" << std::endl;

        if (total_clock_cycles > 0) {
            double util = (double)total_mac_ops / total_clock_cycles * 100;
            std::cout << "[Monitor] 计算单元利用率: " << util << " %" << std::endl;
        }
        std::cout << "=================================================\n" << std::endl;
    }

private:
    unsigned long long total_clock_cycles;
    unsigned long long total_mac_ops;
    unsigned long long total_sram_reads;
};

#endif