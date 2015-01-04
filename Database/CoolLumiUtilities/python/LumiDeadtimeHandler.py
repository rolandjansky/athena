#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# LumiDeadtimeHandler
#
# Eric Torrence - June 2011
#
# Contents:
#
# LumiDeadtimeHandler - function to determine L1 deadtime from trigger information
#

# Get our global DB handler object
from CoolLumiUtilities.LumiDBHandler import LumiDBHandler
from CoolLumiUtilities.CoolDataReader import CoolDataReader

from CoolLumiUtilities.LumiBlobConversion import unpackLiveFraction, unpackBunchGroup

class LumiDeadtimeHandler:

    # Constructor
    def __init__(self):

        self.verbose = False
        
        # Data readers
        self.vetoReader = None
        self.menuReader = None
        self.countsReader = None
        
        # Trigger channels to read for live fractions
        self.trigList=['L1_EM30']
        
        # Storage of per-BCID deadtime (index by [runlb][bcid])
        self.liveFracBCID = dict()

        # Storage of L1 trigger counts deadtime (index by [runlb][trigname])
        self.liveFracTrig = dict()
        
        # Storage for L1 trigger name -> value mapping 
        self.trigChan = dict()

    def calculateAll(self, run):
        self.loadData(run)
        self.findBCIDDeadtime()
        
    def loadData(self, run):
        self.loadVetoData(run)
        self.loadTrigChannels(run)
        self.loadTrigCounts(run)
        
    def loadVetoData(self, run):
        if self.verbose: print 'Loading trigger veto data'

        # Instantiate new COOL data reader if not already done
        if self.vetoReader == None:
            self.vetoReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/PerBcidDeadtime')

        self.vetoReader.setIOVRangeFromRun(run)
        self.vetoReader.readData()

        if self.verbose:
            print 'Read %d Trig veto records' % len(self.vetoReader.data)

    # Read trigger channel mappings
    # Fills self.trigChan based on values in self.trigList
    def loadTrigChannels(self, run):
        if self.verbose: print 'Loading trigger channel data'
        
        self.trigChan = dict()
        for trig in self.trigList:
            self.trigChan[trig] = -1

        if self.menuReader == None:
            self.menuReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LVL1/Menu')
            
        self.menuReader.setIOVRangeFromRun(run)
        self.menuReader.readData()

        for obj in self.menuReader.data:
            trigName = obj.payload()['ItemName']
            if trigName in self.trigList:
                self.trigChan[trigName] = int(obj.channelId())
                    
        for trig in self.trigList:
            if self.trigChan[trig] == -1:
                print "Couldn't find", trig, "in run", run

            if self.verbose:
                print 'Found', trig, 'in channel', self.trigChan[trig] 


    # Load all trigger counts for the given run
    # Fills counts for all triggers with channels found in self.trigChan
    def loadTrigCounts(self, run):

        if self.verbose:
            print 'loading Trigger Counts data'

        self.liveFracTrig = dict()
        
        if self.countsReader == None:    
            self.countsReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LVL1COUNTERS')

        self.countsReader.setIOVRangeFromRun(run)

        # Build channel list
        chanList = self.trigChan.values()
        chanList.sort()

        # Skip any trigger we didn't find
        tmpList = []
        for chan in chanList:
            if chan < 0: continue
            tmpList.append( chan )
        chanList = tmpList

        self.countsReader.setChannel(chanList)
        self.countsReader.readData()

        for obj in self.countsReader.data:
            
            #if self.verbose:
            #    print obj.since()>>32, '/', obj.since()&0xFFFFFFFF, obj.channelId(), obj.payload()['BeforePrescale'], obj.payload()['AfterPrescale'], obj.payload()['L1Accept']
            
            # use the string as the dictionary key
            ss = obj.since()

            if not ss in self.liveFracTrig:
                self.liveFracTrig[ss] = dict()
                
            for (trig, chan) in self.trigChan.iteritems():
                if chan != obj.channelId(): continue

                ratio = 0.
                if obj.payload()['AfterPrescale'] > 0:
                    ratio = float(obj.payload()['L1Accept'])/obj.payload()['AfterPrescale']

                self.liveFracTrig[ss][trig] = ratio    

                if self.verbose:
                    print obj.since()>>32, '/', obj.since()&0xFFFFFFFF, trig, ratio
                    
    def findBCIDDeadtime(self):
        if self.verbose: print 'Calculating per-BCID deadtime'

        # First dictionary index is lumiblock IOV
        self.liveFracBCID = dict()

        # Loop over each lumi block
        for obj in self.vetoReader.data:

            key = obj.since()

            run = key >> 32
            lb = key & 0xFFFFFFFF
            bloblength = obj.payload()['HighPriority'].size()

            #if self.verbose:
            #    print '%d %d Found trigger counter blob of length %d' % (run, lb, bloblength)

            # Unpack High Priority blob here
            liveVec = unpackLiveFraction(obj.payload())
            self.liveFracBCID[key] = liveVec
            
            # Each BCID is one 24-bit integer
            #if self.verbose:
            #
            #    for i in range(10):
            #        print 'BICD: %d Live: %f' % (i+1, liveVec[i])
                        
    
if __name__ == '__main__':
    odh = LumiDeadtimeHandler()
    odh.loadData(183128)
    odh.findBCIDDeadtime()
    
