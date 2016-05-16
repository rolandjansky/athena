#!/usr/bin/env python
import sys


#
#__________________________________________________________________
def dec2hex(n):
    return ("0%X" % n)[-2:]


#
#__________________________________________________________________
modToFrag = {'LBA' : '0x1',
	     'LBC' : '0x2',
	     'EBA' : '0x3',
	     'EBC' : '0x4'}

inFile = sys.argv[1]

buf = ""

lines = open(inFile,"r").readlines()
for line in lines:
    field = line.split(":")
    if not len(field): continue
    module = field[0][:3]
    modNum = int(field[0][3:]) - 1
    chan   = int(field[1])
    type   = field[2].strip()

    hexModule = modToFrag[module] + dec2hex(modNum)

    lhstat = ""
    if   type=="L": lhstat = "1\t0"
    elif type=="H": lhstat = "0\t1"
    elif type=="B": lhstat = "1\t1"
    else:
        raise("unkown type \"%s\""%type)
    
    buf = buf + "Bch\t%s\t%s\t%s\n" % (hexModule,chan,lhstat)

print buf

bchFile = open("Tile2007.bch","w")
bchFile.write(buf)
bchFile.close()
