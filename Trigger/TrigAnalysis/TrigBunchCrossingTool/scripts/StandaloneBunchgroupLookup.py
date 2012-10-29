#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBunchCrossingTool.StandaloneBunchgroupHandler import BunchgroupHandler

run = 165954
lb = 44
bcid = 1
bg = BunchgroupHandler()
bg.verbose = False

# specify run, lb, bcid as input
# this function would be called per event
# on the first time round for a given run, all data are loaded from COOL
# and stored in memory, all other calls use the data in memory
before, after = bg.getNeighbourPattern(run,lb,bcid)

# return values are (binary) lists of neighbouring BCIDs, +- 32 crossings;
# '1': filled crossing, '0': empty crossing
if bg.verbose:
    print "Before",before
    print "After",after

# function to directly access lists of neighbouring filled BCIDs
l1, l2 = bg.getNeighbourBcids(run,lb,bcid)
print "Run",run,"neighbouring filled BCIDs for BCID",bcid,":",sorted(l1), sorted(l2)

pos = bg.getTrainPosition(run,lb,bcid)
print "BCID",bcid,"is at position",pos,"in the bunch train"

# test cycle
for lumiBl in range(100):
    for bunch in range(3564):
        before, after = bg.getNeighbourBcids(run,lumiBl,bunch)
