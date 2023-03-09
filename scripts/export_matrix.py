#!/usr/bin/env python3
import numpy as np
import torch as torch
import torch.nn as nn
import torch.nn.functional as F
import torchhd as functional
import sys, getopt

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
    if type == "r":
        m =  functional.random_hv(rows,dim,dtype=torch.bool)
    elif type == "l":
        m = functional.level_hv(rows,dim,dtype=torch.bool)
    elif type == "c":
        m = functional.circular_hv(rows,dim,dtype=torch.bool)
    elif type == "s":
        pass
    
    if save: torch.save(m, "{0}.pt".format(name))

    data = convert_data(m)
    out = "uint32_t __attribute__((section(\".text\"))) {0}_data[{1}] = {{\n".format(name,len(data)) +\
    convert_str(data) +\
    "};\n\n" +\
    "Matrix {0} = {{\n".format(name) +\
    "    .dtype  = MBin,\n" +\
    "    .height = {0},\n".format(m.size()[0]) +\
    "    .width  = {0},\n".format(m.size()[1]) +\
    "    .size   = {0},\n".format(len(data)) +\
    "    .data   = {0}_data\n".format(name) +\
    "};"

    return out

def usage():
    out = "usage: export_matrix.py -D {dimension} -R {rows} -s {similarity} -t {type} -n {name} [-h]\n" +\
    "\t(-h --help)  prints this usage message\n" +\
    "\t(-D --dim)   dimension of hyperspace\n" +\
    "\t(-R --rows)  rows in hyperspace\n" +\
    "\t(-s --sim)   similarity of hyperspace\n" +\
    "\t(-t --type)  data type of hyperspace\n" +\
    "\t(-n --name)  name of generated Matrix struct\n"
    print(out)

options = "ht:s:m:M:D:R:n:"
long_options = ["help","type=","sim=","min=","max=","dim=","rows=","name="] 

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], options, long_options)
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)
    dtype = None
    sim   = None
    min   = None
    max   = None
    dim   = None
    rows  = None
    name  = None

    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("-t", "--type"):
            if a in ("b", "bin", "binary"):
                dtype = torch.bool
            elif a in ("i", "int"):
                dtype = torch.int32
            elif a in ("f", "float"):
                dtype = torch.float32
            else:
                print("(-t --type) invalid type: {0}".format(a))
                print("valid types:")
                print("\t(b, bin, binary)")
                print("\t(i, int)")
                print("\t(f, float)")
                sys.exit()
        elif o in ("-s", "--sim"):
            if a in ("r", "random"):
                sim = "r"
            elif a in ("l", "level"):
                sim = "l"
            elif a in ("c", "circular"):
                sim = "c"
            elif a in ("s", "sinusoid"):
                sim = "s"
            else:
                print("(-s --sim) invalid similarity: {0}".format(a))
                print("valid similarities:")
                print("\t(r, random)")
                print("\t(l, level)")
                print("\t(c, circular)")
                print("\t(s, sinusoid)")
                sys.exit()
        elif o in ("-m", "--min"):
            try: # need to try to convert this to float or int correctly
                pass
            except:
                pass
            min = a
            print("Min: ", a)
            pass
        elif o in ("-M", "--max"):
            try: # need to try to convert this to float or int correctly
                pass
            except:
                pass
            print("Max", a)
            pass
        elif o in ("-D", "--dim"):
            try:
                dim = int(a)
            except:
                print("(-D --dim) invalid dimension: {0}".format(a))
                sys.exit()
        elif o in ("-R", "--rows"):
            try:
                rows = int(a)
            except:
                print("(-R --rows) invalid rows: {0}".format(a))
                sys.exit()
        elif o in ("-n", "--name"):
            name = a
        else:
            assert False, "unhandled option"
    
    if dim == None:
        print("(-D --dim) must specify dimension")
        sys.exit()
    if rows == None:
        print("(-R --rows) must rows")
        sys.exit()
    if sim == None:
        print("(-s --sim) must specify similarity")
        sys.exit()
    if dtype == None:
        dtype = torch.bool
        print("Defaulting to binary hypervectors...")
    if name == None:
        name = "hv"
        print("Defaulting to name:\thv")


    # Actually create hypervector now...
    out = export_hv(name, sim, rows, dim)
    # Need convert appropriate datatypes to hex for importing in C
    print(out)    

if __name__ == '__main__':
    main()