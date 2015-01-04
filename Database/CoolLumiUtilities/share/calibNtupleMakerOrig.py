#!/usr/bin/env python
#
# calibNtupleMaker.py
#
# Produce an ntuple of per-bcid luminosities using the raw lumi data
#
# Eric Torrence - 27 April 2011
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

from CoolLumiUtilities.LumiBlobConversion import LumiCalibrator, unpackBCIDMask, unpackBCIDValues
from CoolLumiUtilities.LumiBlobConversion import unpackLiveFraction, unpackBunchGroup

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import forceOpen,ensureFolder,athenaDesc

from ROOT import TFile, TH1D, TTree, gROOT, AddressOf

class CalibNtupleMaker:

    def __init__(self):

        # Control output level
        self.verbose = True

        # Luminosity Channel
        self.lumiChanList = [101,102,105,106, 201,202, 211, 212] # Lucid Event OR

        # Stable only
        self.stableOnly = True

        # Scale calibration
        self.correct = True

        # Write output ntuple
        self.ntuple = True

        # Write local DB instance
        self.writedb = False
        
        # List of (integer) run numbers specified on the command line
        self.runList = []

        # Dict of (lblo, lbhi) pairs giving extent of StableBeams
        self.stableDict = dict()

        # Utlity routine for handling COOL connections
        self.dbHandler = LumiDBHandler()

        # Output DB parameters
        self.outDBString = "sqlite://;schema=./mycool.db;dbname=COMP200"
        self.oflLumiFolder = '/TRIGGER/OFLLUMI/LBLESTOFL'
        self.oflLumiTag = 'OflLumi-7TeV-001'
        self.outDB = None
        self.coolSpec = None
        self.outFolder = None
        
        # Data readers
        self.maskReader = None
        self.lumiReader = None
        self.caliReader = None
        self.lblbReader = None
        self.lhcReader  = None
        self.bgReader = None
        
        # Object which stores the luminosity calibration
        self.caliObj = dict() # LumiCalib()
        
        # Object which stores the BCID information
        self.maskObj = BCIDMask()

        # Object to store the bunch group definition
        self.bgObj = BunchGroup()

        self.currentRun = 0
        
    # Explicitly clean up our DB connections
    def __del__(self):
        self.dbHandler.closeAllDB()
        
    # Called in command-line mode
    def execute(self):

        # Handle command-line switches
        self.parseOpts()

        # Prepare output DB
        if self.writedb:

            if not self.createOutputDB():
                print 'Error creating output DB!'
                sys.exit()
            
        # Process each run in self.runList
        for run in self.runList:

            # Load all data from COOL
            self.loadBCIDData(run)

            # Skip if run doesn't exist
            if len(self.lblbReader.data) == 0: continue
            
            # Find stable beams, output goes to self.stableTime
            self.findStable()

            if self.stableOnly and len(self.stableTime) == 0: continue
            
            # Open new output file and init ntuple
            if self.ntuple:
                nt = NtupleHandler()
                nt.fileName = '/tmp/torrence/r%d.root' % run
                nt.open()
                nt.init()

            # Storage objects keyed by [runlb][algo]
            objDict = dict()
            runlbList = []
            
            # Loop over all objects and sort by runlb and algo
            for obj in self.lumiReader.data:
                channel = obj.channelId()
                runlb = obj.payload()['RunLB']
                if runlb not in objDict:
                    objDict[runlb] = dict()
                    runlbList.append(runlb)
                objDict[runlb][channel] = obj
                
                # if self.verbose:
                #     run = runlb >> 32
                #     lb = runlb & 0xFFFFFFFF
                #     print 'Found Run %d LB %d chan %d' % (run, lb, channel)
                    
            # Loop over all lumi blocks and calibrate each algorithm
            runlbList.sort()
            for runlb in runlbList:

                run = runlb >> 32
                lb = runlb & 0xFFFFFFFF

                self.currentRun = run
                
                # Make sure bunch group is valid (keyed by run/lb)
                if not self.updateBunchGroup(runlb):
                    print "Couldn't find valid Bunch Group definition for Run %d LB %d!" % (run, lb)
                    continue

                # Zero everything here
                if self.ntuple:
                    nt.bcidStruct.fNBunch = len(self.bgObj.bg)

                # Get integer to make average mu value below
                nBunch = len(self.bgObj.bg)
                if nBunch < 1: nBunch = 1
                
                bcidDict = dict()
                for i in range(len(self.bgObj.bg)):
                    bcid = self.bgObj.bg[i]
                    bcidDict[bcid] = i

                    if not self.ntuple: continue
                    
                    nt.bcidStruct.fBCIDs[i] = bcid
                    # print 'Entry %d BCID %d' % (i, bcid)
                    
                    nt.bcidStruct.fLumiLucAND[i] = 0.
                    nt.bcidStruct.fRawLucAND[i] = 0.
                    nt.bcidStruct.fMuLucAND[i] = 0.
                    nt.bcidStruct.fLumiLucOR[i] = 0.
                    nt.bcidStruct.fRawLucOR[i] = 0.
                    nt.bcidStruct.fMuLucOR[i] = 0.
                    nt.bcidStruct.fLumiLucA[i] = 0.
                    nt.bcidStruct.fRawLucA[i] = 0.
                    nt.bcidStruct.fMuLucA[i] = 0.
                    nt.bcidStruct.fLumiLucC[i] = 0.
                    nt.bcidStruct.fRawLucC[i] = 0.
                    nt.bcidStruct.fMuLucC[i] = 0.
                    
                    nt.bcidStruct.fLumiBcmHAND[i] = 0.
                    nt.bcidStruct.fRawBcmHAND[i] = 0.
                    nt.bcidStruct.fMuBcmHAND[i] = 0.
                    nt.bcidStruct.fLumiBcmHOR[i] = 0.
                    nt.bcidStruct.fRawBcmHOR[i] = 0.
                    nt.bcidStruct.fMuBcmHOR[i] = 0.
                    nt.bcidStruct.fLumiBcmVAND[i] = 0.
                    nt.bcidStruct.fRawBcmVAND[i] = 0.
                    nt.bcidStruct.fMuBcmVAND[i] = 0.
                    nt.bcidStruct.fLumiBcmVOR[i] = 0.
                    nt.bcidStruct.fRawBcmVOR[i] = 0.
                    nt.bcidStruct.fMuBcmVOR[i] = 0.
                        

                first = True
                lbSum = dict()
                for chan in self.lumiChanList:

                    if chan not in objDict[runlb]:
                        print "Can't find channel", chan, "in run/lb", run, '/', lb  
                        continue
                    
                    obj = objDict[runlb][chan]

                    if chan != obj.channelId():
                        print 'Channel', chan, '!=', obj.channelId(), '!'
                        continue

                    if runlb != obj.payload()['RunLB']:
                        print 'RunLB', runlb, '!=', obj.payload()['RunLB'], '!'
                        continue

                    startTime = obj.since()
                    endTime = obj.until()
                    dtime = (endTime-startTime)/1E9
                    valid = obj.payload()['Valid'] & 0x03

                    # Hack for lucid validity
                    if 100<chan<200 and valid==1: valid=0
                    
                    # Check whether this is in stable beams
                    isStable = False
                    for stable in self.stableTime:
                        if not stable[0] <= startTime < stable[1]: continue
                        isStable = True
                        break
                
                    if self.stableOnly and not isStable: continue

                    # Clear lumi block sum counters
                    lbSum[chan] = 0.
                    
                    # Only do this once per lumi block
                    if first:
                        first = False
                        
                        if self.verbose:
                            print 'Found stable Run %d LB %d' % (run, lb)

                        # Fill general LB information
                        if self.ntuple:
                            nt.fillLBData(obj)
                            nt.lbDataStruct.fStable = isStable

                        # These change slowly, so checking them every run/lb is OK
                        # Make sure BCID mask is valid
                        if not self.updateBCIDMask(startTime):
                            print "Couldn't find valid BCID mask data for Run %d LB %d!" % (run, lb)
                            continue

                    
                    # Make sure calibration is valid
                    if not self.updateCalibration(startTime, chan):
                        print 'Error finding calibration for Run %d LB %d Chan %d!' % (run, lb, chan)
                        continue

                    # Save this so I don't need to save all calibrated data
                    if self.ntuple:
                        nt.bcidStruct.fMuToLumi = self.caliObj[chan].muToLumi
                    
                    # Now we want to start extracting bunch-by-bunch information

                    # Get raw lumi
                    normValue = obj.payload()['AverageRawInstLum']
                    blobValue = obj.payload()['BunchRawInstLum']
                    bcidVec, rawLumi = unpackBCIDValues(blobValue, self.maskObj.coll, normValue)

                    for i in range(len(rawLumi)):

                        # Check if this is in our bunch group
                        bcid = bcidVec[i]
                        if not bcid in self.bgObj.bg: continue

                        # Figure out ordinal value of this BCID
                        ind = bcidDict[bcid]
                    
                        # Now need to save bcid, mu, and calibLumi 
                        lraw = rawLumi[i]
                        lcal = self.caliObj[chan].calibrate(lraw)
                        muval = lcal / self.caliObj[chan].muToLumi

                        #if muval < 1:
                        #    print 'Chan %d BCID %d (Ind %d) Mu %f' % (chan, bcid, ind, muval)

                        # Keep track of total lumi per channel per lumi block
                        lbSum[chan] += lcal
                        
                        if not self.ntuple: continue
                        
                        if chan == 101:
                            nt.bcidStruct.fLumiLucAND[ind] = lcal
                            nt.bcidStruct.fRawLucAND[ind] = lraw
                            nt.bcidStruct.fMuLucAND[ind] = muval

                        elif chan == 102:
                            nt.bcidStruct.fLumiLucOR[ind] = lcal
                            nt.bcidStruct.fRawLucOR[ind] = lraw
                            nt.bcidStruct.fMuLucOR[ind] = muval
                            
                        elif chan == 105:
                            nt.bcidStruct.fLumiLucA[ind] = lcal
                            nt.bcidStruct.fRawLucA[ind] = lraw
                            nt.bcidStruct.fMuLucA[ind] = muval
                            
                        elif chan == 106:
                            nt.bcidStruct.fLumiLucC[ind] = lcal
                            nt.bcidStruct.fRawLucC[ind] = lraw
                            nt.bcidStruct.fMuLucC[ind] = muval
                            
                        elif chan == 201:
                            nt.bcidStruct.fLumiBcmHOR[ind] = lcal
                            nt.bcidStruct.fRawBcmHOR[ind] = lraw
                            nt.bcidStruct.fMuBcmHOR[ind] = muval
                            
                        elif chan == 202:
                            nt.bcidStruct.fLumiBcmHAND[ind] = lcal
                            nt.bcidStruct.fRawBcmHAND[ind] = lraw
                            nt.bcidStruct.fMuBcmHAND[ind] = muval

                        elif chan == 211:
                            nt.bcidStruct.fLumiBcmVOR[ind] = lcal
                            nt.bcidStruct.fRawBcmVOR[ind] = lraw
                            nt.bcidStruct.fMuBcmVOR[ind] = muval
                            
                        elif chan == 212:
                            nt.bcidStruct.fLumiBcmVAND[ind] = lcal
                            nt.bcidStruct.fRawBcmVAND[ind] = lraw
                            nt.bcidStruct.fMuBcmVAND[ind] = muval

                    # End loop over BCIDs
                    
                    # Fill DB object
                    if self.writedb:
                        payload = cool.Record(self.coolSpec)
                        payload['LBAvInstLumi'] = lbSum[chan]
                        payload['LBAvEvtsPerBX'] = lbSum[chan] / self.caliObj[chan].muToLumi / nBunch
                        payload['Valid'] = valid
                        
                        self.outFolder.storeObject(runlb, runlb+1, payload, chan, self.oflLumiTag, True)
                        # print run, lb, chan, payload

                # End of loop over channels
                if self.stableOnly and not isStable: continue

                
                # print 'Filled Run/LB: %d/%d Stable: %d N: %d' % (nt.lbDataStruct.fRun, nt.lbDataStruct.fLB, nt.lbDataStruct.fStable, nt.bcidStruct.fNBunch)

                if self.ntuple:
                    nt.tree.Fill()

            # End of loop over LBs
            if self.ntuple:
                nt.close()

        # End of loop over runs                
        if self.writedb:
            try:
                self.outFolder.flushStorageBuffer()
            except Exception, e:
                print e
                print 'Error storing lumi records to local DB!'
                
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
        self.caliReader.setChannel(self.lumiChanList)
        self.caliReader.readData()

        if self.verbose:
            print 'Read %d Calibration records' % len(self.caliReader.data)
            
    def loadBCIDLumi(self, iov):
        if self.verbose: print 'Loading BCID luminosity values'

        # Instantiate new COOL data reader if not already done
        if self.lumiReader == None:
            self.lumiReader = CoolDataReader('COOLONL_TDAQ/MONP200', '/TDAQ/OLC/BUNCHLUMIS')

        self.lumiReader.setIOVRange(iov[0], iov[1])
        self.lumiReader.setChannel(self.lumiChanList)
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

                # Only correct the calibration scale if requested
                if not self.correct: break

                # Do nothing if fill is after OLC update
                if self.currentRun > 180710: break
                
                # Try rescaling the calibration parameters
                # Pre-receiver card modification
                if 177530 < self.currentRun < 178110:

                    if chan == 101:
                        self.caliObj[chan].rescaleAND(1.035, 1.08)
                        # self.caliObj[chan].setAND(44170., 15000.)

                    elif chan == 102:
                        self.caliObj[chan].rescaleOR(1.035)
                        #self.caliObj[chan].setOR(44170.)

                    elif chan == 105:
                        self.caliObj[chan].rescaleOR(1.045)
                        # self.caliObj[chan].setOR(29430.)
                    
                    elif chan == 106:
                        self.caliObj[chan].rescaleOR(1.55)
                        # self.caliObj[chan].setOR(29800.)


                # Lucid receiver card modification
                if self.currentRun > 179580:
                    if chan == 101:
                        # self.caliObj[chan].rescaleAND(1.06, 1.15)
                        self.caliObj[chan].setAND(44170., 15000.)

                    elif chan == 102:
                        # self.caliObj[chan].rescaleOR(1.06)
                        self.caliObj[chan].setOR(44170.)

                    elif chan == 105:
                        # self.caliObj[chan].rescaleOR(1.077)
                        self.caliObj[chan].setOR(29430.)
                    
                    elif chan == 106:
                        # self.caliObj[chan].rescaleOR(1.091)
                        self.caliObj[chan].setOR(29800.)

                # Only modify BCM after threshold change    
                if self.currentRun > 180123:
                    if chan == 201:
                        # self.caliObj[chan].rescaleOR(1.037)
                        self.caliObj[chan].setOR(4764.)

                    elif chan == 202:
                        # self.caliObj[chan].rescaleAND(1.037, 1.037*1.037)
                        self.caliObj[chan].setAND(4764., 142.27)

                    elif chan == 211:
                        # self.caliObj[chan].rescaleOR(1.040)
                        self.caliObj[chan].setOR(4778.)

                    elif chan == 212:
                        # self.caliObj[chan].rescaleAND(1.040, 1.040*1.040)
                        self.caliObj[chan].setAND(4778., 143.1)
                    
                break
                
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

        parser.add_option("-r", "--updateRun",
                          dest="runlist", metavar="RUN",
                          help="update specific run, or comma separated list")

        parser.add_option("--noScale",
                          action="store_false", default=self.correct, dest="correct",
                          help="Don't correct calibration scale (default: Do)")

        parser.add_option("--noNtuple",
                          action="store_false", default=self.ntuple, dest='ntuple',
                          help="Don't store output ntuple (default: Do)")

        parser.add_option("--writeDB",
                          action="store_true", default=self.writedb, dest='writedb',
                          help="Write local DB instance (default: False)")

        parser.add_option("-t", "--tag",
                           dest="lumitag", metavar="TAG", default=self.oflLumiTag,
                           help='output luminosity tag (default: %s)' % self.oflLumiTag)
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.verbose = options.verbose
        self.correct = options.correct
        self.ntuple = options.ntuple
        self.writedb = options.writedb
        self.oflLumiTag = options.lumitag
        
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

    def createOutputDB(self):
               print 'calibNtupleMaker.createOutputDB() called'
               
               print 'calibNtupleMaker.createOutputDB() - connecting to DB', self.outDBString
               try:
                   self.outDB = forceOpen(self.outDBString)
               except Exception, e:
                   print e
                   print "calibNtupleMaker.createOutputDB() - Could not open DB %s" % self.outDBString
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
                   print "calibNtupleMaker.createOutputDB() - Could not access or create folder %s" % self.oflLumiFolder
                   return False
               
               print "calibNtupleMaker.createOutputDB() - Opened %s %s" % (self.outDBString, self.oflLumiFolder)
               self.outFolder.setupStorageBuffer()
               
               return True
           
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

    def setOR(self, sigvis):
        print 'Changing sigma_inel/sigma_vis from %f to %f' % (self.parVec[0], 71500./sigvis)
        self.parVec[0] = 71500./sigvis
        
    def setAND(self, sigvisOR, sigvisAND):
        print 'Changing sigma_vis(OR)  from %f to %f' % (self.parVec[0], sigvisOR)
        print 'Changing sigma_vis(AND) from %f to %f' % (self.parVec[1], sigvisAND)
        self.parVec[0] = sigvisOR
        self.parVec[1] = sigvisAND
        self.parVec[5] = 71500./sigvisAND
        
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
    
        
class NtupleHandler:

    def __init__(self):

        self.fileName = 'lumi.root'
        self.treeName = 'lumiData'
        self.file = None
        self.tree = None
        
    def open(self):
        print 'NtupleHandler.open() called'

        self.file = TFile(self.fileName, 'recreate')
        self.tree = TTree(self.treeName, self.treeName)

    def close(self):
        print 'ScanNtupleHandler.close() called'
        
        self.file.Write()
        self.file.Close()
        
    def init(self):
        print 'ScanNtupleHandler.init() called'

        self.initLBData()
        self.initBCIDData()

    # Information about the general lumi block
    def initLBData(self):

        #
        # Define LBDATA tree
        # Time is in ms
        structStr = "struct LBDataStruct {\
                ULong64_t fStartTime;\
                ULong64_t fEndTime;\
                UInt_t fRun;\
                UInt_t fLB;\
                UInt_t fStable;\
            };"
        
        # Replace sizes if needed
        gROOT.ProcessLine(structStr)
        from ROOT import LBDataStruct
        self.lbDataStruct = LBDataStruct()

        # l = 64 bit unsigned int, L = 64 bit signed int
        self.tree.Branch('LBDATA', self.lbDataStruct, 'StartTime/l:EndTime/l:Run/i:LB/i:stable/i')
        

    # Pass an IObject object for a single ntuple entry
    def fillLBData(self, obj):
        self.lbDataStruct.fStartTime = obj.since() # Time in ns
        self.lbDataStruct.fEndTime = obj.until()   # Time in ns
        self.lbDataStruct.fRun = obj.payload()['RunLB'] >> 32
        self.lbDataStruct.fLB = obj.payload()['RunLB']&0xFFFFFFFF
        
    # All BCID-dependent quantities
    def initBCIDData(self):

        gROOT.ProcessLine("struct BCIDStruct {\
        Float_t fMuToLumi;\
        Int_t fNBunch;\
        Int_t fBCIDs[3564];\
        Float_t fMuLucOR[3564];\
        Float_t fMuLucAND[3564];\
        Float_t fMuLucA[3564];\
        Float_t fMuLucC[3564];\
        Float_t fMuBcmHOR[3564];\
        Float_t fMuBcmHAND[3564];\
        Float_t fMuBcmVOR[3564];\
        Float_t fMuBcmVAND[3564];\
        Float_t fLumiLucOR[3564];\
        Float_t fLumiLucAND[3564];\
        Float_t fLumiLucA[3564];\
        Float_t fLumiLucC[3564];\
        Float_t fLumiBcmHOR[3564];\
        Float_t fLumiBcmHAND[3564];\
        Float_t fLumiBcmVOR[3564];\
        Float_t fLumiBcmVAND[3564];\
        Float_t fRawLucOR[3564];\
        Float_t fRawLucAND[3564];\
        Float_t fRawLucA[3564];\
        Float_t fRawLucC[3564];\
        Float_t fRawBcmHOR[3564];\
        Float_t fRawBcmHAND[3564];\
        Float_t fRawBcmVOR[3564];\
        Float_t fRawBcmVAND[3564];\
        };")


        from ROOT import BCIDStruct
        self.bcidStruct = BCIDStruct()

        self.tree.Branch('muToLumi', AddressOf(self.bcidStruct, 'fMuToLumi'), 'muToLumi/F')
        self.tree.Branch('nCollBG', AddressOf(self.bcidStruct, 'fNBunch'), 'nCollBG/i')
        self.tree.Branch('BCID', AddressOf(self.bcidStruct, 'fBCIDs'), 'BCID[nCollBG]/i')

        #self.tree.Branch('LumiLucOR',  AddressOf(self.bcidStruct, 'fLumiLucOR'), 'LumiLucOR[nCollBG]/F')
        #self.tree.Branch('LumiLucAND', AddressOf(self.bcidStruct, 'fLumiLucAND'), 'LumiLucAND[nCollBG]/F')
        #self.tree.Branch('LumiBcmOR',  AddressOf(self.bcidStruct, 'fLumiBcmOR'), 'LumiBcmOR[nCollBG]/F')
        #self.tree.Branch('LumiBcmAND', AddressOf(self.bcidStruct, 'fLumiBcmAND'), 'LumiBcmAND[nCollBG]/F')

        #self.tree.Branch('RawLucOR',  AddressOf(self.bcidStruct, 'fRawLucOR'), 'RawLucOR[nCollBG]/F')
        #self.tree.Branch('RawLucAND', AddressOf(self.bcidStruct, 'fRawLucAND'), 'RawLucAND[nCollBG]/F')
        #self.tree.Branch('RawBcmOR',  AddressOf(self.bcidStruct, 'fRawBcmOR'), 'RawBcmOR[nCollBG]/F')
        #self.tree.Branch('RawBcmAND', AddressOf(self.bcidStruct, 'fRawBcmAND'), 'RawBcmAND[nCollBG]/F')

        self.tree.Branch('MuLucOR',  AddressOf(self.bcidStruct, 'fMuLucOR'), 'MuLucOR[nCollBG]/F')
        self.tree.Branch('MuLucA',  AddressOf(self.bcidStruct, 'fMuLucA'), 'MuLucA[nCollBG]/F')
        self.tree.Branch('MuLucC',  AddressOf(self.bcidStruct, 'fMuLucC'), 'MuLucC[nCollBG]/F')
        self.tree.Branch('MuLucAND', AddressOf(self.bcidStruct, 'fMuLucAND'), 'MuLucAND[nCollBG]/F')
        self.tree.Branch('MuBcmHOR',  AddressOf(self.bcidStruct, 'fMuBcmHOR'), 'MuBcmHOR[nCollBG]/F')
        self.tree.Branch('MuBcmHAND', AddressOf(self.bcidStruct, 'fMuBcmHAND'), 'MuBcmHAND[nCollBG]/F')
        self.tree.Branch('MuBcmVOR',  AddressOf(self.bcidStruct, 'fMuBcmVOR'), 'MuBcmVOR[nCollBG]/F')
        self.tree.Branch('MuBcmVAND', AddressOf(self.bcidStruct, 'fMuBcmVAND'), 'MuBcmVAND[nCollBG]/F')
        
        
# Executed from the command line
if __name__ == '__main__':
    mhm = CalibNtupleMaker()
    mhm.execute()

                
