#!/usr/bin/env python
#
# lumiCopy.py
#
# Utility to copy from one COOL channel to another
#
# Eric Torrence - 05 May 2011
#
import os
import sys
import glob
import time
import math
import array
import shutil
import subprocess

from optparse import OptionParser

# Get our global DB handler object
from CoolLumiUtilities.LumiDBHandler import LumiDBHandler
from CoolLumiUtilities.CoolDataReader import CoolDataReader

from CoolLumiUtilities.LumiCalibrator import LumiCalibrator
from CoolLumiUtilities.LumiBlobConversion import unpackBCIDMask, unpackBCIDValues
from CoolLumiUtilities.LumiBlobConversion import unpackLiveFraction, unpackBunchGroup

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import forceOpen,ensureFolder,athenaDesc

from ROOT import TFile, TH1D, TTree, gROOT, AddressOf

class LumiCopy:

    def __init__(self):

        # Control output level
        self.verbose = True

        # Utlity routine for handling COOL connections
        self.dbHandler = LumiDBHandler()

        # Run list
        self.runList = []

        # IOV list
        self.iovList = []
        
        # Input DB parameters
        self.sourceDB = 'COOLOFL_TRIGGER/COMP200'

        # Output DB parameters
        self.outDBFile = 'mycool.db'
        self.oflLumiFolder = '/TRIGGER/OFLLUMI/LBLESTOFL'

        self.outDB = None
        self.coolSpec = None
        self.outFolder = None

        self.sourceTag = 'OflLumi-7TeV-003'
        self.destTag = 'OflLumi-7TeV-003'

        self.sourceChannel = 261
        self.destChannel = 0

        # Data readers
        self.lumiReader = None

        # Force output to be valid
        self.forceValid = False
        
    # Explicitly clean up our DB connections
    def __del__(self):
        self.dbHandler.closeAllDB()
        
    # Called in command-line mode
    def execute(self):

        # Handle command-line switches
        self.parseOpts()

        # Prepare output DB
        if not self.createOutputDB():
            print 'Error creating output DB!'
            sys.exit()
            
        # Process each run in self.runList
        for iov in self.iovList:

            if self.verbose:
                runstart = iov[0]>>32
                lbstart  = iov[0]&0xFFFFFFFF
                runend   = iov[1]>>32
                lbend    = iov[1]&0xFFFFFFFF
                print "Processing %d/%d to %d/%d inclusive" % (runstart, lbstart, runend, lbend)
                
            # Load all data from COOL
            self.loadLumiData(iov)

            # Loop over all objects and sort by runlb and algo
            nstored = 0
            for obj in self.lumiReader.data:
                channel = obj.channelId()
                startTime = obj.since()
                endTime = obj.until()
                valid = obj.payload()['Valid']

                if self.forceValid:
                    valid = 0
                    
                if self.destChannel == 0 and self.sourceChannel != 0:
                    valid = (valid & 0x3FF) + (self.validChannel(self.sourceChannel) << 22)

                elif self.destChannel != 0 and self.sourceChannel == 0:
                    valid = (valid & 0x3FF)

                elif self.destChannel == 0 and self.sourceChannel == 0:
                    # Do this just to clean up any problems with this previously
                    source = valid >> 22
                    valid = (valid & 0x3FF) + (self.validChannel(source) << 22)
                    
                if (valid & 0xFFFFFFFF != valid):
                    print 'Valid:', valid, ' has too many bits, are you trying to use a channel > 1023?'
                    valid = (valid & 0xFFFFFFFF)
                    
                payload = cool.Record(self.coolSpec)
                payload['LBAvInstLumi'] = obj.payload()['LBAvInstLumi'] * self.scale
                payload['LBAvEvtsPerBX'] = obj.payload()['LBAvEvtsPerBX'] * self.scale
                payload['Valid'] = valid
                        
                self.outFolder.storeObject(startTime, endTime, payload, self.destChannel, self.destTag, True)
                nstored += 1

            if self.verbose:
                print 'Found %d lumi blocks' % nstored
                
        # End of loop over iovs
        try:
            if self.verbose:
                print 'Writing data to local DB instance'
            self.outFolder.flushStorageBuffer()
        except Exception, e:
            print e
            print 'Error storing lumi records to local DB!'

    def validChannel(self, channel):

        # Channel ID written to the validity flag can *NOT* be an offline (+50) channel
        # This function checks and subtracts 50 if needed
        base = 100*math.floor(channel/100)
        if channel-base > 50:
            return channel-50

        return channel
        
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

        parser.add_option("--runSince",
                          dest="runsince", metavar="RUN",
                          help="Set minimum run IOV (inclusive), either run or run/LB")

        parser.add_option("--runUntil",
                          dest="rununtil", metavar="RUN",
                          help="Set maximum run IOV (exclusive), either run or run/LB")

        parser.add_option("--sourceDB",
                          dest="sourcedb", metavar="DB", default = self.sourceDB,
                          help="Set source DB (default: %s)" % self.sourceDB)

        parser.add_option("--destFile",
                          dest='destfile', metavar="FILE", default = self.outDBFile,
                          help="Set output DB filename (default: %s)" % self.outDBFile)
        
        parser.add_option("--scale",
                          dest="scale", metavar="SCALE", default = 1.,
                          help="specify multiplicative scale factor")

        parser.add_option("--forceValid",
                          action="store_true", default=self.forceValid, dest="forceValid",
                          help="force output record to have valid=0")
                          
        parser.add_option("--sourceTag",
                           dest="sourcetag", metavar="TAG", default=self.sourceTag,
                           help='source luminosity tag (default: %s)' % self.sourceTag)
        
        parser.add_option("--destTag",
                           dest="desttag", metavar="TAG", default=self.destTag,
                           help='destination luminosity tag (default: %s)' % self.destTag)
        
        parser.add_option("--sourceChan",
                           dest="sourcechan", metavar="NUM", default=self.sourceChannel,
                           help='source luminosity channel (default: %d)' % self.sourceChannel)
        
        parser.add_option("--destChan",
                           dest="destchan", metavar="NUM", default=self.destChannel,
                           help='destination luminosity channel (default: %d)' % self.destChannel)
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.verbose = options.verbose
        self.sourceDB = options.sourcedb
        self.outDBFile = options.destfile
        self.outDBString = "sqlite://;schema=./%s;dbname=COMP200" % self.outDBFile

        self.scale = float(options.scale)
        self.sourceTag = options.sourcetag
        self.destTag = options.desttag
        self.sourceChannel = int(options.sourcechan)
        self.destChannel = int(options.destchan)

        self.forceValid = options.forceValid
        
        #
        # Either use a run list, or use RunLB range
        #
        
        # Parse run list if available
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

            # Add this to the iov list
            for runnum in self.runList:
                self.iovList.append((runnum<<32, (runnum+1)<<32))
                
            return
        
        # Start with full range
        iovStart = cool.ValidityKeyMin
        iovEnd = cool.ValidityKeyMax

        # Try to work out specific run/LB range
        if options.runsince != None:

            spl = options.runsince.split('/')
            if len(spl) == 1:
                iovStart = int(spl[0])<<32
            elif len(spl) == 2:
                iovStart = (int(spl[0])<<32) + (int(spl[1]) & 0xFFFFFFFF)
            else:
                print 'Run specification invalid: %s' % options.runsince

            
        if options.rununtil != None:
            spl = options.rununtil.split('/')
            # Subtract 1 to make this an exclusive IOV range
            if len(spl) == 1:
                iovEnd = (int(spl[0])<<32) - 1
            elif len(spl) == 2:
                iovEnd = (int(spl[0])<<32) + (int(spl[1]) & 0xFFFFFFFF) - 1
            else:
                print 'Run specification invalid: %s' % options.rununtil

        self.iovList.append((iovStart, iovEnd))

    def loadLumiData(self, iov):
        if self.verbose: print 'Loading Luminosity data'

        # Instantiate new COOL data reader if not already done
        if self.lumiReader == None:
            self.lumiReader = CoolDataReader(self.sourceDB, self.oflLumiFolder)

        self.lumiReader.setIOVRange(iov[0], iov[1])
        self.lumiReader.setTag(self.sourceTag)
        self.lumiReader.setChannelId(self.sourceChannel)
        self.lumiReader.readData()


    def createOutputDB(self):
               print 'lumiCopy.createOutputDB() called'
               
               print 'lumiCopy.createOutputDB() - connecting to DB', self.outDBString
               try:
                   self.outDB = forceOpen(self.outDBString)
               except Exception, e:
                   print e
                   print "lumiCopy.createOutputDB() - Could not open DB %s" % self.outDBString
                   return False

               # Now create folder if needed
               self.coolSpec=cool.RecordSpecification()
               self.coolSpec.extend("LBAvInstLumi", cool.StorageType.Float)
               self.coolSpec.extend("LBAvEvtsPerBX",cool.StorageType.Float)
               self.coolSpec.extend("Valid", cool.StorageType.UInt32)
               try:
                   self.outFolder=ensureFolder(self.outDB, self.oflLumiFolder, self.coolSpec, athenaDesc(True,'CondAttrListCollection'),cool.FolderVersioning.MULTI_VERSION)
               except Exception,e:
                   print e
                   print "lumiCopy.createOutputDB() - Could not access or create folder %s" % self.oflLumiFolder
                   return False
               
               print "lumiCopy.createOutputDB() - Opened %s %s" % (self.outDBString, self.oflLumiFolder)
               self.outFolder.setupStorageBuffer()
               
               return True
           
# Executed from the command line
if __name__ == '__main__':
    lc = LumiCopy()
    lc.execute()

                
