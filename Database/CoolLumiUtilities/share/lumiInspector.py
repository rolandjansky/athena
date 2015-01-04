#!/usr/bin/env python
#
# lumiInspector.py
#
# Print out per-BCID luminosity values and other information.
#
# Eric Torrence - 16 August 2012
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

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import forceOpen,ensureFolder,athenaDesc

from ROOT import TFile, TH1D, TTree, gROOT, AddressOf

class LumiInspector:

    def __init__(self):

        # Control output level
        self.verbose = True

        # Luminosity Channel
        self.lumiChanList = [0]

        # Specific channels to load in current run (for preferred)
        self.runChanList = [] 
        
        # List of (integer) run numbers specified on the command line
        self.runList = []

        # List of (integer, integer) COOL-format IOVs to print out
        self.iovList = []
        
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

            # Load offline luminosity data from COOL
            self.loadLumiData(run)
            
            # Load all other data from COOL
            self.loadBCIDData(run)

            # Skip if run doesn't exist
            if len(self.lblbReader.data) == 0: continue
            
            # Find stable beams, output goes to self.stableTime
            self.findStable()

            # Loop over all lumi blocks and calibrate each algorithm
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
                
                print
                print '---------------------------------------------------------------------------------------------'
                print 'Found Run/LB %d/%d (%s - %s) Length %.1f sec' % (run, lb, startStr, endStr, dtime),
                if isStable:
                    print ' -> stable',
                print

                # These change slowly, so checking them every run/lb is OK
                # Make sure bunch group is valid (keyed by run/lb)
                if not self.updateBunchGroup(runlb):
                    print "Couldn't find valid Bunch Group definition for Run %d LB %d!" % (run, lb)
                    continue

                # Get integer to make average mu value below
                nBunch = len(self.bgObj.bg)
                if nBunch < 1: nBunch = 1

                # Make sure BCID mask is valid
                if not self.updateBCIDMask(startTime):
                    print "Couldn't find valid BCID mask data for Run %d LB %d!" % (run, lb)
                    continue

                print 'Bunch Group Length: %d Colliding BCIDs: %d' % (nBunch, len(self.maskObj.coll))

                if nBunch != len(self.maskObj.coll) and isStable:
                    print 'Bunch group:'
                    for bcid in self.bgObj.bg:
                        print ' %d' % bcid,
                    print
                    print 'Colliding bunches:'
                    for bcid in self.maskObj.coll:
                        print ' %d' % bcid,
                    print    

                # Loop through defined channels and print luminosity
                if runlb not in self.lumiDict:
                    print 'No offline lumi data found!'
                    continue
                
                lbSum = dict()
                for chan in self.lumiChanList:

                    if chan not in self.lumiDict[runlb]:
                        print 'Channel %d not found in offline lumi!' % chan
                        continue

                    lumiobj = self.lumiDict[runlb][chan]
                    
                    oflLumi = lumiobj.payload()['LBAvInstLumi']
                    oflMu   = lumiobj.payload()['LBAvEvtsPerBX']
                    oflValid = lumiobj.payload()['Valid']

                    if chan == 0:
                        rawChan = oflValid >> 22
                        oflValid = oflValid & 0x3FF
                        print 'Analyzing channel %d -> %d, Lumi = %.1f, AvgMu = %.1f, Valid = %d' % (chan, rawChan, oflLumi, oflMu, oflValid)

                    else:
                        rawChan = chan
                        print 'Analyzing channel %d, Lumi = %.1f, Mu = %.1f, Valid = %d' % (chan, oflLumi, oflMu, oflValid)
                        
                    # Now find raw data
                    if rawChan not in self.rawDict[runlb]:
                        print "Can't find channel", rawChan, "in run/lb", run, '/', lb  
                        continue
                    
                    obj = self.rawDict[runlb][rawChan]

                    if rawChan != obj.channelId():
                        print 'Channel', rawChan, '!=', obj.channelId(), '!'
                        continue

                    if runlb != obj.payload()['RunLB']:
                        print 'RunLB', runlb, '!=', obj.payload()['RunLB'], '!'
                        continue

                    #valid = obj.payload()['Valid'] & 0x03

                    # Clear lumi block sum counters
                    lbSum[rawChan] = 0.
                    
                    # Make sure calibration is valid
                    if not self.updateCalibration(startTime, rawChan):
                        print 'Error finding calibration for Run %d LB %d Chan %d!' % (run, lb, rawChan)
                        continue

                    # Also must update PMT currents in calibration object
                    if runlb in self.pmtDict:
                        self.caliObj[rawChan].setLucidCurrent(self.pmtDict[runlb].payload())

                    # Now we want to start extracting bunch-by-bunch information

                    # Get raw lumi
                    normValue = obj.payload()['AverageRawInstLum']
                    blobValue = obj.payload()['BunchRawInstLum']
                    rawValid  = obj.payload()['Valid']
                    
                    print 'Calling unpackBCIDValues'
                    bcidVec, rawLumi = unpackBCIDValues(blobValue, self.maskObj.coll, normValue)

                    print
                    print 'Run %d LB %4d Chan %4d Valid %d AverageRawInstLum %f' % (run, lb, rawChan, rawValid, normValue)
                    print '-------------------------------------------------------------------'
                    # dict to hold BCID lumi keyed by BCID
                    bcidLumi = dict() 
                    for i in range(len(rawLumi)):

                        # Check if this is in our bunch group (only really need to fill this once)
                        bcid = bcidVec[i]

                        # Protect against any weird values
                        if bcid >= self.nBCID:
                            print 'BCID %d found >= %d!' % (bcid, self.nBCID)
                            continue

                        # Now need to save bcid, mu, and calibLumi 
                        lraw = rawLumi[i]
                        lcal = self.caliObj[rawChan].calibrate(lraw)

                        bcidLumi[bcid] = lcal

                        # Subtract afterglow (simple subtraction) from physics BG
                        #if self.afterglow and bcid in self.bgObj.bg:
                        #    lcal = bcidLumi[bcid] - bcidLumi.get(bcid-1, 0.)

                        muval = lcal / self.caliObj[rawChan].muToLumi

                        # Keep track of total lumi per channel per lumi block
                        # Only if in physics bunch group!
                        if bcid in self.bgObj.bg:
                            lbSum[rawChan] += lcal
                            print '  BCID %4d, Raw = %f, Cal = %f, Mu = %f' % (bcid, lraw, lcal, muval)
                            
                    # End loop over BCIDs
                    print ' Total raw lumi in Physics BG: %f (offline: %f, ratio: %f)' % (lbSum[rawChan], oflLumi, oflLumi/max(lbSum[rawChan],1))
                    
                # End of loop over channels
                
            # End of loop over LBs

        # End of loop over runs                

    # Load luminosity data
    def loadLumiData(self, runnum):

        print 'lumiInspector.loadLumiData(%s) called' % runnum

        self.loadLumi(runnum)
        
    # Load all data necessary to make bunch-by-bunch lumi for a single run
    def loadBCIDData(self, runnum):

        print 'MuHistMaker.loadBCIDData(%s) called' % runnum

        # Many folders are indexed by time stamp.  Load RunLB -> time conversion here
        self.loadLBLB(runnum)

        if len(self.lblbReader.data) == 0: return

        # Determine start and end time of this run
        startTime = self.lblbReader.data[0].payload()['StartTime']
        endTime =  self.lblbReader.data[-1].payload()['EndTime']
        iov = (startTime, endTime)
        
        # Read LHC Data (for stable beams)
        self.loadLHCData(iov)

        # Read the bunch group (so we sum over the right thing)
        self.loadBGData(runnum)
        
        # Read BCID Data
        self.loadBCIDMask(iov)
        self.loadBCIDCali(iov)
        self.loadBCIDLumi(iov)

        self.loadBCIDCurrents(iov)

        # Read LUCID PMT Currents
        self.loadPmtData(iov)

    def loadLumi(self, run):
        if self.verbose: print 'Loading Lumi data'

        if self.oflReader == None:
            if self.online:
                self.oflReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBLESTONL')
            else:
                self.oflReader = CoolDataReader('COOLOFL_TRIGGER/COMP200', '/TRIGGER/OFLLUMI/LBLESTOFL')
                self.oflReader.setTag(self.lumiTag)
                
        self.oflReader.setIOVRangeFromRun(run)
        self.oflReader.setChannel(self.lumiChanList)
        self.oflReader.readData()

        if self.verbose:
            print 'Read %d Lumi records' % len(self.oflReader.data)

        # Fill lumiDict and the runChanList giving channels needed for pref algorithm
        self.lumiDict = dict()
        self.runChanList = []
        for chan in self.lumiChanList:
            self.runChanList.append(chan)
        
        for obj in self.oflReader.data:
            channel = obj.channelId()
            runlb = obj.since()
            if runlb not in self.lumiDict:
                self.lumiDict[runlb] = dict()
            self.lumiDict[runlb][channel] = obj

            if channel == 0:
                prefChan = (obj.payload()['Valid'] >> 22)

                if prefChan not in self.runChanList:
                    self.runChanList.append(prefChan)

                    
        self.runChanList.sort()
        if self.verbose:
            print 'Loading channels:', self.runChanList
        
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

        
    def loadBGData(self, run):
        if self.verbose: print 'Loading Bunch group data'

        # Instantiate new COOL reader if not already done
        if self.bgReader == None:
            self.bgReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LVL1/BunchGroupContent')

        self.bgReader.setIOVRangeFromRun(run)
        self.bgReader.readData()

        if self.verbose:
            print 'Read %d bunch group records' % len(self.bgReader.data)
            
    def loadBCIDMask(self, iov):
        if self.verbose: print 'Loading BCID masks'

        # Instantiate new COOL data reader if not already done
        if self.maskReader == None:
            self.maskReader = CoolDataReader('COOLONL_TDAQ/COMP200', '/TDAQ/OLC/LHC/FILLPARAMS')
            
        self.maskReader.setIOVRange(iov[0], iov[1])
        self.maskReader.readData()

        if self.verbose:
            print 'Read %d BCID Mask records' % len(self.maskReader.data)

    def loadBCIDCali(self, iov):
        if self.verbose: print 'Loading BCID luminosity calibrations'

        # Instantiate new COOL data reader if not already done
        if self.caliReader == None:
            self.caliReader = CoolDataReader('COOLONL_TDAQ/COMP200', '/TDAQ/OLC/CALIBRATIONS')
            
        self.caliReader.setIOVRange(iov[0], iov[1])
        self.caliReader.setChannel(self.runChanList)
        self.caliReader.readData()

        if self.verbose:
            print 'Read %d Calibration records' % len(self.caliReader.data)
            
    def loadBCIDLumi(self, iov):
        if self.verbose: print 'Loading BCID luminosity values'

        # Instantiate new COOL data reader if not already done
        if self.lumiReader == None:
            # Switch at start of September
            if iov[0] < 1315395299650946035:
                self.lumiReader = CoolDataReader('COOLONL_TDAQ/MONP200', '/TDAQ/OLC/BUNCHLUMIS')
            else:
                self.lumiReader = CoolDataReader('COOLONL_TDAQ/COMP200', '/TDAQ/OLC/BUNCHLUMIS')

        self.lumiReader.setIOVRange(iov[0], iov[1])
        self.lumiReader.setChannel(self.runChanList)
        self.lumiReader.readData()

        if self.verbose:
            print 'Read %d Raw Lumi records' % len(self.lumiReader.data)
            
        self.rawDict = dict()
        for obj in self.lumiReader.data:
            channel = obj.channelId()
            runlb = obj.payload()['RunLB']
            if runlb not in self.rawDict:
                self.rawDict[runlb] = dict()
            self.rawDict[runlb][channel] = obj
            
    # Bunch currents
    def loadBCIDCurrents(self, iov):
        if self.verbose: print 'Loading Bunch Current information'

        if self.currReader == None:
            self.currReader = CoolDataReader('COOLONL_TDAQ/MONP200', '/TDAQ/OLC/LHC/BUNCHDATA')

        self.currReader.setIOVRange(iov[0], iov[1])
        self.currReader.setChannelId(1) # 0 = BPTX, 1 = Fast BCT
        self.currReader.readData()

        # Current objects keyed by [runlb]
        self.currDict = dict()
        for obj in self.currReader.data:
            runlb = obj.payload()['RunLB']
            self.currDict[runlb] = obj

        if self.verbose:
            print 'Read %d Current records' % len(self.currReader.data)
        
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
        
    # LUCID PMT currents
    def loadPmtData(self, iov):
        if self.verbose: print 'Loading LUCID PMT Current information'

        if self.pmtReader == None:
            self.pmtReader = CoolDataReader('COOLONL_TDAQ/COMP200', '/TDAQ/OLC/LUCIDCURRENTS')

        self.pmtReader.setIOVRange(iov[0], iov[1])
        self.pmtReader.setChannel([0, 1]) # Load total PMT and fiber currents
        self.pmtReader.readData()

        if self.verbose:
            print 'Read %d PMT current records' % len(self.pmtReader.data)

        # PMT currents keyed by [runlb]
        self.pmtDict = dict()
        self.fibDict = dict() # Fiber currents
        for obj in self.pmtReader.data:
            runlb = obj.payload()['RunLB']
            if obj.channelId() == 0:
                self.pmtDict[runlb] = obj
            elif obj.channelId() == 1:
                self.fibDict[runlb] = obj
            else:
                print 'Found channel %d in pmtReader!' % obj.channel()
                
        
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

                    
    def updateBCIDMask(self, startTime):

        if not self.maskObj.isValid(startTime):

            self.maskObj.clearValidity()

            # Find the proper mask object
            maskData = None
            for mask in self.maskReader.data:
                if not mask.since() <= startTime < mask.until(): continue
                self.maskObj.setMask(mask)
                break

            if not self.maskObj.isValid(startTime):
                return False

        return True
    
    def updateCalibration(self, startTime, chan):

        if not chan in self.caliObj:
            self.caliObj[chan] = LumiCalib()
            
        if not self.caliObj[chan].isValid(startTime):

            self.caliObj[chan].clearValidity()
                    
            # Find the proper calibration object
            for cali in self.caliReader.data:
                if cali.channelId() != chan: continue
                if not cali.since() <= startTime < cali.until(): continue
                
                print 'New Calibration for channel %d' % chan
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

        parser.add_option("--online",
                          action="store_true", default=self.online, dest="online",
                          help="Use online luminosity")

        parser.add_option("--tag",
                          dest="lumitag", default=self.lumiTag, metavar="TAG",
                          help="use offline lumi tag (default: %s)" % self.lumiTag)
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.verbose = options.verbose
        self.online = options.online
        self.lumiTag = options.lumitag
        
        # Parse run list
        if options.runlist != None:
            self.parseRunList(options.runlist)

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

                
