#!/usr/bin/env python
#
# compareLumi.py
#
# Utility to read luminosity from two sources and compare, both on a run-by-run basis
# but also on a lumi block by lumiblock basis.  Will also check for missing records.
#
# Eric Torrence - 3 June 2012
#
import os
import sys
import glob
import time
import array
import shutil
import subprocess

from optparse import OptionParser

# Get our global DB handler object
from CoolLumiUtilities.LumiDBHandler import LumiDBHandler
from CoolLumiUtilities.CoolDataReader import CoolDataReader

from PyCool import cool

class CompareLumi:

    def __init__(self):

        # Control output level
        self.verbose = False

        # Channels for comparision
        self.lumi1Channel = 0
        self.lumi2Channel = 261
        self.lumi1Tag = 'OflLumi-7TeV-002'
        self.lumi2Tag = 'OflLumi-7TeV-003'

        # Difference (in percent) to complain about
        self.threshold = 5.
        
        # Stable only
        self.stableOnly = True

        # List of IOVRanges with stable beams
        self.stableTime = []

        # ATLAS Ready only
        self.readyOnly = True

        #Dict of (runlblo, runlblbhi) pairs giving extent of ATLAS ready
        self.readyTime = []
        
        # List of (integer) run numbers specified on the command line
        self.runList = []

        # Utlity routine for handling COOL connections
        self.dbHandler = LumiDBHandler()

        # Data readers
        self.lumi1Reader = None
        self.lumi2Reader = None
        self.lblbReader = None
        self.lhcReader  = None
        self.rdyReader  = None
        
    # Explicitly clean up our DB connections
    def __del__(self):
        self.dbHandler.closeAllDB()
        
    # Called in command-line mode
    def execute(self):

        # Handle command-line switches
        self.parseOpts()

        lumi1Sum = dict()
        lumi2Sum = dict()
        
        # Process each run in self.runList
        for run in self.runList:

            # Load all data from COOL
            self.loadCOOLData(run)

            # Skip if run doesn't exist
            if len(self.lblbReader.data) == 0: continue
            
            # Find stable beams, output goes to self.stableTime
            self.findStable()

            # Find ATLAS ready, result goes to self.readyTime
            self.findReady()
            
            if self.stableOnly and len(self.stableTime) == 0: continue
            if self.readyOnly and len(self.readyTime) == 0: continue
            
            # Storage objects keyed by [lb]
            lumi1Dict = dict()
            lumi2Dict = dict()

            lumi1Sum[run] = 0.
            lumi2Sum[run] = 0.

            # Keep track of LB range
            lbLo = 99999
            lbHi = 0
            
            # Loop over all objects and sort by runlb and algo
            for obj in self.lumi1Reader.data:
                runlb = obj.since()
                lumi1Dict[runlb] = obj.payload()

            for obj in self.lumi2Reader.data:
                runlb = obj.since()
                lumi2Dict[runlb] = obj.payload()

                # if self.verbose:
                #     run = runlb >> 32
                #     lb = runlb & 0xFFFFFFFF
                #     print 'Found Run %d LB %d chan %d' % (run, lb, channel)

            # Loop over defined data
            for obj in self.lblbReader.data: 

                runlb = obj.since()
                run = runlb >> 32
                lb = runlb & 0xFFFFFFFF

                startTime = obj.payload()['StartTime']
                endTime = obj.payload()['EndTime']
                dtime = (endTime-startTime)/1E9
                    
                # Check whether this is in stable beams
                isStable = False
                for stable in self.stableTime:
                    if not stable[0] <= startTime < stable[1]: continue
                    isStable = True
                    break
                
                if self.stableOnly and not isStable: continue

                # Check whether this in in ATLAS ready
                isReady = False
                for ready in self.readyTime:
                    if not ready[0] <= runlb < ready[1]: continue
                    isReady = True
                    break
                
                #if self.verbose:
                #    print 'Found stable Run %d LB %d' % (run, lb)

                if lb < lbLo: lbLo = lb
                if lb > lbHi: lbHi = lb
                
                # Check if lumi record exists
                missing1 = False
                missing2 = False
                if runlb not in lumi1Dict:
                    missing1 = True

                if runlb not in lumi2Dict:
                    missing2 = True

                    
                if missing1 and missing2:
                    if isStable:
                        print 'Run: %d LB: %4d does not have any luminosity record in stable beams!' % (run, lb)
                    else:
                        print 'Run: %d LB: %4d does not have any luminosity record!' % (run, lb)
                    continue

                elif missing1:
                    if isStable:
                        print 'Run: %d LB: %4d does not have first lumi record in stable beams!' % (run, lb)
                    else:
                        print 'Run: %d LB: %4d does not have first lumi record!' % (run, lb)

                elif missing2:
                    if isStable:
                        print 'Run: %d LB: %4d does not have second lumi record in stable beams!' % (run, lb)
                    else:
                        print 'Run: %d LB: %4d does not have second lumi record!' % (run, lb)
                        
                    
                
                # Get lumi
                lumi1 = 0.
                valid1 = 999
                chan1 = self.lumi1Channel
                
                lumi2 = 0.
                valid2 = 999
                chan2 = self.lumi2Channel
                
                if not missing1:
                    lumi1 = lumi1Dict[runlb]['LBAvInstLumi']
                    valid1 = lumi1Dict[runlb]['Valid'] & 0x3FF
                    if chan1 == 0:
                        chan1 = lumi1Dict[runlb]['Valid'] >> 22
                    lumi1Sum[run] += (lumi1 * dtime)/1.E6

                if not missing2:
                    lumi2 = lumi2Dict[runlb]['LBAvInstLumi']
                    valid2 = lumi2Dict[runlb]['Valid'] & 0x3FF
                    if chan2 == 0:
                        chan2 = lumi2Dict[runlb]['Valid'] >> 22
                    lumi2Sum[run] += (lumi2 * dtime)/1.E6


                try:
                    ratio = 100*(lumi2/lumi1 - 1.)
                except ZeroDivisionError:
                    ratio = -99.
                    
                if self.verbose:
                    print 'Run: %6d LB: %4d Lumi1: %6.1f Chan1: %d Valid1: %d Lumi2: %6.1f Chan2: %d Valid2: %d  Ratio: %.3f%%' % (run, lb, lumi1, chan1, valid1, lumi2, chan2, valid2, ratio)
                    

                # Compare
                if (abs(ratio) > self.threshold and (lumi1>1. or lumi2>1.)): # or valid1 != 0 or valid2 != 0:
                    print '>>> Run: %6d LB: %4d dTime: %f Lumi1: %6.1f Chan1: %d Valid1: %d Lumi2: %6.1f Chan2: %d Valid2: %d  Ratio: %.3f%%' % (run, lb, dtime, lumi1, chan1, valid1, lumi2, chan2, valid2, ratio)
                    
            # End of loop over LBs

            try:
                ratio = 100*(lumi2Sum[run]/lumi1Sum[run] - 1.)
            except ZeroDivisionError:
                ratio = -99.
                    
            print 'Run: %6d LBLo: %4d LBHi: %4d IntLumi1: %8.3f IntLumi2: %8.3f Ratio: %.3f%%' % (run, lbLo, lbHi, lumi1Sum[run], lumi2Sum[run], ratio)
            
        # End of loop over runs                
        totalLumi1 = 0.
        totalLumi2 = 0.
        for lumi in lumi1Sum.itervalues():
            totalLumi1 += lumi

        for lumi in lumi2Sum.itervalues():
            totalLumi2 += lumi

        try:
            ratio = 100*(totalLumi2/totalLumi1 - 1.)
        except ZeroDivisionError:
            ratio = -99.
            
        print 'Total Luminosity1: %8.3f Luminosity2: %8.3f => 2/1 = %.3f%%' % (totalLumi1, totalLumi2, ratio)
        
    # Load all data necessary to make bunch-by-bunch lumi for a single run
    def loadCOOLData(self, runnum):
        if self.verbose:
            print 'LumiChecker.loadCOOLData(%s) called' % runnum

        # Many folders are indexed by time stamp.  Load RunLB -> time conversion here
        self.loadLBLB(runnum)

        if len(self.lblbReader.data) == 0: return

        # Determine start and end time of this run
        startTime = self.lblbReader.data[0].payload()['StartTime']
        endTime =  self.lblbReader.data[-1].payload()['EndTime']
        iov = (startTime, endTime)
        
        # Read LHC Data (for stable beams)
        self.loadLHCData(iov)

        # Read Ready Data
        self.loadReadyData(runnum)
        
        # Read the luminosity
        self.loadLumi(runnum)
        
    def loadLBLB(self, run):
        if self.verbose: print 'Loading LBLB data'

        # Instantiate new COOL data reader if not already done
        if self.lblbReader == None:
            self.lblbReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBLB')

        self.lblbReader.setIOVRangeFromRun(run)
        self.lblbReader.readData()

        if self.verbose:
            print 'Read %d LBLB records' % len(self.lblbReader.data)
            if len(self.lblbReader.data) > 0:
                print 'First LB %d/%d' % (self.lblbReader.data[0].since() >> 32, self.lblbReader.data[0].since() & 0xFFFFFFFF) 
                print 'Last  LB %d/%d' % (self.lblbReader.data[-1].since() >> 32, self.lblbReader.data[-1].since() & 0xFFFFFFFF)
            
    def loadLumi(self, runnum):
        if self.verbose: print 'Loading luminosity values'

        # Instantiate new COOL data reader if not already done
        if self.lumi1Reader == None:
            self.lumi1Reader = CoolDataReader('COOLOFL_TRIGGER/COMP200',  '/TRIGGER/OFLLUMI/LBLESTOFL')

        self.lumi1Reader.setIOVRangeFromRun(runnum)
        self.lumi1Reader.setChannel([self.lumi1Channel])
        self.lumi1Reader.setTag(self.lumi1Tag)
        self.lumi1Reader.readData()

        if self.verbose:
            print 'Read %d Lumi records' % len(self.lumi1Reader.data)
            
        # Instantiate new COOL data reader if not already done
        if self.lumi2Reader == None:
            self.lumi2Reader = CoolDataReader('COOLOFL_TRIGGER/COMP200',  '/TRIGGER/OFLLUMI/LBLESTOFL')

        self.lumi2Reader.setIOVRangeFromRun(runnum)
        self.lumi2Reader.setChannel([self.lumi2Channel])
        self.lumi2Reader.setTag(self.lumi2Tag)
        self.lumi2Reader.readData()

        if self.verbose:
            print 'Read %d Lumi records' % len(self.lumi2Reader.data)
            

    # Information about stable beams
    def loadLHCData(self, iov):
        if self.verbose: print 'Loading LHC information'

        # Instantiate new COOL data reader if not already done
        if self.lhcReader == None:
            self.lhcReader = CoolDataReader('COOLOFL_DCS/COMP200', '/LHC/DCS/FILLSTATE')
            
        self.lhcReader.setIOVRange(iov[0], iov[1])
        self.lhcReader.readData()
            
        if self.verbose:
            print 'Read %d LHC records' % len(self.lhcReader.data)
        
    # Information about ATLAS ready
    def loadReadyData(self, run):
        if self.verbose: print 'Loading ATLAS ready information'

        # Instantiate new COOL data reader if not already done
        if self.rdyReader == None:
            self.rdyReader = CoolDataReader('COOLONL_TDAQ/COMP200', '/TDAQ/RunCtrl/DataTakingMode')
            
        self.rdyReader.setIOVRangeFromRun(run)
        self.rdyReader.readData()
            
        if self.verbose:
            print 'Read %d ATLAS ready records' % len(self.rdyReader.data)
        
    # Fill the stable beam information in the OflLumiRunData object
    def findStable(self):
        if self.verbose: print 'Finding stable beam periods'
        
        # First, fill stable beam time periods for this run
        tlo = cool.ValidityKeyMax
        thi = cool.ValidityKeyMin
        self.stableTime = []
        for obj in self.lhcReader.data:
            if obj.payload()['StableBeams'] == 0: continue

            if tlo > thi:             # First stable beams
                tlo = obj.since()
                thi = obj.until()
            elif thi == obj.since():  # Extension of existing stable beams
                thi = obj.until()
            else:                     # Not contiguous, add old to list and start again
                self.stableTime.append((tlo, thi))
                tlo = obj.since()
                thi = obj.until()

        if tlo < thi:
            self.stableTime.append((tlo, thi))

        if self.verbose:
            print 'Stable beam periods found:', self.stableTime

    # Fill the stable beam information in the OflLumiRunData object
    def findReady(self):
        if self.verbose: print 'Finding ATLAS ready beam periods'
        
        # First, fill stable beam time periods for this run
        tlo = cool.ValidityKeyMax
        thi = cool.ValidityKeyMin
        self.readyTime = []
        for obj in self.rdyReader.data:
            if obj.payload()['ReadyForPhysics'] == 0: continue

            if tlo > thi:             # First ready
                tlo = obj.since()
                thi = obj.until()
            elif thi == obj.since():  # Extension of existing ready
                thi = obj.until()
            else:                     # Not contiguous, add old to list and start again
                self.readyTime.append((tlo, thi))
                tlo = obj.since()
                thi = obj.until()

        if tlo < thi:
            self.readyTime.append((tlo, thi))

        if self.verbose:
            print 'ATLAS ready periods found:', self.readyTime

                    
    def parseOpts(self):

        parser = OptionParser(usage="usage: %prog [options]", add_help_option=False)

        parser.add_option("-?", "--usage", action="store_true", default=False, dest="help",
                          help="show this help message and exit")
        
        parser.add_option("-v", "--verbose",
                     action="store_true", default=self.verbose, dest="verbose",
                     help="turn on verbose output")

        parser.add_option("-r", "--updateRun",
                          dest="runlist", metavar="RUN",
                          help="update specific run, or comma separated list")

        parser.add_option("--tag1",
                           dest="lumi1tag", metavar="TAG", default=self.lumi1Tag,
                           help='first luminosity tag (default: %s)' % self.lumi1Tag)

        parser.add_option("--chan1",
                          dest="lumi1chan", metavar="CHAN", default = self.lumi1Channel,
                          help='first luminosity channel (default: %d)' % self.lumi1Channel)
        
        parser.add_option("--tag2",
                           dest="lumi2tag", metavar="TAG", default=self.lumi2Tag,
                           help='first luminosity tag (default: %s)' % self.lumi2Tag)

        parser.add_option("--chan2",
                          dest="lumi2chan", metavar="CHAN", default = self.lumi2Channel,
                          help='second luminosity channel (default: %d)' % self.lumi2Channel)
        
        parser.add_option("--noStable",
                          action="store_false", default=self.stableOnly, dest="stable",
                          help="turn off stable beams requirement (default: stable only)")
        
        parser.add_option("--noReady",
                          action="store_false", default=self.readyOnly, dest="ready",
                          help="turn off ATLAS ready requirement (default: ready only)")
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.verbose = options.verbose
        self.lumi1Tag = options.lumi1tag
        self.lumi2Tag = options.lumi2tag
        self.stableOnly = options.stable
        self.readyOnly = options.ready
        self.lumi1Channel = int(options.lumi1chan)
        self.lumi2Channel = int(options.lumi2chan)
        
        # Parse run list
        if options.runlist != None:

            # Clear the old one
            self.runList = []
            
            # Can be comma-separated list of run ranges
            runlist = options.runlist.split(',')
            if len(runlist) == 0:
                print 'Invalid run list specified!'
                sys.exit()

            # Go through and check for run ranges
            for runstr in runlist:
                subrunlist = runstr.split('-')
                
                if len(subrunlist) == 1: # Single run
                    self.runList.append(int(subrunlist[0]))
                    
                elif len(subrunlist) == 2: # Range of runs
                    for runnum in range(int(subrunlist[0]), int(subrunlist[1])+1):
                        self.runList.append(runnum)

                else: # Too many parameters
                    print 'Invalid run list segment found:', runstr
                    sys.exit()

            self.runList.sort()
            if self.verbose:
                print 'Finished parsing run list:',
                for runnum in self.runList:
                    print runnum,
                print


# Executed from the command line
if __name__ == '__main__':
    mhm = CompareLumi()
    mhm.execute()

                
