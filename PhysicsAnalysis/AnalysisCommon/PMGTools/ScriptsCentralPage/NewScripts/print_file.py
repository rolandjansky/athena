# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import re
import os
import glob
import sys
import test
substr = sys.argv[1]
astr = sys.argv[2]
a1str = sys.argv[3]
a2str = sys.argv[4]
a3str = sys.argv[5]
a4str = sys.argv[6]
a5str =sys.argv[7]
a6str =sys.argv[8]
a7str =sys.argv[9]
a8str =sys.argv[10]
a9str =sys.argv[11]
a10str = sys.argv[12]
a11str =sys.argv[13]
a12str =sys.argv[14]
a13str =sys.argv[15]
a14str =sys.argv[16]
a15str =sys.argv[17]
a16str =sys.argv[18]
a17str =sys.argv[19]
a18str =sys.argv[20]
a19str =sys.argv[21]
a20str =sys.argv[23]
print substr + astr + a1str + a2str + a3str + a4str + a5str + a6str + a7str + a8str + a9str + a10str + a11str + a12str + a13str + a14str + a15str + a16str + a17str + a18str + a19str
if os.path.exists(str(a20str)):
    fh = open(str(a20str), "r+")
    fh.read()
    value = substr + astr + a1str + a2str + a3str + a4str + a5str + a6str + a7str + a8str + a9str + a10str + a11str + a12str + a13str + a14str + a15str + a16str + a17str + a18str + a19str
    s = str(value)
    fh.write(s + '\n')
else:
    fh = open(str(a20str), "w")
    value = substr + astr + a1str + a2str + a3str + a4str + a5str + a6str + a7str + a8str + a9str + a10str + a11str + a12str + a13str + a14str + a15str + a16str + a17str + a18str + a19str
    s = str(value)
    fh.write(s + '\n')
fh.close()
