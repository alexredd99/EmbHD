#!/usr/bin/env python3
import torch as torch
import torchhd.functional as functional
import numpy as np
import sys
import math

def convert_uint32(input: torch.tensor, name: str):
    vhex = np.vectorize(lambda x: '0x{0:08x}'.format(x))
    rows, dim = input.shape
    if input.dtype == torch.bool:
        dtype = "MBin"
        m = (input > 0).type(torch.uint8) 
        pad = 32 - (dim%32)
        m = torch.nn.functional.pad(m, (0,pad), mode="constant")
        m = np.packbits(m.numpy())

    elif input.dtype in (torch.int8, torch.uint8):
        dtype = "MInt8" if input.dtype == torch.int8 else "MUint8"
        pad = 4-(dim%4)
        if pad == 4: 
            m = input
        else:
            m = torch.nn.functional.pad(input, (0,pad), mode="constant")
        m = m.numpy()

    elif input.dtype == torch.int16:
        dtype = "MInt16"
        if dim % 2 == 0:
            m = input
        else:
            m = torch.nn.functional.pad(input, (0,1), mode="constant") 
            m = m.numpy()
    else: # Data type already 4 bytes wide
        if input.dtype in (torch.int32, torch.int):
            dtype = "MInt32"
        elif input.dtype == torch.float32:
            dtype = "MFloat32"
        m = input.numpy()

    m = vhex(m.flatten().view('uint32'))

    size = len(m)
    m = np.insert(m, range(1, len(m)), ",")
    m = np.insert(m, range(16, len(m), 16), "\n")
    raw = "".join(m)
    out = "#include <stdint.h>\n"
    out += "uint32_t __attribute__((section(\".text\"))) {0}_data[{1}] = {{\n".format(name,size)
    out += raw + "\n};\n\n" +\
    "Matrix {0} = {{\n".format(name) +\
    "    .dtype  = {0},\n".format(dtype) +\
    "    .height = {0},\n".format(rows) +\
    "    .width  = {0},\n".format(dim) +\
    "    .size   = {0},\n".format(size) +\
    "    .data   = {0}_data\n".format(name) +\
    "};"
    
    header_file = open("{0}.h".format(name), "w")
    header_file.write(out)
    print("#include: \"{0}.h\"".format(name))
    print("extern Matrix {0};\n".format(name))

# Sinusoid should also create bias
def export_hv(name: str, dtype: any, sim: str, dim: int, rows: int):
    if sim == "random":
        m = functional.random_hv(rows, dim, dtype=dtype)
    elif sim == "level":
        m = functional.level_hv(rows, dim, dtype=dtype)
    elif sim == "circular":
        m = functional.circular_hv(rows, dim, dtype=dtype)
    elif sim == "sinusoid":
        # Ignore dtype, default to float32...
        if (dtype != torch.float32):
            raise ValueError('dtype for sinusoid must be torch.float32')
        m = torch.normal(mean=0, std=1, size=(rows,dim), dtype=torch.float32)
        m = torch.nn.functional.normalize(m)
        b = torch.empty(1, dim, dtype=torch.float32)
        torch.nn.init.uniform_(b,0,2*torch.pi)
        print(b)
        convert_uint32(b, "{0}_bias".format(name))
    else:
        raise ValueError('invalid similarity')
    print(m)
    return convert_uint32(m, name)

t = torch.tensor([[1,2]], dtype=torch.float32)

convert_uint32(t,"test")