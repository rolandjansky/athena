#!/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
## Central Trigger Crew, Johan Lundberg, 2009
#
# get current run number or list of latest run numbers from cool
# ----------------------
#
# optional arguments MAXRUNS MAXDAYS
#
# Maxruns is the maximum number of runs to return
#
# maxdays is the maximum number of days (from today) to search
#
# default is 1 1

import l1ctpy

maxruns=1
maxdays=1

import sys
firstarg=1
verbose=0
if len(sys.argv)>1:
    maxruns=int(sys.argv[firstarg])
if len(sys.argv)>2:
    maxdays=int(sys.argv[firstarg+1])


R=l1ctpy.get_current_runs(maxdays,maxruns)

print " "
print "RUNS:",
for r in R:
    print r,





