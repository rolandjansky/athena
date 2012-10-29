#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# BunchgroupHandler
#
# David Berge - October 2010
#
# Contents:
# BunchgroupHandler - utility tool to access bunch-group information from COOL
#

import sys
import math
import types
import numpy

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen

class BunchgroupRunData:

    def __init__(self,run):

        self.run = run
        # physics bunch group BCIDs, keyed by LB
        self.physicsBunchgroupBcids = {}
        # BCID lookup, keyed by LB
        self.bcidLookupBefore = {}
        # BCID lookup, keyed by LB
        self.bcidLookupAfter = {}
        # LB lookup
        self.lbLookup = {}

        self.verbose = False

    def createLookup(self,lb,bcidRange):
        if self.verbose:
            print "createLookup called"
        self.bcidLookupBefore[lb] = []
        self.bcidLookupAfter[lb] = []
        for bcid in range(3564):
            bcidListBefore = []
            bcidListAfter = []
            for offset in range(1,bcidRange+1):
                checkBcid = bcid + offset
                if checkBcid > 3563: 
                    checkBcid -= 3564
                if checkBcid in self.physicsBunchgroupBcids[lb]:
                    bcidListAfter.append(1)
                else:
                    bcidListAfter.append(0)
            self.bcidLookupAfter[lb].append(bcidListAfter)

            for offset in range(1,bcidRange+1):
                checkBcid = bcid - offset
                if checkBcid < 0: 
                    checkBcid += 3564
                if checkBcid in self.physicsBunchgroupBcids[lb]:
                    bcidListBefore.append(1)
                else:
                    bcidListBefore.append(0)
            self.bcidLookupBefore[lb].append(bcidListBefore)

        if self.verbose:
            print "Bg run data, Run",self.run,"LB",lb
            for bcid in range(3564):                
                print "BCID",bcid,"before",self.bcidLookupBefore[lb][bcid]
                print "BCID",bcid,"after",self.bcidLookupAfter[lb][bcid]

class BunchgroupHandler:

    def __init__(self):

        # Database parameters
        self.trigProdDbName = 'COOLONL_TRIGGER/COMP200'
        self.bunchGroupContentFolder = '/TRIGGER/LVL1/BunchGroupContent'
        self.bunchGroupDescFolder = '/TRIGGER/LVL1/BunchGroupDescription'

        # Database handle
        self.trigProdDb = None
        
        # self.verbose = False
        self.verbose = True

        # set physics bunch group
        self.physicsBunchgroupBit = 1

        # run objects
        self.runObjects = dict()

        # remember how many runs we have, keep only runsToArchive in memory
        self.runArchive = []
        # number of runs we keep in memory
        self.runsToArchive = 20

        # BCID range
        self.bcidRange = 32

    def openDB(self):
        # Open the trigger COOL database
        try:
            self.trigProdDb = indirectOpen(self.trigProdDbName, True, False, False)
            if self.verbose: print 'Connected to', self.trigProdDbName, 'for Trigger data'
        except Exception, e:
            print e
            return False
        return True

    def closeDB(self):
        # Close the database
        if self.trigProdDb != None:
            self.trigProdDb.closeDatabase()
        self.trigProdDb = None

    def loadRunData(self, run):        
        self.openDB()
        self.loadBunchgroups(run)
        self.closeDB()
        if len(self.runArchive) > self.runsToArchive:
            if self.verbose:
                print "Remove run",self.runArchive[0],"from memory"
            del self.runObjects[self.runArchive[0]]
            self.runArchive.pop(0)

    def loadBunchgroups(self, run):

        self.runObjects[run] = BunchgroupRunData(run)
        self.runArchive.append(run)
        self.runObjects[run].verbose = self.verbose

        if self.verbose:
            print 'loadBounchgroups called, run',run

        bgFolder = self.trigProdDb.getFolder(self.bunchGroupContentFolder)
        itr = bgFolder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
        while itr.goToNext() :
            obj=itr.currentRef()
            lb=(obj.since() & 0xffff)
            self.runObjects[run].physicsBunchgroupBcids[lb] = []
            bgcont = obj.payload()['BunchCode']
            for bcid in range(3564): 
                if(bgcont[bcid]>>self.physicsBunchgroupBit & 1): self.runObjects[run].physicsBunchgroupBcids[lb].append(bcid)
            self.runObjects[run].createLookup(lb,self.bcidRange)
            if self.verbose:
                print "LB",lb,"BCIDs",self.runObjects[run].physicsBunchgroupBcids[lb]

    def getTrainPosition(self,run,lb,bcid):
        
        if self.verbose:
            print 'getTrainPosition called'
        before, after = self.getNeighbourBcids(run,lb,bcid)
        return (len(before)+1)

    def getNeighbourBcids(self,run,lb,bcid):
        
        if self.verbose:
            print 'getNeighbourBcids called'

        before, after = self.getNeighbourPattern(run,lb,bcid)

        l1 = []
        for bcidOffset in range(len(before)):
            checkBcid = bcid - (bcidOffset+1)
            if checkBcid < 0: checkBcid += 3564
            if before[bcidOffset]: 
                if self.verbose:
                    print "BCID",checkBcid,"is filled"
                l1.append(checkBcid)
        l2 = []
        for bcidOffset in range(len(after)):
            checkBcid = bcid + (bcidOffset+1)
            if checkBcid > 3563: checkBcid -= 3564
            if after[bcidOffset]:
                if self.verbose:
                    print "BCID",checkBcid,"is filled"
                l2.append(checkBcid)
        return l1, l2


    def getNeighbourPattern(self,run,lb,bcid):
        
        if self.verbose:
            print 'getNeighbourPattern called'

        lbList = []
        try:
            lbList = self.runObjects[run].physicsBunchgroupBcids.keys()
        except KeyError:
            self.loadRunData(run)
            lbList = self.runObjects[run].physicsBunchgroupBcids.keys()

        latestLb = -1
        try:
            latestLb = self.runObjects[run].lbLookup[lb]
        except KeyError:
            for testLb in sorted(lbList):
                if latestLb < 0: latestLb = testLb
                if lb > testLb: latestLb = testLb
            if self.verbose:
                print "LB",lb,"latest LB",latestLb
            self.runObjects[run].lbLookup[lb] = latestLb

        return (self.runObjects[run].bcidLookupBefore[latestLb][bcid], self.runObjects[run].bcidLookupAfter[latestLb][bcid])

if __name__ == '__main__':
    
    import sys,getopt,os, math

    def usage():
        print "%s" % sys.argv[0]
        print "  --help         this printout"
        print "  --run <runNo>  run number"
        print "  --bcid <bcid>  BCID"
        print "  --lb <lb>      LB"

    try:
        longopts=['run=','bcid=','lb=','help']
        opts,args=getopt.getopt(sys.argv[1:],'',longopts)
    except getopt.GetoptError,e:
        usage()
        sys.exit(2)

    run = -1
    lb = -1
    bcid = -1

    for o,a in opts:
        if o in ('-h','--help'):
            usage()
            sys.exit(0)
        if o == '--run':
            run = int(a)
        if o == '--bcid':
            bcid = int(a)
        if o == '--lb':
            lb = int(a)

    if (run == -1 or bcid == -1 or lb == -1) : 
        usage()
        sys.exit(2)

    bg = BunchgroupHandler()
    bg.verbose = False
    bg.loadRunData(run)

    l1, l2 = bg.getNeighbourBcids(run,lb,bcid)
    print "Run",run,"neighbouring filled BCIDs for BCID",bcid,":",sorted(l1), sorted(l2)
