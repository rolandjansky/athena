#!/usr/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/bin/env python2.5
#
# Level 1 Central Trigger crew,
# Johan.Lundberg@cern.ch, CERN, 2009

import sys,getopt,os

def usage():
    print "%s" % sys.argv[0]
    print "  --help                                   this printout"
    print " "
    print " From COOL DB, read ctpin input signal configuration"
    print " for specified Run, and lumiblock"
    print " "
    print " Mandatory: "
    print "  --run <runNo>              run number (single number), eg 135254"
    print " "
    print " Optional: "
    print "  --lb <lb ranges>           lumi block (single number)"
    print "       (default='1', first) "
    print "  --names <signalnames>      regular expression"
    print "  --sumtable                 print only: connector,cable,type,name"
    print "  --sumtablereg              print regular expressions for matching each cable"
    
try:
    longopts=['run=','lb=','names=','sumtable','sumtablereg',
              'help','verbose']
    opts,args=getopt.getopt(sys.argv[1:],'h',longopts)
except getopt.GetoptError,e:
    usage()
    sys.exit(2) 
for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)

# defaults:
OPT={}
OPT["--run"]=int(-1)
OPT["--lb"]=1 
OPT["--verbose"]=False
OPT["--names"]=".*"
OPT["--sumtable"]=False
OPT["--sumtablereg"]=False
 
for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o in OPT:
        OPT[o] = a
    if o == '--verbose':                 
        OPT["verbose"] = True
    if o == '--sumtable':                 
        OPT["--sumtable"] = True
    if o == '--sumtablereg':                 
        OPT["--sumtablereg"] = True

try:
    OPT["--run"]=int(OPT["--run"])
    OPT["--lb"]=int(OPT["--lb"])
    if OPT["--run"] < 1:
        raise ValueError()
except ValueError:
    usage()
    print "Error: You must specify a single run number, not:",OPT["--run"]
    sys.exit(2)


import l1ctpy
from PyCool import cool
import ctpmon_py as ctpcl
import re

pattern=".*%s.*" % OPT["--names"]

dbstring=l1ctpy.get_dbid_MONP()
ROV1=(OPT["--run"]<<32)+OPT["--lb"];
numfound={}

coutner='.*'
CounterType='.*'

signals=l1ctpy.getcounterConfig(dbstring,ROV1,ROV1,pattern,CounterType)

# display signals:
if signals:
    realkeys = signals[0].keys() 
    for p in signals:
        if "CounterName" in realkeys and p["CounterName"]:
            p["DisplayName"]=p["CounterName"]
        else:
            p["DisplayName"]=p["ThresholdName"]
    if OPT["--sumtable"]:
        for p in signals:
            print p["CtpinSlot"]," ",p["CtpinConnector"], " ", p['DisplayName']," ", p['CounterType']
            
    elif OPT["--sumtablereg"]:
        first=1
        lastcabcon=''
        currentstring=""
        for p in signals:
            cabcon="%s %s %s " % ( p["CounterType"],p["CtpinSlot"], str(p["CtpinConnector"]))
            if not lastcabcon == cabcon:
                print currentstring
                currentstring=""
                lastcabcon=cabcon
                currentstring=cabcon
            else:
                currentstring=currentstring+str(",")
            currentstring=currentstring+str("^%s$"%p['DisplayName'])
        if currentstring:
            print currentstring
        print ""
    else:
        for p in signals:
            print "======== %40s : Monitor: %d" % ( p['DisplayName'], p['CounterNr'])
            for it in sorted(realkeys):
                print it, " ", p[it]
                numfound=len(signals)


if not ( OPT["--sumtable"] or OPT["--sumtablereg"]):
    print "-------------------------------------------------------------------"
    print "Number of found signals for pattern",OPT["--names"],"run:",OPT["--run"]," lb:",OPT["--lb"] ," is %40s"% numfound


    


