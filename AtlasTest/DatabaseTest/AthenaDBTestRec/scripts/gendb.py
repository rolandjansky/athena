#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# wrapper to run TestCoolRecWriter with a given confiyration file (assumed
# to be in ../config/ directory) to produce a joboption file for Athena
# and a database in sqlite subdirectory
# Typically to be used in a run subdirectory of the package
#
import sys
import os
import traceback

if len(sys.argv) < 2:
    print "usage", sys.argv[0],'<configfile> <nrun>'
    sys.exit(-1)

config=sys.argv[1]
nrun=sys.argv[2]

jofile=config+"_r"+nrun+"_jobOptions.py"
dbfile="sqlite/"+config+"_r"+nrun+".db"

print "Generate Athena joboptions in",jofile
os.system("export POOL_OUTMSG_LEVEL=E;TestCoolRecWriter.exe \"sqlite:X:"+dbfile+":TESTDB\" 2 ../config/"+config+".txt "+nrun+" 10000 730000 0 3600")
os.system("mv CoolReadConfig.py "+jofile)

print "Generate database file",dbfile
os.system("export POOL_OUTMSG_LEVEL=E;TestCoolRecWriter.exe \"sqlite:X:"+dbfile+":TESTDB\" 1 ../config/"+config+".txt "+nrun+" 10000 730000 0 3600")

print "Done - filesize:"
os.system("ls -lL "+dbfile)

