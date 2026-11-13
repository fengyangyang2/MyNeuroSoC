# NeuroSoC — gem5 + SystemC 神经形态 SoC 混合 ESL 仿真

基于 **gem5 SE** 与 **SystemC/TLM 2.0** 的联合仿真：ARM 程序通过 MMIO 驱动 2×2 Torus NoC + 多核 PE（SNN/ANN）。

## 依赖

- [gem5](https://www.gem5.org/) **24.1.x**，已启用 **ARM** 与内置 **SystemC** 构建
- 将本目录整体置于：`gem5/src/MyNeuroSoC/`（含 `SConscript`）
- 重新编译：`scons build/ARM/gem5.opt -j$(nproc)`

## 运行

在 gem5 仓库根目录：

```bash
./build/ARM/gem5.opt src/MyNeuroSoC/AppTest/neuro_soc_test.py
```

## 目录说明

| 路径 | 说明 |
|------|------|
| `NeuroSoCWrapper.*` | gem5 ↔ SystemC 桥接 SimObject |
| `NeuromorphicSubsystemTop.*` | SoC 顶层、MMIO、FIFO 注入 |
| `PE_Core.*` / `NoC_Router.*` | 计算核与 Torus 路由器 |
| `AppTest/` | 仿真 config、ARM 测试程序与文档 |

详细设计见 `AppTest/PROJECT_GUIDE.md`。

## 编译 ARM 测试程序（可选）

```bash
arm-linux-gnueabihf-gcc -static -O2 \
  -o AppTest/neuro_soc_app.elf AppTest/neuro_soc_app.c
```
