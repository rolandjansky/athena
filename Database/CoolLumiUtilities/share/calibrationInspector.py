#!/usr/bin/env python
#
# calibrationInspector.py
#
# Print out all calibration values found for a given run
#
# Eric Torrence - 5 September 2012
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

from CoolLumiUtilities.LumiCalibrator import LumiCalibrator
from CoolLumiUtilities.LumiBlobConversion import unpackBCIDMask, unpackBCIDValues
from CoolLumiUtilities.LumiBlobConversion import unpackLiveFraction, unpackBunchGroup
from CoolLumiUtilities.LumiChannelDefs import LumiChannelDefs

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import forceOpen,ensureFolder,athenaDesc

from ROOT import TFile, TH1D, TTree, gROOT, AddressOf

class LumiInspector:

    def __init__(self):

        # Control output level
        self.verbose = True

        # List of (integer) run numbers specified on the command line
        self.runList = []

        # List of (integer, integer) COOL-format IOVs to print out
        self.iovList = []

        # List of channel numbers found in calibration data
        self.lumiChanList = []
        
        # Dict of (lblo, lbhi) pairs giving extent of StableBeams
        self.stableDict = dict()

        # Utlity routine for handling COOL connections
        self.dbHandler = LumiDBHandler()

        # Data readers
        self.maskReader = None
        self.oflReader = None
        self.lumiReader = None
        self.caliReader = None
        self.lblbReader = None
        self.currReader = None
        self.lhcReader  = None
        self.bgReader = None
        self.pmtReader = None
        
        # Object which stores the luminosity calibration
        self.caliObj = dict() # LumiCalib()
        
        # Object which stores the BCID information
        self.maskObj = BCIDMask()

        # Object to store the bunch group definition
        self.bgObj = BunchGroup()

        # Offline tag
        self.lumiTag = 'OflLumi-8TeV-002'
        
        # Use online DB
        self.online = False

        self.nBCID = 3564

    # Explicitly clean up our DB connections
    def __del__(self):
        self.dbHandler.closeAllDB()
        
    # Called in command-line mode
    def execute(self):

        # Handle command-line switches
        self.parseOpts()

        # Process each run in self.runList
        for run in self.runList:

            # Load data from COOL
            self.loadData(run)
            
            # Skip if run doesn't exist
            if len(self.lblbReader.data) == 0: continue
            
            # Find stable beams, output goes to self.stableTime
            self.findStable()

            # Loop over all calibration records
            for obj in self.lblbReader.data:

                runlb = obj.since()
                
                # Check if this is in our IOV list
                found = False
                for iov in self.iovList:
                    if not (iov[0] <= runlb < iov[1]): continue
                    found = True
                    break

                if not found: continue
                
                run = runlb >> 32
                lb = runlb & 0xFFFFFFFF
                startTime = obj.payload()['StartTime']
                endTime = obj.payload()['EndTime']
                dtime = (endTime-startTime)/1E9

                # Check whether this is in stable beams
                isStable = False
                for stable in self.stableTime:
                    if not (stable[0] <= startTime < stable[1]): continue
                    isStable = True
                    break

                startStr = time.strftime('%y/%m/%d, %H:%M:%S', time.gmtime(startTime/1E9))
                endStr   = time.strftime('%y/%m/%d, %H:%M:%S', time.gmtime(endTime/1E9))
                
                print 'Found Run/LB %d/%d (%s - %s) Length %.1f sec' % (run, lb, startStr, endStr, dtime),
                if isStable:
                    print ' -> stable',
                print

                # These change slowly, so checking them every run/lb is OK
                # Make sure bunch group is valid (keyed by run/lb)
                for chan in self.lumiChanList:

                    # Make sure calibration is valid
                    if not self.updateCalibration(startTime, chan):
                        print 'Error finding calibration for Run %d LB %d Chan %d!' % (run, lb, rawChan)
                        continue

                # End of loop over channels
                
            # End of loop over LBs

        # End of loop over runs                

    # Load all data necessary to make bunch-by-bunch lumi for a single run
    def loadData(self, runnum):

        print 'calibrationInspector.loadData(%s) called' % runnum

        # Many folders are indexed by time stamp.  Load RunLB -> time conversion here
        self.loadLBLB(runnum)

        if len(self.lblbReader.data) == 0: return

        # Determine start and end time of this run
        startTime = self.lblbReader.data[0].payload()['StartTime']
        endTime =  self.lblbReader.data[-1].payload()['EndTime']
        iov = (startTime, endTime)
        
        # Read LHC Data (for stable beams)
        self.loadLHCData(iov)

        # Read calibration data
        self.loadBCIDCali(iov)

        if len(self.lumiChanList) == 0:
            for obj in self.caliReader.data:
                if obj.channelId() not in self.lumiChanList:
                    self.lumiChanList.append(obj.channelId())
        
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

        
    def loadBCIDCali(self, iov):
        if self.verbose: print 'Loading BCID luminosity calibrations'

        # Instantiate new COOL data reader if not already done
        if self.caliReader == None:
            self.caliReader = CoolDataReader('COOLONL_TDAQ/COMP200', '/TDAQ/OLC/CALIBRATIONS')
            
        self.caliReader.setIOVRange(iov[0], iov[1])
        self.caliReader.readData()

        if self.verbose:
            print 'Read %d Calibration records' % len(self.caliReader.data)
            
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

                    
    def updateCalibration(self, startTime, chan):

        if not chan in self.caliObj:
            self.caliObj[chan] = LumiCalib()
            self.caliObj[chan].verbose = True
            
        if not self.caliObj[chan].isValid(startTime):

            self.caliObj[chan].clearValidity()
                    
            # Find the proper calibration object
            for cali in self.caliReader.data:
                if cali.channelId() != chan: continue
                if not cali.since() <= startTime < cali.until(): continue
                
                print 'New Calibration for channel %d (%s)' % (chan, LumiChannelDefs().name(chan))
                self.caliObj[chan].setCalibration(cali)

            if not self.caliObj[chan].isValid(startTime):
                return False

        return True

    def updateBunchGroup(self, runlb):

        if not self.bgObj.isValid(runlb):

            self.bgObj.clearValidity()

            # Find the proper BG object
            for bg in self.bgReader.data:
                if not bg.since() <= runlb < bg.until(): continue
                self.bgObj.setBG(bg)
                break

            if not self.bgObj.isValid(runlb):
                return False

        return True
    
    def parseOpts(self):

        parser = OptionParser(usage="usage: %prog [options]", add_help_option=False)

        parser.add_option("-?", "--usage", action="store_true", default=False, dest="help",
                          help="show this help message and exit")
        
        parser.add_option("-v", "--verbose",
                     action="store_true", default=self.verbose, dest="verbose",
                     help="turn on verbose output")

        parser.add_option("-r", "--run",
                          dest="runlist", metavar="RUN",
                          help="process specific run, run/lb, range, or comma separated list")

        parser.add_option("--channel",
                          dest="chanlist", metavar="CHAN",
                          help="select specific channel")
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.verbose = options.verbose
        
        # Parse run list
        if options.runlist != None:
            self.parseRunList(options.runlist)

        if options.chanlist != None:
            self.lumiChanList = [int(options.chanlist)]
            
    # Parse text-based run list
    # Can specify runs alone or run/lb lumi blocks
    # Ranges are specified with a dash, and commas separate instances
    def parseRunList(self,runstr):    

        # Clear the old one
        self.runList = [] # Each run gets one entry here
        self.iovList = [] # Pair of IOVs in COOL format
        
        # Can be comma-separated list of run ranges
        runlist = runstr.split(',')
        if len(runlist) == 0:
            print 'Invalid run list specified!'
            sys.exit()

        # Further parse each comma-separated item
        for str in runlist:

            # Check for ranges
            subrunlist = str.split('-')
                
            if len(subrunlist) == 1: # Single item

                # Check for lumi block or not
                lblist = str.split('/')
                if len(lblist) == 1: # Single run
                    runnum = int(subrunlist[0])
                    self.iovList.append(((runnum << 32), ((runnum+1) << 32)))
                        
                elif len(lblist) == 2: # Run/LB
                    runnum = int(lblist[0])
                    lbnum = int(lblist[1])
                    self.iovList.append(((runnum << 32) + lbnum, (runnum << 32) + lbnum + 1))

                else: # Too many parameters
                    print 'Invalid run list item found:', str
                    sys.exit()

                if runnum not in self.runList:
                    self.runList.append(runnum)
    
            elif len(subrunlist) == 2: # Range

                # Parse starting item
                lblist = subrunlist[0].split('/')
                if len(lblist) == 1: # Single run
                    startrun = int(lblist[0])
                    startiov = startrun << 32

                elif len(lblist) == 2: # Run/LB
                    startrun = int(lblist[0])
                    lb = int(lblist[1])
                    startiov = (startrun << 32) + lb

                else: # Too many parameters
                    print 'Invalid run list item found:', str
                    sys.exit()

                # Parse ending item
                lblist = subrunlist[1].split('/')
                if len(lblist) == 1: # Single run
                    endrun = int(lblist[0])
                    endiov = (endrun+1) << 32

                elif len(lblist) == 2: # Run/LB
                    endrun = int(lblist[0])
                    lb = int(lblist[1])
                    endiov = (endrun << 32) + lb + 1

                else: # Too many parameters
                    print 'Invalid run list item found:', str
                    sys.exit()

                self.iovList.append((startiov, endiov))
                
                for runnum in range(startrun, endrun+1):
                    if runnum not in self.runList:
                        self.runList.append(runnum)

            else: # Too many parameters
                print 'Invalid run list item found:', str
                sys.exit()

        self.runList.sort()
        if self.verbose:
            print 'Finished parsing run list:',
            for runnum in self.runList:
                print runnum,
            print

        self.iovList.sort()
        if self.verbose:
            for iov in self.iovList:
                runlo = iov[0] >> 32
                lblo = iov[0] & 0xFFFFFFFF
                print '%d/%d - %d/%d' % (iov[0] >> 32, iov[0] & 0xFFFFFFFF, iov[1] >> 32, iov[1] & 0xFFFFFFFF)
                    
