#!/usr/bin/env python
#
# muHistMaker.py
#
# Simple demonstration of the bunch-by-bunch calibration method.
#
# Also used to create the mu histograms by lumiBlock used for MC reweighting
#
# Eric Torrence - 30 March 2011
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

from CoolLumiUtilities.LumiBlobConversion import unpackBCIDMask, unpackBCIDValues
from CoolLumiUtilities.LumiBlobConversion import unpackLiveFraction, unpackBunchGroup
from CoolLumiUtilities.LumiCalibrator import LumiCalibrator

from PyCool import cool
from ROOT import TFile, TH1D

class MuHistMaker:

    def __init__(self):

        # Control output level
        self.verbose = True

        # Luminosity Channel
        self.lumiChan = 103 # Lucid_HitOR
        # self.lumiChan = 201 # BCM Event OR
        # self.lumiChan = 102 # Lucid Event OR
        # self.lumiChan = 0 # Use preferred (doesn't work?)
        
        # Stable only
        self.stableOnly = True

        # Ready only
        self.readyOnly = True
        
        # Apply deadtime
        self.deadtime = False
        
        # List of (integer) run numbers specified on the command line
        self.runList = []

        # Dict of (lblo, lbhi) pairs giving extent of StableBeams
        self.stableDict = dict()

        # Dict of (lblo, lbhi) pairs giving extent of ATLAS Ready
        self.readyDict = dict()
        
        # Utlity routine for handling COOL connections
        self.dbHandler = LumiDBHandler()

        # Data readers
        self.maskReader = None
        self.lumiReader = None
        self.caliReader = None
        self.lblbReader = None
        self.lhcReader  = None
        self.trigReader = None
        self.bgReader = None
        self.rdyReader = None
        
        # Object which stores the luminosity calibration
        self.caliObj = LumiCalib()
        
        # Object which stores the BCID information
        self.maskObj = BCIDMask()

        # Object to store the bunch group definition
        self.bgObj = BunchGroup()
        
        # Histogram parameters
        self.nbins = 500
        self.mulo = 0.
        self.muhi = 50.
        
    # Explicitly clean up our DB connections
    def __del__(self):
        self.dbHandler.closeAllDB()
        
    # Called in command-line mode
    def execute(self):

        # Handle command-line switches
        self.parseOpts()

        # Process each run in self.runList
        for run in self.runList:

            # Load all data from COOL
            self.loadBCIDData(run)

            # Skip if run doesn't exist
            if len(self.lblbReader.data) == 0: continue
            
            # Find stable beams, output goes to self.stableTime
            self.findStable()

            # Find ATLAS ready
            self.findReady()
            
            # Skip if no stable beams
            if self.stableOnly and len(self.stableTime) == 0:
                continue

            # Skip of no ATLAS ready
            if self.readyOnly and len(self.readyTime) == 0:
                continue
            
            # Open new output file
            filename = 'run%d_mu.root' % run
            rootfile = TFile(filename, 'recreate')
            
            # Precompute deadtime into dictionary
            if self.deadtime:
                self.findDeadtime()

            if self.readyOnly:
                htitle = 'readyMuDist'
            elif self.stableOnly:
                htitle = 'stableMuDist'
            else:
                htitle = 'allMuDist'
                
            hall = TH1D(htitle, htitle, self.nbins, self.mulo, self.muhi)

            delivered = 0. 
            recorded = 0.
            average = 0.
            navg = 0
            
            # Loop over lumi blocks and calibrate
            for obj in self.lumiReader.data:

                startTime = obj.since()
                endTime = obj.until()
                runlb = obj.payload()['RunLB']
                run = runlb >> 32
                lb = runlb & 0xFFFFFFFF
                valid = obj.payload()['Valid'] & 0x3FF
                dtime = (endTime-startTime)/1E9

                if self.verbose:
                    print 'Run %d LB %d ' % (run, lb),
                    
                # Check whether this is in stable beams
                if self.stableOnly:
                    
                    isStable = False
                    for stable in self.stableTime:
                        if not stable[0] <= startTime < stable[1]: continue
                        isStable = True
                        break
                
                    if not isStable: continue

                    if self.verbose:
                        print 'stable ',

                # Check whether this is in ATLAS ready
                if self.readyOnly:

                    isReady = False
                    for ready in self.readyTime:
                        if not ready[0] <= runlb < ready[1]: continue
                        isReady = True
                        break

                    if not isReady:
                        if self.verbose: print
                        continue

                    if self.verbose:
                        print 'ready',

                if run == 189610 and lb < 310:
                    if self.verbose: print ' - skipped'
                    continue

                # Global timing shift
                if run == 201494 and lb > 220 and lb < 241:
                    if self.verbose: print ' - skipped'
                    continue
                
                if self.verbose:
                    print
                    
                # Make sure lumi data is valid
                #if valid != 0:
                #    print 'Invalid data %d found in Run %d LB %d!' % (valid, run, lb)
                #    continue

                # These all change slowly, so checking them every run/lb is OK
                
                # Make sure calibration is valid
                if not self.updateCalibration(startTime):
                    print 'Error finding calibration for Run %d LB %d!' % (run, lb)
                    continue

                # Make sure BCID mask is valid
                if not self.updateBCIDMask(startTime):
                    print "Couldn't find valid BCID mask data for Run %d LB %d!" % (run, lb)
                    continue

                # Make sure bunch group is valid (keyed by run/lb)
                if not self.updateBunchGroup(runlb):
                    print "Couldn't find valid Bunch Group definition for Run %d LB %d!" % (run, lb)
                    continue
                    
                # Now we want to start extracting bunch-by-bunch information

                # Get raw lumi
                normValue = obj.payload()['AverageRawInstLum']
                blobValue = obj.payload()['BunchRawInstLum']
                bcidVec, rawLumi = unpackBCIDValues(blobValue, self.maskObj.coll, normValue)

                bcidavg = 0.
                nbcid = 0
                h = TH1D(str(lb), '', self.nbins, self.mulo, self.muhi)

                for i in range(len(rawLumi)):

                    # Check if this is in our bunch group
                    bcid = bcidVec[i]

                    # Off by one, check if *following* BCID is in bunch group
                    # If so, this previous bcid is where the actual data is
                    if run >= 189639 and run <= 189660 and bcid > 1:
                        bcid += 1

                    if run == 191933 and bcid > 1:
                        bcid += 1
                        
                    if not bcid in self.bgObj.bg: continue
                    
                    ldel = self.caliObj.calibrate(rawLumi[i])
                    muval = ldel / self.caliObj.muToLumi
                    lumi = ldel * dtime
                    
                    delivered += lumi
                    
                    if self.deadtime:
                        
                        lrec = ldel * self.liveFrac[runlb][bcid]
                        lumi = lrec * dtime
                        recorded += lumi
                        bcidavg += self.liveFrac[runlb][bcid]
                        nbcid += 1
                        
                    # Note, mu only depends on delivered
                    # Weighting of recorded data depends on recorded
                    h.Fill(muval, lumi)
                    hall.Fill(muval, lumi)

                if nbcid > 0:
                    bcidavg /= nbcid
                    average += bcidavg
                    navg += 1

                h.Write()
                
            # Close output file
            hall.Write()
            rootfile.Close()

            print 'Lumi Delivered:', delivered
            print 'Lumi Recorded:', recorded
            if delivered > 0.:
                print 'True live fraction:', recorded/delivered
            else:
                print 'True live fraction: 0.0'

            if navg > 0:
                print 'BCID average frac: ', average/navg
            else:
                print 'BCID average frac: 0.0'
                
    # Load all data necessary to make bunch-by-bunch lumi for a single run
    def loadBCIDData(self, runnum):
        print 'MuHistMaker.loadBCIDData(%s) called' % runnum

        # Many folders are indexed by time stamp.  Load RunLB -> time conversion here
        self.loadLBLB(runnum)

        # Check if this run exists
        if len(self.lblbReader.data) == 0: return
        
        # Determine start and end time of this run
        startTime = self.lblbReader.data[0].payload()['StartTime']
        endTime =  self.lblbReader.data[-1].payload()['EndTime']
        iov = (startTime, endTime)
        
        # Read LHC Data (for stable beams)
        self.loadLHCData(iov)

        # Read ATLAS ready data
        self.loadReadyData(runnum)
        
        # Read the bunch group (so we sum over the right thing)
        self.loadBGData(runnum)
        
        # Read BCID Data
        self.loadBCIDMask(iov)
        self.loadBCIDCali(iov)
        self.loadBCIDLumi(iov)

        # Read turn counters (for livetime)
        if self.deadtime:
            self.loadTrigData(runnum)

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
            if len(self.lblbReader.data) > 1:
                print 'Last  LB %d/%d' % (self.lblbReader.data[-1].since() >> 32, self.lblbReader.data[-1].since() & 0xFFFFFFFF)
            
    def loadTrigData(self, run):
        if self.verbose: print 'Loading Trigger data'

        # Instantiate new COOL data reader if not already done
        if self.trigReader == None:
            self.trigReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/PerBcidDeadtime')

        self.trigReader.setIOVRangeFromRun(run)
        self.trigReader.readData()

        if self.verbose:
            print 'Read %d Trig records' % len(self.trigReader.data)

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
        self.caliReader.setChannel([self.lumiChan])
        self.caliReader.readData()

        if self.verbose:
            print 'Read %d Calibration records' % len(self.caliReader.data)
            
    def loadBCIDLumi(self, iov):
        if self.verbose: print 'Loading BCID luminosity values'

        # Instantiate new COOL data reader if not already done
        if self.lumiReader == None:
            # self.lumiReader = CoolDataReader('COOLONL_TDAQ/MONP200', '/TDAQ/OLC/BUNCHLUMIS')
            self.lumiReader = CoolDataReader('COOLONL_TDAQ/COMP200', '/TDAQ/OLC/BUNCHLUMIS')

        self.lumiReader.setIOVRange(iov[0], iov[1])
        self.lumiReader.setChannel([self.lumiChan])
        self.lumiReader.readData()

        if self.verbose:
            print 'Read %d Lumi records' % len(self.lumiReader.data)
            
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

    # Precompute the deadtime for this run into a dictionary indexed by lumi block and BCID
    def findDeadtime(self):
        if self.verbose: print 'Calculating per-BCID deadtime'

        # First dictionary index is lumiblock IOV
        self.liveFrac = dict()

        # Loop over each lumi block
        for obj in self.trigReader.data:

            key = obj.since()

            run = key >> 32
            lb = key & 0xFFFFFFFF
            bloblength = obj.payload()['HighPriority'].size()

            if self.verbose:
                print '%d %d Found trigger counter blob of length %d' % (run, lb, bloblength)

            # Unpack High Priority blob here
            liveVec = unpackLiveFraction(obj.payload())
            self.liveFrac[key] = liveVec
            
            # Each BCID is one 24-bit integer
            if self.verbose:
            
                for i in range(10):
                    print 'BICD: %d Live: %f' % (i+1, liveVec[i])
                        
    
                    
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
    
    def updateCalibration(self, startTime):
    
        if not self.caliObj.isValid(startTime):

            self.caliObj.clearValidity()
                    
            # Find the proper calibration object
            for cali in self.caliReader.data:
                if not cali.since() <= startTime < cali.until(): continue
                self.caliObj.setCalibration(cali)
                break
                
            if not self.caliObj.isValid(startTime):
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

        parser.add_option("-r", "--updateRun",
                          dest="runlist", metavar="RUN",
                          help="update specific run, or comma separated list")

        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.verbose = options.verbose

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
        
    def setValidity(self, obj):
        self.validStartTime = obj.since()
        self.validEndTime = obj.until()

    def isValid(self, time):
        return (self.validStartTime <= time < self.validEndTime)
    
        
        

# Executed from the command line
if __name__ == '__main__':
    mhm = MuHistMaker()
    mhm.execute()

                
