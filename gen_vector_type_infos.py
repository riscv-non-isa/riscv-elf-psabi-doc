import subprocess
import os
import sys
import math

LMUL=["mf8", "mf4", "mf2", "m1", "m2", "m4", "m8"]
NF=[2, 3, 4, 5, 6, 7, 8]
SEW=[8, 16, 32, 64]
TYPES = ["int", "uint", "float", "bfloat"]

def lmul2nreg(lmul):
    if (lmul.startswith("mf")):
        return 1
    else:
        return int(lmul[1:])

def lmul2num(lmul):
    if (lmul.startswith("mf")):
        if lmul=="mf8":
            return 0.125
        if lmul=="mf4":
            return 0.25
        if lmul=="mf2":
            return 0.5
        assert false
    else:
        return int(lmul[1:])

def sizestr(lmul, nf=1):
    sz_mul = lmul2num(lmul) * nf
    if (sz_mul == 1):
        return "(VLEN / 8)"
    if (sz_mul > 1):
        return "(VLEN / 8) * %g" %(sz_mul)
    if (sz_mul < 1):
        if (sz_mul == 0.5):
            return "(VLEN / 8) / 2"
        if (sz_mul == 0.25):
            return "(VLEN / 8) / 4"
        if (sz_mul == 0.125):
            return "(VLEN / 8) / 8"
        if (sz_mul == 0.375):
            return "(VLEN / 8) * 0.375"
        if (sz_mul == 0.625):
            return "(VLEN / 8) * 0.625"
        if (sz_mul == 0.75):
            return "(VLEN / 8) * 0.75"
        if (sz_mul == 0.875):
            return "(VLEN / 8) * 0.875"
        assert false

def valid_type(sew, lmul, base_t, nf=1):
    nreg = lmul2nreg(lmul)
    if nreg * nf > 8:
        return False
    if t == "bfloat" and sew != 16:
        return False
    if t == "float" and sew == 16:
        return False
    return True

def get_note(sew, lmul, base_t, nf=1):
    ln = lmul2num(lmul)
    x = (32 / sew * ln)
    notes = []
    if (32 / sew * ln) < 1:
        notes += ["`*1`"]
    if base_t == "float" and sew == 16:
        notes += ["`*2`"]
    if base_t == "bfloat":
        notes += ["`*3`"]
    if base_t == "float" and sew == 32:
        notes += ["`*4`"]
    if base_t == "float" and sew == 64:
        notes += ["`*5`"]
    if (base_t == "int" or base_t == "uint") and sew == 64:
        notes += ["`*6`"]
    return ", ".join(notes)

print (".Type sizes and alignments for vector data types")
print ("[cols=\"4,3,>3,>2\"]")
print ("[width=80%]")
print ("|===")
print ("| Internal Name          | Type                 | Size (Bytes)  | Alignment (Bytes)")
print("")

for sew in SEW:
    for lmul in LMUL:
        for t in TYPES:
            if not valid_type(sew, lmul, t):
                continue
            typename = "v%s%s%s_t" %(t, sew, lmul)
            mname = "__rvv_" + typename
            size = sizestr(lmul)
            print ("| %-22s | %-20s | %-18s | %d" %(mname, typename, size, sew/8))
print ("|===")
print ("")

print (".Type sizes and alignments for vector tuple types")
print ("[cols=\"4,3,>3,>2\"]")
print ("[width=80%]")
print ("|===")

print ("| Internal Name          | Type                 | Size (Bytes)  | Alignment (Bytes)")
print ("")
for sew in SEW:
    for lmul in LMUL:
        for nf in NF:
            for t in TYPES:
                if not valid_type(sew, lmul, t, nf):
                    continue
                typename = "v%s%s%sx%s_t" %(t, sew, lmul, nf)
                mname = "__rvv_" + typename
                size = sizestr(lmul, nf)
                print ("| %-22s | %-20s | %-18s | %d" %(mname, typename, size, sew/8))
print ("|===")