# Utility object to keep track of valid calibration object
class LumiCalib(LumiCalibrator):
    def __init__(self):

        self.clearValidity()

        # Make sure we initialize the base class as well
        LumiCalibrator.__init__(self)

    def clearValidity(self):
        self.validStartTime = cool.ValidityKeyMax
        self.validEndTime = cool.ValidityKeyMin

    def setCalibration(self, calibObj):
        self.setValidity(calibObj)
        LumiCalibrator.setCalibration(self, calibObj.payload())

        print "Type: %s" % self.fType
        print "muToLumi: %f" % self.muToLumi
        for i in range (len(self.parVec)):
            print "parVec[%d] = %f" % (i, self.parVec[i])

    def setCurrentScale(self, ver, a, b, c=0.):

        self.currVersion = ver
        self.currParA = a
        self.currParB = b
        self.currParC = c

        print 'Type: %s' % self.fType
        print 'Changing current calibration to %d %f %f %f' % (ver, a, b, c)
        
    def setOR(self, sigvis):
        print 'Changing sigma_inel/sigma_vis from %f to %f' % (self.parVec[0], 73000./sigvis)
        self.parVec[0] = 73000./sigvis
        
    def setAND(self, sigvisOR, sigvisAND):
        print 'Changing sigma_vis(OR)  from %f to %f' % (self.parVec[0], sigvisOR)
        print 'Changing sigma_vis(AND) from %f to %f' % (self.parVec[1], sigvisAND)
        self.parVec[0] = sigvisOR
        self.parVec[1] = sigvisAND
        if self.fType == 'LookupTable_EventAND_Lin':
            self.parVec[5] = 73000./sigvisAND
        else:
            self.parVec[7] = 73000./sigvisAND 
        
    def rescaleOR(self, scale):
        print 'Rescaling sigma_vis OR by %f' % scale
        self.parVec[0] /= scale

    def rescaleAND(self, orscale, andscale):
        print 'Rescaling sigma_vis OR by %f' % orscale
        print 'Rescaling sigma_vis AND by %f' % andscale

        self.parVec[0] *= orscale
        self.parVec[1] *= andscale
        self.parVec[5] /= andscale
        
    def setValidity(self, obj):
        self.validStartTime = obj.since()
        self.validEndTime = obj.until()

    def isValid(self, time):
        return (self.validStartTime <= time < self.validEndTime)
    

