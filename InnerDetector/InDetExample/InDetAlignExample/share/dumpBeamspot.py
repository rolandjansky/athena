#!/usr/bin/env python
#
# Dump contents of a beamspot.db COOL file using AtlCoolTool.
#
# Juerg Beringer, May 2008.

import sys
from CoolConvUtilities import AtlCoolTool

if len(sys.argv)<2:
    print "Usage: dumpBeamspot.py beamspot.db"
    exit(1)

dbconn = "sqlite://;schema="+sys.argv[1]+";dbname=TESTCOOL"
#dbconn = "COOLONL_INDET/OFLP200"
folder = "/Indet/Beampos"
tool = AtlCoolTool.AtlCoolTool(dbconn)
print "Beam spot information in file %s:\n" % sys.argv[1]
tool.usetag("nominal")
print tool.listinfo(folder)
print
print tool.lstags(folder)
print
print tool.more(folder)
print
