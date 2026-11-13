# Import('*')


# # 🌟🌟🌟 终极顿悟：彻底抛弃外部 SystemC 路径！
# # 直接把 Gem5 内部的 SystemC 头文件目录加进来！
# # 这样我们用的头文件和 Gem5 链接的库 100% 严丝合缝，绝不会有任何符号冲突！
# env.Append(CPPPATH = [
#     'src/systemc',          # Gem5 内置的 SystemC 核心头文件
#     'src/systemc/tlm_core', # Gem5 内置的 TLM 2.0 头文件
#     'src/systemc/tlm_utils',# Gem5 内置的 TLM 工具头文件
#     Dir('.').abspath,       # 你自己的头文件（如 PE_Core.h）
# ])

# # 只需要开启动态进程宏（Gem5 内部库支持这个）
# env.Append(CPPDEFINES = ['SC_INCLUDE_DYNAMIC_PROCESSES'])

# SimObject('NeuroSoCWrapper.py', sim_objects=['NeuroSoCWrapper'])


# # 🌟🌟🌟 彻底删除 LINKFLAGS！绝不链接外部 libsystemc.a！
# sources = [
#     'PE_Core.cpp',
#     'NoC_Router.cpp',
#     'PPA_Monitor.cpp',
#     'NeuromorphicSubsystemTop.cpp',
#     'PPA_Monitor.cpp',
#     'NeuroSoCWrapper.cpp',
# ]

# for src in sources:
#     Source(src, tags=['gem5 lib'])


Import('*')

env.Append(CPPPATH = [
    'src/systemc',
    'src/systemc/tlm_core',
    'src/systemc/tlm_utils',
])

env.Append(CPPDEFINES = ['SC_INCLUDE_DYNAMIC_PROCESSES'])

# 关键：用 PySource 把同目录下的 py 文件注入进来
SimObject('NeuroSoCWrapper.py', sim_objects=['NeuroSoCWrapper'],
          enums=[])  
#PySource('m5.objects', 'NeuroSoCWrapper.py')

sources = [
    'PE_Core.cpp',
    'NoC_Router.cpp',
    'PPA_Monitor.cpp',
    'NeuromorphicSubsystemTop.cpp',
    'NeuroSoCWrapper.cpp'
]

for src in sources:
    Source(src, tags=['gem5 lib'])