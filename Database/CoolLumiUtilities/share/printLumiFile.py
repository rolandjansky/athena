#!/usr/bin/env python
#
# printLumiFile.py
#
# Utility to dump a text file with luminosity and other useful parambers by Run/LB and time stamp
#
#
import os
import sys
import array
import time, calendar

from optparse import OptionParser

# Get our global DB handler object
from CoolLumiUtilities.CoolDataReader import CoolDataReader
from CoolLumiUtilities.LumiDBHandler import LumiDBHandler
from CoolLumiUtilities.LumiDBCache import LumiDBCache

class LumiDumper:

    def __init__(self):

        # Control output level
        self.verbose = True

        # Channel to chose (preferred by default)
        self.lumiChan = 0
        
        # Run list
        self.runList = []

        # Luminosity tag
        self.lumiTag = 'OflLumi-7TeV-002'

        # Use online instead
        self.online = False
        
        # Instantiate the LumiDBHandler, so we can cleanup all COOL connections in the destructor
        self.dbHandler = LumiDBHandler()

        # Output file (default stdout)
        self.outFile = None

        # Output directory (default none - pwd)
        self.outDir = None

        # Write stable beams only
        self.checkStable = True

        # Predefine data readers
        self.lumi = None
        self.fill = None
        self.ardy = None
        self.lhc  = None
        self.lblb = None
        
    # Explicitly clean up our DB connections
    def __del__(self):
        self.dbHandler.closeAllDB()
        
    # Called in command-line mode
    def execute(self):

        # Handle command-line switches
        self.parseOpts()

        # Process each run in the runlist
        for runnum in self.runList:
            
            # Read all COOL data
            self.readData(runnum)

            self.printData(runnum)

    def parseOpts(self):

        parser = OptionParser(usage="usage: %prog [options]", add_help_option=False)

        parser.add_option("-?", "--usage", action="store_true", default=False, dest="help",
                          help="show this help message and exit")
        
        parser.add_option("-v", "--verbose",
                     action="store_true", default=self.verbose, dest="verbose",
                     help="turn on verbose output")

        parser.add_option("-r", "--run",
                          dest="runlist", metavar="RUN",
                          help="Specific run, range, or comma-separated list of both")
        
        parser.add_option("--channel",
                          dest='chan', metavar='N', default=self.lumiChan,
                          help = 'specify luminosity channel (default: %d)' % self.lumiChan)

        parser.add_option("--lumiTag",
                          dest='lumitag', metavar='TAG', default=self.lumiTag,
                          help = 'specify luminosity tag (default: %s)' % self.lumiTag)

        parser.add_option("--online",
                          action='store_true', default=self.online, dest='online',
                          help='use online luminosity (default: use offline)')

        parser.add_option("--outDir",
                          dest='outdir', metavar='DIR', default=self.outDir,
                          help = 'change default output directory')

        parser.add_option("--noStable",
                          action="store_false", default=self.checkStable, dest="checkstable",
                          help="write non-stable beam luminosity (default: stable only)")

        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        if options.verbose: self.verbose = options.verbose
            
        self.lumiChan = int(options.chan)
        self.lumiTag = options.lumitag
        self.online = options.online
        self.outDir = options.outdir
        self.checkStable = options.checkstable

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

    def readData(self, runnum):

        # Luminosity folder
        if self.verbose: print 'Reading lumi for run %d' % runnum
        if self.lumi == None:
            if self.online:
                self.lumi = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBLESTONL')
            else:
                self.lumi = CoolDataReader('COOLOFL_TRIGGER/COMP200', '/TRIGGER/OFLLUMI/LBLESTOFL')
                self.lumi.setTag(self.lumiTag)

            self.lumi.setChannelId(self.lumiChan)
               
        self.lumi.setIOVRangeFromRun(runnum)
        self.lumi.readData()

        if self.verbose:
            print 'Read %d Luminosity records' % len(self.lumi.data)

        # Load stable beam flag (
        if self.verbose: print 'Reading ATLAS ready flag'
        if self.ardy == None:
            self.ardy = LumiDBCache('COOLONL_TDAQ/COMP200', '/TDAQ/RunCtrl/DataTakingMode')

        self.ardy.reader.setIOVRangeFromRun(runnum)
        self.ardy.reader.readData()

        if self.verbose:
            print 'Read %d ATLAS ready records' % len(self.ardy.reader.data)
        
        # Load LBLB data (needed to convert to time)
        if self.verbose: print 'Reading LBLB data'
        if self.lblb == None:
            self.lblb = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBLB')

        self.lblb.setIOVRangeFromRun(runnum)
        self.lblb.readData()

        if self.verbose:
            print 'Read %d LBLB records' % len(self.lblb.data)
            if len(self.lblb.data) > 0:
                print 'First LB %d/%d' % (self.lblb.data[0].since() >> 32, self.lblb.data[0].since() & 0xFFFFFFFF)
                print 'Last  LB %d/%d' % (self.lblb.data[-1].since() >> 32, self.lblb.data[-1].since() & 0xFFFFFFFF)

        # Now figure out starting/ending time
        if len(self.lblb.data) < 1:
            print 'No LBLB data found!'
            return # Nothing to work with

        tlo = self.lblb.data[0].payload()['StartTime']
        thi = self.lblb.data[-1].payload()['EndTime']

        # Fillparams (slow)
        if self.verbose: print 'Reading Fillparams'
        if self.fill == None:
            self.fill = LumiDBCache('COOLONL_TDAQ/COMP200', '/TDAQ/OLC/LHC/FILLPARAMS')

        self.fill.reader.setIOVRange(tlo, thi)
        self.fill.reader.readData()

        if self.verbose:
            print 'Read %d FILLPARAMS records' % len(self.fill.reader.data)
            
        # LHC information (for stable beams)
        if self.verbose: print 'Reading LHC information'
        if self.lhc == None:
            self.lhc = LumiDBCache('COOLOFL_DCS/COMP200', '/LHC/DCS/FILLSTATE')

        self.lhc.reader.setIOVRange(tlo, thi)
        self.lhc.reader.readData()

        if self.verbose:
            print 'Read %d LHC records' % len(self.lhc.reader.data)
            
    def printData(self, runnum):

        # Only proceed if we have actual lumi data
        if len(self.lumi.data) == 0:
            return

        f = None
 
        # Make time map
        lblbMap = dict()
        for obj in self.lblb.data:
            lblbMap[obj.since()] = (obj.payload()['StartTime'], obj.payload()['EndTime'])
            
        # OK, now we want to go through the luminosity records and match the other data
        for obj in self.lumi.data:

            run = obj.since() >> 32
            lb  = obj.since() & 0xFFFFFFFF
            startTime = lblbMap.get(obj.since(), (0., 0.))[0]
            endTime   = lblbMap.get(obj.since(), (0., 0.))[1]
            
            lumi = obj.payload()['LBAvInstLumi']
            mu = obj.payload()['LBAvEvtsPerBX']

            payload = self.fill.getPayload(startTime)
            if payload == None:
                ncol = 0
            else:
                ncol = payload['LuminousBunches']

            payload = self.lhc.getPayload(startTime)
            if payload == None:
                stable = 0
            else:
                stable = payload['StableBeams']

            payload = self.ardy.getPayload(obj.since())
            if self.ardy == None:
                ready = 0
            else:
                ready = payload['ReadyForPhysics']

            if self.checkStable and not stable: continue

            # Open file if not open already
            if f == None:
            
                # Open
                if self.outDir != None:
                    self.outFile = '%s/run%d.txt' % (self.outDir, runnum)
                else:
                    self.outFile = 'run%d.txt' % runnum
                    
                print 'Writing file %s' % self.outFile
                f = open(self.outFile, 'w')

            # OK, print it out
            print >>f, run, lb, startTime/1.E9, endTime/1.E9, lumi, mu, ncol, ready

        # Close file
        if f != None:
            f.close()

        
# Executed from the command line
if __name__ == '__main__':
    ld = LumiDumper()
    ld.execute()


                
