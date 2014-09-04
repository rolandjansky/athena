#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    diffTAGTree.py
# @purpose: Check that 2 TAG files have same content.
#           (Based on original script from PROC, modified to deal with 
#           prodsys generated TAGs by Graeme.)
# @author:  Graeme Andrew Stewart <graeme.andrew.stewart@cern.ch>
# @date:    April 2012
#
# @example:
#
# diffTAGTree.py oldTAG newTAG
#

__version__ = "$Revision: 1.3 $"
__author__  = "Graeme Andrew Stewart"


import sys
import os
sys.argv += [ '-b' ] # tell ROOT to not use graphics
from ROOT import TFile, TTree
sys.argv.pop()

setIgnoreLeaves=("Token","StreamESD_ref","StreamRDO_ref","StreamAOD_ref","RecoTimeRAWtoESD","RecoTimeESDtoAOD")

def diffTTree(tOld,tNew,details=None): 
    nOld = tOld.GetEntriesFast()
    nNew = tNew.GetEntriesFast()
    n=min(nOld,nNew)

    if nOld != nNew:
        msg="Different number of entries: %i vs %i. Comparing first %i" % \
            (nOld,nNew,n)
        print msg
        if details is not None: details.write(msg+"\n")

    leavesOld=tOld.GetListOfLeaves()
    leavesNew=tNew.GetListOfLeaves()
    
    checkLeavesOld=set()
    
    for l in leavesOld:
        name=l.GetName()
        if not name in setIgnoreLeaves:
            checkLeavesOld.add(name)

    checkLeavesNew=set()
    for l in leavesNew:
        name=l.GetName()
        if not name in setIgnoreLeaves:
            checkLeavesNew.add(name)
    
    #print checkLeavesOld
    checkLeaves=checkLeavesOld & checkLeavesNew
    
    diffLeaves=checkLeavesOld ^ checkLeavesNew
    if len(diffLeaves):
        msg="The following variables exist in only one tree, not compared:\n"
        for d in diffLeaves:
            msg+=d+"\n"
        print msg
        if details is not None: details.write(msg)

    nGood=0
    nBad=0
    diffSummary=dict()

    # To cope with events being out of order in the new TAG file
    # (which can happen when running through prodsys) build up
    # a cache for the new TAG file, mapping run/event numbers -> index
    newRunEventDict=dict()  # Hold information about the run/event numbers vs entry index here
    cachedIndex=0           # How far we looked through the new file already
    
    # Loop over events in the old file
    for iEntry in range(n):
        tOld.GetEntry(iEntry)

        try:
            evOld=tOld.EventNumber
            runOld=tOld.RunNumber
            # Simple index combining run and event
            runEventIndex = "%d-%d" % (runOld, evOld)
#            print "Trying to match %i-%i" % (runOld, evOld)
            if runEventIndex in newRunEventDict:
#                print "Cache hit!"
                tNew.GetEntry(newRunEventDict[runEventIndex])
                evNew=tNew.EventNumber
                runNew=tNew.RunNumber
            else:
                for sEntry in range(cachedIndex, n):
                    tNew.GetEntry(sEntry)
                    evNew=tNew.EventNumber
                    runNew=tNew.RunNumber
                    newRunEventDict["%d-%d" % (runNew, evNew)] = sEntry
#                    print "Cached %i-%i" % (runNew, evNew)
                    cachedIndex = sEntry + 1
                    if evNew == evOld and runNew == runOld:
                        break
            if evOld != evNew or runOld != runNew:
                msg="Run/Event numbers don't match: found no partner for RunNbr: %i, EventNbr: %i\n" % (runOld, evOld)
                msg+="\nStop comparison now."
                print msg
                if details is not None:
                    details.write(msg+"\n")
                break
        except AttributeError:
            pass

        foundDiff=False
        for name in checkLeaves:
            #exec "vOld=tOld."+name
            vOld=getattr(tOld,name)
            vNew=getattr(tNew,name)
        
            if vOld != vNew:
                foundDiff=True
                try: #Get Run/Event number
                    evt=tNew.EventNumber
                    rn=tNew.RunNumber

                    evtO=tOld.EventNumber
                    rnO=tOld.RunNumber
                    
                    evId="(Run %i, Evt %i)" % (rn, evt)
                    evId+="(Run %i, Evt %i)" % (rnO, evtO)
                except:
                    evId=""
                    
                #print "Event #",iEntry,"Difference:",name,
                diffmsg="Event #%i %s Difference: %s %s -> %s" %\
                    (iEntry,evId,name,str(vOld),str(vNew))
                #print vOld,"vs",vNew,
                try:
                    d=100.0*(vNew-vOld)/vOld
                    diffmsg+=" (%.3f%%)" % d
                except:
                    pass
                if details is not None:
                    details.write(diffmsg+"\n")
                else:
                    print diffmsg

                if diffSummary.has_key(name):
                    diffSummary[name]+=1
                else:
                    diffSummary[name]=1
        if foundDiff:
            nBad+=1
        else:
            nGood+=1

    msg="Found %i identical events and %i different events" % (nGood,nBad)
    print msg
    if details is not None:
        details.write(msg+"\n")
        
    for n,v in diffSummary.iteritems():
        msg="\tName: %s: %i Events differ" % (n,v)
        print msg
        if details is not None:
            details.write(msg+"\n")

    return (nGood,nBad)

if __name__=='__main__':

    if len(sys.argv)<3 or len(sys.argv)>4 or sys.argv[1]=="-help":
        print "Usage:",sys.argv[0],"File1 File2 <treename>"
        sys.exit(-1)

    fnOld=sys.argv[1]
    fnNew=sys.argv[2]
    if len(sys.argv)>3:
        treename=sys.argv[3]
    else:
        treename="POOLCollectionTree"

    if not os.access(fnOld,os.R_OK):
        print "Can't access file",fnOld
        sys.exit(-1)
        
    if not os.access(fnNew,os.R_OK):
        print "Can't access file",fnNew
        sys.exit(-1)


    fOld = TFile(fnOld)
    if fOld is None:
        print "Failed to open file",fnOld
        
    tOld = fOld.Get(treename)
    if tOld is None:
        print "Tree",treename,"not found in file",fnOld
        sys.exit(-1)

    fNew = TFile(fnNew)
    if fNew is None:
        print "Failed to open file",fnNew

    tNew = fNew.Get(treename)
    if fNew is None:
        print  "Tree",treename,"not found in file",fnNew
 
    ndiff=diffTTree(tOld,tNew)

