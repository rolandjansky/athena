#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,sys
from PROCTools.getFileLists import *
sys.argv += [ '-b' ] # tell ROOT to not use graphics
from ROOT import TFile,TTree
from PROCTools.diffTAGTree import diffTTree
#import re

os.environ['STAGE_SVCCLASS']="atlascerngroupdisk"
os.environ['STAGE_HOST']="castoratlast3"

ignoreTrees=set(("CollectionMetadata",))

if __name__ == "__main__":
    if len(sys.argv)<4 or sys.argv[1]=="-h" or sys.argv[1]=="--help":
        print "Usage: compareNtuple.py ntuple1 ntuple2"
        print "  Example: compareTCTs.py /castor/cern.ch/user/m/mlimper/ntuple1.root /castor/cern.ch/user/m/mlimper/ntuple2.root"
        sys.exit(-1)

    rName = sys.argv[1]
    vName = sys.argv[2]
        
    if rName.startswith("/castor"):
        rFile=TFile.Open("root://castoratlas/"+rName)
    else:
        rFile=TFile.Open(rName)

    if vName.startswith("/castor"):
        vFile=TFile.Open("root://castoratlas/"+vName)
    else:
        vFile=TFile.Open(vName)

    if rFile is None:
        print "Failed to open reference file",rName
    
    if vFile is None:
        print "Failed to open validation file",vName

    rKeys=set()
    for k in rFile.GetListOfKeys():
        rKeys.add(k.GetName())
    vKeys=set()
    for k in vFile.GetListOfKeys():
        vKeys.add(k.GetName())
    #print rKeys
    #print vKeys
    keys=rKeys & vKeys
    keys -= ignoreTrees
        
    if len(keys)==0:
        print "ERROR no common trees names found in files",rName,vName

    nGood=0
    nBad=0
    for k in keys:
        rTree=rFile.Get(k)
        vTree=vFile.Get(k)
        if not isinstance(rTree,TTree):
            continue
        if not isinstance(vTree,TTree):
            continue
        print "Comparing TTree",k
        (good,bad)=diffTTree(rTree,vTree)
        nGood+=good
        nBad+=bad
    
