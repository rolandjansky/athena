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

if len(sys.argv) < 4:
    print "usage", sys.argv[0],'<configfile> <nrun> <schema> <passwd>'
    sys.exit(-1)

config=sys.argv[1]
nrun=sys.argv[2]
schema=sys.argv[3]
passwd=sys.argv[4]

jofile=config+"_r"+nrun+"_intr_jobOptions.py"
dbstr="\"oracle:INTR:"+schema+":TESTDB:"+schema+":"+passwd+"\""

print "Generate Athena joboptions in",jofile
os.system("export POOL_OUTMSG_LEVEL=E;TestCoolRecWriter.exe "+dbstr+" 2 ../config/"+config+".txt "+nrun+" 10000 730000 0 36")
os.system("mv CoolReadConfig.py "+jofile)

print "Fill Oracle database"
os.system("export POOL_OUTMSG_LEVEL=E;TestCoolRecWriter.exe "+dbstr+" 1 ../config/"+config+".txt "+nrun+" 10000 730000 0 36")

print "Done"


