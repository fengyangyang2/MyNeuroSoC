import os

import m5
from m5.objects import *

_APP_DIR = os.path.dirname(os.path.abspath(__file__))

SOC_BASE = 0x10000000
SOC_SIZE = 0x1000

system = System()
system.clk_domain = SrcClockDomain(clock="1GHz", voltage_domain=VoltageDomain())
system.mem_mode = 'atomic'
system.mem_ranges = [AddrRange('128MB')]

system.cpu = AtomicSimpleCPU()
system.cpu.interrupts = [ArmInterrupts()]

system.membus = SystemXBar()
system.cpu.icache_port = system.membus.cpu_side_ports
system.cpu.dcache_port = system.membus.mem_side_ports

system.neuro_soc = NeuroSoCWrapper()
system.neuro_soc.cpu_side_port = system.membus.mem_side_ports
system.neuro_soc.addr_ranges = [AddrRange(SOC_BASE, SOC_BASE + SOC_SIZE - 1)]

system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.port = system.membus.mem_side_ports

binary = os.path.join(_APP_DIR, "neuro_soc_app.elf")

system.workload = SEWorkload.init_compatible(binary)

process = Process()
process.cmd = [binary]
system.cpu.workload = process
system.cpu.createThreads()

system.system_port = system.membus.cpu_side_ports

root = Root(full_system=False, system=system)
m5.instantiate()

process.map(SOC_BASE, SOC_BASE, SOC_SIZE, cacheable=False)

print("✅ atomic 模式 MMIO 验证启动！")
exit_event = m5.simulate()
print(f"退出原因: {exit_event.getCause()}")
