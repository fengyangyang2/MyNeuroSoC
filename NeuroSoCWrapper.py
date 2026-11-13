from m5.SimObject import SimObject
from m5.params import *
from m5.proxy import *

class NeuroSoCWrapper(SimObject):
    type = 'NeuroSoCWrapper'
    cxx_header = "MyNeuroSoC/NeuroSoCWrapper.h"
    cxx_class = 'gem5::NeuroSoCWrapper'

    cpu_side_port = ResponsePort("CPU side port")
    addr_ranges = VectorParam.AddrRange([], "Address ranges for this device")