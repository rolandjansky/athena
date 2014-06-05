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
    print "  --run <runNo>              run number (single number), eg 123212"    
    print " "
    print " Optional: "
    print "  --lb <lb ranges>           lumi block (single number)"
    print "       (default='1', first) "
    print "  --names <signalnames>      regular expression"
    print " "
    
try:
    longopts=['run=','lb=','names=',
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
 
for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o in OPT:
        OPT[o] = a
    if o == '--verbose':                 
        OPT["verbose"] = True

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
for foldertype in ['/TRIGGER/LVL1/CTPINSIGNALS','/TRIGGER/LVL1/CTPINSIGNALSMBTS']:

    folder=ctpcl.getCoolFolder(dbstring,foldertype)
    objs=folder.browseObjects(ROV1,ROV1,cool.ChannelSelection.all())

    signals=[]
    for obj in objs:
        payload=obj.payload()
        name=payload["Name"]
        if re.match(pattern,name,re.IGNORECASE):
            pl={}
            for it in folder.payloadSpecification():
                pl[it.name()]=(payload[it.name()])
            signals.append(pl)

    # defin sort order
    def sortkey(sig):
        return [sig["Slot"],sig["Connector"],sig["FirstBit"],sig]

    
    if signals:
        signals.sort(key=sortkey)
        for p in signals:
            print "======== %40s : %s" % ( foldertype, p['Name'])
            for it in folder.payloadSpecification():
                print it.name(), " ", p[it.name()]
    numfound[foldertype]=len(signals)

print "-------------------------------------------------------------------"
print "Number of found signals for pattern",OPT["--names"],"run:",OPT["--run"]," lb:",OPT["--lb"]
for x in numfound:
    print "%40s :"%x, numfound[x]