# Utility object to keep track of valid BCID mask
class BCIDMask():
    
    def __init__(self):

        self.clearValidity()
        
        self.nb1 = 0
        self.nb2 = 0
        self.ncol = 0

        self.beam1 = []
        self.beam2 = []
        self.coll = []

    def clearValidity(self):
        self.validStartTime = cool.ValidityKeyMax
        self.validEndTime = cool.ValidityKeyMin

    def setMask(self, maskObj):
        self.setValidity(maskObj)

        self.nb1 = maskObj.payload()['Beam1Bunches']
        self.nb2 = maskObj.payload()['Beam2Bunches']
        self.ncol = maskObj.payload()['LuminousBunches']
        bmask = maskObj.payload()['BCIDmasks']
        
        self.beam1, self.beam2, self.coll = unpackBCIDMask(bmask, self.nb1, self.nb2, self.ncol)
        
    def setValidity(self, obj):
        self.validStartTime = obj.since()
        self.validEndTime = obj.until()

    def isValid(self, time):
        return (self.validStartTime <= time < self.validEndTime)
    
# Utility object to keep track of valid Bunch Group definition
# Bunch group is kept as a list of integers (0-3563)
class BunchGroup():
    
    def __init__(self):

        self.clearValidity()
        self.bg = []

    def clearValidity(self):
        self.validStartTime = cool.ValidityKeyMax
        self.validEndTime = cool.ValidityKeyMin

    def setBG(self, bgObj):
        self.setValidity(bgObj)

        blob = bgObj.payload()['BunchCode']
        self.bg = unpackBunchGroup(blob)
        self.bg.sort()
        
    def setValidity(self, obj):
        self.validStartTime = obj.since()
        self.validEndTime = obj.until()

    def isValid(self, time):
        return (self.validStartTime <= time < self.validEndTime)
    
        
                
# Executed from the command line
if __name__ == '__main__':
    li = LumiInspector()
    li.execute()

                
