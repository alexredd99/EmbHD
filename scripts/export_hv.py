import numpy as np
import torch as torch
import torchhd as functional

def bool_str(i):
    if i: return "1"
    else: return "0"

def convert_data(m):
    h, w = m.size()
    pad_len = ((w + 31) // 32)*32 - w
    raw = ""

    for row in m:
        temp = list(map(bool_str, row))
        temp.append("0"*pad_len)
        raw += "".join(temp)
    
    converted = []
    for i in range(0,len(raw),32):
        converted.append(int(raw[i:i+32], 2))

    return converted

def convert_str(data):
    out = ""
    for i, val in enumerate(data):
        out += "{0:2}, ".format(hex(val))
        if (i+1) % 20 == 0: out += "\n"
    return out

def export_hv(name, type, rows, dim, save=False):
    if type == "random":
        m =  functional.random_hv(rows,dim,dtype=torch.bool)
    elif type == "level":
        m = functional.level_hv(rows,dim,dtype=torch.bool)
    elif type == "circular":
        m = functional.circular_hv(rows,dim,dtype=torch.bool)
    else:
        print("Wrong type")
        return
    
    if save: torch.save(m, "{0}.pt".format(name))

    data = convert_data(m)
    out = "#ifndef HV_MATRIX\n" +\
    "#define HV_MATRIX\n" +\
    "#include \"hv_matrix.h\"\n" +\
    "#endif\n\n" +\
    "unsigned int {0}_data[{1}] = {{\n".format(name,len(data)) +\
    convert_str(data) +\
    "};\n\n" +\
    "HvMatrix {0} = {{\n".format(name) +\
    "    .dtype  = HvBinary,\n" +\
    "    .height = {0},\n".format(m.size()[0]) +\
    "    .width  = {0},\n".format(m.size()[1]) +\
    "    .size   = {0},\n".format(len(data)) +\
    "    .data   = (void*) {0}_data\n".format(name) +\
    "};"

    return out

"""
Usage example, must save printed out code to .h file
Ex, python3 export_hv.py > my_new_hv.h


    DIM  = 10000
    ROWS = 28*28
    TYPE = "random"
    NAME = "randomhv"
    SAVE = False

    new_hv = export_hv(NAME, TYPE, ROWS, DIM, SAVE)
    print(new_hv)

* Make this a CLI program and pass args that way?
"""