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


import argparse
import sys
import os
import logging
sys.argv += [ '-b' ] # tell ROOT to not use graphics
from ROOT import TFile, TTree
sys.argv.pop()

global msg
msg = logging.getLogger('diffTAGTree')
hdlr = logging.StreamHandler(sys.stdout)
frmt = logging.Formatter("%(levelname)s %(message)s")
hdlr.setFormatter(frmt)
msg.addHandler(hdlr)
msg.setLevel(logging.INFO)

setIgnoreLeaves=("Token","StreamESD_ref","StreamRDO_ref","StreamAOD_ref","RecoTimeRAWtoESD","RecoTimeESDtoAOD","RecoTimeRAWtoALL","RandomNumber")

def getEventDict(tagHandle, name):
    duplicates = False
    indexDict = {}
    n=tagHandle.GetEntriesFast()
    for i in range(n):
        tagHandle.GetEntry(i)
        idx = "{0}-{1}".format(tagHandle.RunNumber, tagHandle.EventNumber)
        msg.debug("New file entry in {0} file {1} is #{2}".format(name, idx, i))
        if idx in indexDict.keys():
            duplicates = True
            msg.error("Error - found duplicated event in file {0}: {1} appears at evt no. {2} and {3}!".format(name, idx, indexDict[idx], i))
        else:
            indexDict[idx] = i
    return indexDict, duplicates
    

def diffTTree(tOld,tNew):
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
    
    checkLeaves=checkLeavesOld & checkLeavesNew
    
    diffLeaves=checkLeavesOld ^ checkLeavesNew
    if len(diffLeaves):
        msg.warning("The following variables exist in only one tree, not compared: {0}".format(" ".join(diffLeaves)))

    nGood=0
    nBad=0
    diffSummary=dict()

    # To cope with events being out of order in the new TAG file
    # (which can happen when running through prodsys or using AthenaMP) build up
    # a cache for each TAG file, mapping run/event numbers -> index
    # Also do a sanity check for duplicated events
    newRunEventDict, ndups=getEventDict(tNew, 'new')
    oldRunEventDict, odups=getEventDict(tOld, 'old')
    if ndups or odups:
        msg.error("There are serious problems with these tag files, but script will compare content of events")

    # Some cross checks on the events we have
    nk=set(newRunEventDict.keys())
    ok=set(oldRunEventDict.keys())
    mismatches = nk ^ ok
    if len(mismatches) > 0:
        msg.error("Inconsistent events in TAG files, these run-event pairs are only in one of the files: {0}".format(mismatches))

    # Loop over events in the old file
    for idx, iEntry in oldRunEventDict.iteritems():
        tOld.GetEntry(iEntry)

        try:
            evt=tOld.EventNumber
            run=tOld.RunNumber
            tNew.GetEntry(newRunEventDict[idx])
            msg.debug("Found {0} at entry {1} in old file, {2} in new file".format(idx, iEntry, newRunEventDict[idx]))
        except KeyError, e:
            msg.error("Run-Event number {0} was not found in the new TAG file")
            nBad+=1
            continue
            
        foundDiff=False
        for name in checkLeaves:
            vOld=getattr(tOld,name)
            vNew=getattr(tNew,name)
        
            if vOld != vNew:
                foundDiff=True
                diffmsg="Run {1}, Event {0} (old#{2}, new#{3}) Difference: {4} {5} -> {6}".format(evt, run, iEntry, newRunEventDict[idx],
                                                                                                  name, str(vOld),str(vNew))
                try:
                    d=100.0*(vNew-vOld)/vOld
                    diffmsg+=" (%.3f%%)" % d
                except:
                    pass
                else:
                    print diffmsg

                if name in diffSummary:
                    diffSummary[name]+=1
                else:
                    diffSummary[name]=1
        if foundDiff:
            nBad+=1
        else:
            nGood+=1

    message = "Found %i identical events and %i different (or missing) events" % (nGood,nBad)
    if nBad > 0:
        msg.error(message)
    else:
        msg.info(message)
        
    for n,v in diffSummary.iteritems():
        msg.warning("Name: %s: %i Events differ" % (n,v))

    return (nGood,nBad)

if __name__=='__main__':

    parser = argparse.ArgumentParser(description = "Examine a pair of tag files for any differences")    
    parser.add_argument('--debug', help='Set debugging output on', action='store_true')
    parser.add_argument('--tree', help='Alternative tree to check (default is POOLCollectionTree)', 
                        default='POOLCollectionTree')
    parser.add_argument('tagFile', help='TAG files to check', nargs=2)

    args = vars(parser.parse_args(sys.argv[1:]))

    fnOld=args['tagFile'][0]
    fnNew=args['tagFile'][1]
    treename=args['tree']
    if args['debug']:
        msg.setLevel(logging.DEBUG)

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

    if ndiff[1] != 0:
        sys.exit(1)
    
    sys.exit(0)
