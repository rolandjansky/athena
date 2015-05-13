#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Susumu Oda (2011/07/28)

import os,sys
if len(sys.argv)<9 or sys.argv[1]=="-h" or sys.argv[1]=="--help":
    print "Usage: tct_findOverwrite.py <ESD w/HIST> <AOD w/ HIST> <TAG w/ HIST> <ESD w/o HIST> <AOD w/o HIST> <TAG w/o HIST> <TAG from ESD> <test name>"
    print "  Example: tct_findOverwrite.py myESD_Egamma_0.pool.root myAOD_Egamma_0.AOD.pool.root myTAG_Egamma_0.root myESD_Egamma_1.pool.root myAOD_Egamma_1.AOD.pool.root myTAG_Egamma_1.root myTAG_Egamma_2.root Run00184169_Egamma_Collisions_Comparison"
    sys.exit(-1)

from PROCTools.compareTCTs import * 

irrelevantContainers = ['RecoTimingObj_p1_RAWtoESD_timings', 'RecoTimingObj_p1_ESDtoAOD_timings', 'EventBookkeeperCollection_p2_EventBookkeepers', 'EventBookkeeperCollection_p2_IncompleteEventBookkeepers', 'IOVMetaDataContainer_p1__TagInfo', 'EventFormat', '_TagInfo', 'xAOD::EventFormat_v1_EventFormat']

ESDwHIST = sys.argv[1]
if not os.path.exists(ESDwHIST):
    print "%s doesn't exist" % ESDwHIST
    ESDwHIST = None

AODwHIST = sys.argv[2]
if not os.path.exists(AODwHIST):
    print "%s doesn't exist" % AODwHIST
    AODwHIST = None

TAGwHIST = sys.argv[3]
if not os.path.exists(TAGwHIST):
    print "%s doesn't exist" % TAGwHIST
    TAGwHIST = None

ESDwoHIST = sys.argv[4]
if not os.path.exists(ESDwoHIST):
    print "%s doesn't exist" % ESDwoHIST
    ESDwoHIST = None

AODwoHIST = sys.argv[5]
if not os.path.exists(AODwoHIST):
    print "%s doesn't exist" % AODwoHIST
    AODwoHIST = None

TAGwoHIST = sys.argv[6]
if not os.path.exists(TAGwoHIST):
    print "%s doesn't exist" % TAGwoHIST
    TAGwoHIST = None

TAGfromESD = sys.argv[7]
if not os.path.exists(TAGfromESD):
    print "%s doesn't exist" % TAGfromESD
    TAGfromESD = None

testName = sys.argv[8]
sumFile=open(testName+".txt","w")
sumFile.write(testName)

details = None
identical = [False, False, False, False]
compName = ["Comparison #1: TAG with HIST vs TAG without HIST",
            "Comparison #2: TAG from AOD  vs TAG from ESD    ",
            "Comparison #3: ESD with HIST vs ESD without HIST",
            "Comparison #4: AOD with HIST vs AOD without HIST"]

if (TAGwHIST!=None and TAGwoHIST!=None):
    print "\nComparison #1: TAG with HIST [%s] vs TAG without HIST [%s]" % (TAGwHIST,TAGwoHIST)
    (eq,dif) = compareTreeFiles(TAGwHIST,TAGwoHIST,details)
    identical[0] = (dif==0) 

if (TAGwoHIST!=None and TAGfromESD!=None):
    print "\nComparison #2: TAG from AOD [%s] vs TAG from ESD [%s]" % (TAGwoHIST,TAGfromESD)
    (eq,dif) = compareTreeFiles(TAGwoHIST,TAGfromESD,details)
    identical[1] = (dif==0) 

if (ESDwHIST!=None and ESDwoHIST!=None):
    print "\nComparison #3: ESD with HIST [%s] vs ESD without HIST [%s]" % (ESDwHIST,ESDwoHIST)
    identical[2] = not diffPoolFiles(ESDwHIST,ESDwoHIST,details,toIgnore=irrelevantContainers)

if (AODwHIST!=None and AODwoHIST!=None):
    print "\nComparison #4: AOD with HIST [%s] vs AOD without HIST [%s]" % (AODwHIST,AODwoHIST)
    identical[3] = not diffPoolFiles(AODwHIST,AODwoHIST,details,toIgnore=irrelevantContainers)

if (identical[0] and identical[1] and identical[2] and identical[3]):
    print "\nResult: All comparisons are OK."
else:
    print "\nResult: At least one comparison is NOT OK:"
    for i in range(4):
        if not identical[i]: print " %s is NOT OK" % (compName[i])

if (identical[0] and identical[1] and identical[2] and identical[3]): sumFile.write(" [OK]\n")
else:                                                                 sumFile.write(" [ERR]\n")
for i in range(4):
    sumFile.write("  "+compName[i]+" is ")
    if not identical[i]: sumFile.write("NOT OK\n") 
    else:                sumFile.write("    OK\n") 
sumFile.close()
