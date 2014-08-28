#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# wrapper to run TestCoolRecWriter with a given confiyration file (assumed
# to be in ../config/ directory) to produce a joboption file for Athena
# and a set of single run databases for Tier0 test
# Typically to be used in a run subdirectory of the package
# ../scripts/gen_tier0test.py ti0c1301 10
#
import sys
import os
import traceback

if len(sys.argv) < 3:
    print "usage", sys.argv[0],'<configfile> <nrun>'
    sys.exit(-1)

config=sys.argv[1]
nrun=sys.argv[2]
dbname="TI0C1301"

for irun in range(0,int(nrun)):
  isrun=str(irun)
  jofile=config+"_jobOptions.py"
  dbfile="sqlite/"+config+"_r"+isrun+".db"

  if (irun==0):
    print "Generate Athena joboptions in",jofile
    os.system("export POOL_OUTMSG_LEVEL=E;TestCoolRecWriter.exe \"sqlite:X:"+dbfile+":"+dbname+"\" 2 ../config/"+config+".txt 1 "+isrun+" 10000 0 3600")
    os.system("mv CoolReadConfig.py "+jofile)

  print "Generate database file",dbfile
  os.system("export POOL_OUTMSG_LEVEL=E;TestCoolRecWriter.exe \"sqlite:X:"+dbfile+":"+dbname+"\" 1 ../config/"+config+".txt 1 "+isrun+" 10000 0 3600")

print "All done"
