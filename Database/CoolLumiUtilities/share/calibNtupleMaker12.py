#!/usr/bin/env python
#
# calibNtupleMaker.py
#
# Produce an ntuple of per-bcid luminosities using the raw lumi data
# 23 May 2011 Modified to write out all BCID
#
# Updated for use in 2012.  Removed historical stuff from 2011
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

from CoolLumiUtilities.LumiCalibrator import LumiCalibrator
from CoolLumiUtilities.LumiBlobConversion import unpackBCIDMask, unpackBCIDValues
from CoolLumiUtilities.LumiBlobConversion import unpackLiveFraction, unpackBunchGroup

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import forceOpen,ensureFolder,athenaDesc

from ROOT import TFile, TH1D, TTree, gROOT, AddressOf

class CalibNtupleMaker:

    def __init__(self):

        # Control output level
        self.verbose = True

        # Luminosity Channel
        self.lumiChanList = []
        self.lumiChanList2011 = [101,102,103,104,105,106,201,202,204,211,212,214,411,412,413,414] # Lucid Event OR
        self.lumiChanList2012 = [101,102,103,104,105,106,201,202,206,207,211,212,216,217,221,222,226,1001,1002,1004,1011,1012,1014]

        self.chan2012 = True

        # Switch to include LUCID fiber channels
        self.fibers = False
        self.fiberList = [111, 112, 113, 114, 115, 116]
        
        # Stable only
        self.stableOnly = True

        # Scale calibration
        self.correct = True

        # Don't calibrate
        self.noCalib = False

        # Don't save currents
        self.noCurrent = False

        # Correct for afterglow in calibrated values in physics bunch group, simple BCID-1 subtraction
        self.afterglow = True
        
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
        self.oflLumiTag = 'OflLumi-8TeV-001'
        self.outDB = None
        self.coolSpec = None
        self.outFolder = None
        
        # Data readers
        self.maskReader = None
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

        self.currentRun = 0
        
        self.nBCID = 3564

        self.outdir = '.'
        
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
                if self.chan2012:
                    nt.chan2011 = False
                    nt.chan2012 = True
                else:
                    nt.chan2011 = True
                    nt.chan2012 = False

                nt.fibers = self.fibers
                nt.fileName = '%s/r%d.root' % (self.outdir, run)
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

            # Current objects keyed by [runlb]
            currDict = dict()
            for obj in self.currReader.data:
                runlb = obj.payload()['RunLB']
                currDict[runlb] = obj

            # PMT currents keyed by [runlb]
            pmtDict = dict()
            fibDict = dict() # Fiber currents
            for obj in self.pmtReader.data:
                runlb = obj.payload()['RunLB']
                if obj.channelId() == 0:
                    pmtDict[runlb] = obj
                elif obj.channelId() == 1:
                    fibDict[runlb] = obj
                else:
                    print 'Found channel %d in pmtReader!' % obj.channel()
                
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

                # Get integer to make average mu value below
                nBunch = len(self.bgObj.bg)
                if nBunch < 1: nBunch = 1

                # Zero all storage locations
                nt.clearBCIDData()

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

                            if runlb in pmtDict:
                                nt.lbDataStruct.fPmtA = pmtDict[runlb].payload()['CurrentSideA']
                                nt.lbDataStruct.fPmtC = pmtDict[runlb].payload()['CurrentSideC']
                                # print 'Found Run %d LB %d PMTA %f PMTC %f' % (run, lb, nt.lbDataStruct.fPmtA, nt.lbDataStruct.fPmtC)

                            if runlb in fibDict:
                                nt.lbDataStruct.fFibA = fibDict[runlb].payload()['CurrentSideA']
                                nt.lbDataStruct.fFibC = fibDict[runlb].payload()['CurrentSideC']
                                # print 'Found Run %d LB %d FIBA %f FIBC %f' % (run, lb, nt.lbDataStruct.fFibA, nt.lbDataStruct.fFibC)
                                
                        # These change slowly, so checking them every run/lb is OK
                        # Make sure BCID mask is valid
                        if not self.updateBCIDMask(startTime):
                            print "Couldn't find valid BCID mask data for Run %d LB %d!" % (run, lb)
                            continue

                        # Do this here so the BCID mask is up to date
                        if self.ntuple and not self.noCurrent:
                            if runlb in currDict:
                                nt.fillCurrentData(currDict[runlb], self.maskObj)
                            
                    # Make sure calibration is valid
                    if not self.noCalib and not self.updateCalibration(startTime, chan):
                        print 'Error finding calibration for Run %d LB %d Chan %d!' % (run, lb, chan)
                        continue

                    # Also must update PMT currents in calibration object
                    if not self.noCalib:
                        self.caliObj[chan].setLucidCurrent(pmtDict[runlb].payload())
                        # self.caliObj[chan].setFiberCurrent(fibDict[runlb].payload())

                    # Save this so I don't need to save all calibrated data
                    if self.ntuple and not self.noCalib:
                        nt.bcidStruct.fMuToLumi = self.caliObj[chan].muToLumi
                    
                    # Now we want to start extracting bunch-by-bunch information

                    # Get raw lumi
                    normValue = obj.payload()['AverageRawInstLum']
                    blobValue = obj.payload()['BunchRawInstLum']
                    bcidVec, rawLumi = unpackBCIDValues(blobValue, self.maskObj.coll, normValue)

                    # dict to hold BCID lumi keyed by BCID
                    bcidLumi = dict() 
                    for i in range(len(rawLumi)):

                        # Check if this is in our bunch group (only really need to fill this once)
                        bcid = bcidVec[i]

                        # Protect against any weird values
                        if bcid >= self.nBCID:
                            print 'BCID %d found >= %d!' % (bcid, self.nBCID)
                            continue

                        if bcid in self.bgObj.bg:
                            nt.bcidStruct.fStatus[bcid] = 1
                        else:
                            nt.bcidStruct.fStatus[bcid] = 0

                        # Now need to save bcid, mu, and calibLumi 
                        lraw = rawLumi[i]
                        lcal = 0.
                        if not self.noCalib:
                            lcal = self.caliObj[chan].calibrate(lraw)

                        bcidLumi[bcid] = lcal

                        # Subtract afterglow (simple subtraction) from physics BG
                        if self.afterglow and bcid in self.bgObj.bg:
                            lcal = bcidLumi[bcid] - bcidLumi.get(bcid-1, 0.)

                        muval = 0.
                        if not self.noCalib:
                            muval = lcal / self.caliObj[chan].muToLumi

                        #if muval < 1:
                        #    print 'Chan %d BCID %d (Ind %d) Mu %f Status %d' % (chan, bcid, ind, muval, nt.bcidStruct.fStatus[bcid])

                        # Keep track of total lumi per channel per lumi block
                        # Only if in physics bunch group!
                        if bcid in self.bgObj.bg:
                            lbSum[chan] += lcal
                        
                        if not self.ntuple: continue

                        nt.fillBCIDData(chan, bcid, lraw, muval)
                        
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

        self.loadBCIDCurrents(iov)

        # Read LUCID PMT Currents
        self.loadPmtData(iov)
        
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
            # Switch at start of September
            if iov[0] < 1315395299650946035:
                self.lumiReader = CoolDataReader('COOLONL_TDAQ/MONP200', '/TDAQ/OLC/BUNCHLUMIS')
            else:
                self.lumiReader = CoolDataReader('COOLONL_TDAQ/COMP200', '/TDAQ/OLC/BUNCHLUMIS')

        self.lumiReader.setIOVRange(iov[0], iov[1])
        self.lumiReader.setChannel(self.lumiChanList)
        self.lumiReader.readData()

        if self.verbose:
            print 'Read %d Lumi records' % len(self.lumiReader.data)
            

    # Bunch currents
    def loadBCIDCurrents(self, iov):
        if self.verbose: print 'Loading Bunch Current information'

        if self.currReader == None:
            # self.currReader = CoolDataReader('COOLONL_TDAQ/MONP200', '/TDAQ/OLC/LHC/BUNCHDATA')
            self.currReader = CoolDataReader('COOLONL_TDAQ/COMP200', '/TDAQ/OLC/LHC/BUNCHDATA')

        self.currReader.setIOVRange(iov[0], iov[1])
        self.currReader.setChannelId(1) # 0 = BPTX, 1 = Fast BCT
        self.currReader.readData()

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

                # Only 2012 updates here

                # Try p-Pb scaling
                if 210184 <= self.currentRun <= 210186:

                    if chan == 101: # EventAND
                        self.caliObj[chan].setAND(1812240., 956880., 2160000.)
                        
                    elif chan == 102: # EventOR
                        self.caliObj[chan].setOR(1812240., 2160000.)

                    elif chan == 105: # EventA
                        self.caliObj[chan].setOR(1522800., 2160000.)
                        
                    elif chan == 106: # EventC
                        self.caliObj[chan].setOR(1246320., 2160000.)

                                    
                # Do nothing after April technical stop
                if self.currentRun > 201556: break
                
                # Try rescaling the calibration parameters
                if 200800 < self.currentRun < 201557:

                    if chan == 101: # EventAND
                        self.caliObj[chan].setAND(36667., 9574.)
                        self.caliObj[chan].setCurrentScale(3, 0., 0.1189, -0.000672)
                        
                    elif chan == 102: # EventOR
                        self.caliObj[chan].setOR(36667.)
                        self.caliObj[chan].setCurrentScale(2, 0., 0.0449)

                    elif chan == 103: # HitOR
                        self.caliObj[chan].setOR(3294.)
                        self.caliObj[chan].setCurrentScale(3, -16.58, 0.2076, -0.000960)
                        
                    elif chan == 105: # EventA
                        self.caliObj[chan].setOR(23597.)
                        self.caliObj[chan].setCurrentScale(2, 0., 0.0399)
                    
                    elif chan == 106: # EventC
                        self.caliObj[chan].setOR(22663.)
                        self.caliObj[chan].setCurrentScale(2, 0., 0.0526)
                        
                    elif chan == 201: # BCMH OR
                        self.caliObj[chan].setOR(4872.67)
                    elif chan == 202: # BCMH AND
                        self.caliObj[chan].setAND(4872.67, 150.33)
                    elif chan == 206: # BCMH ORC
                        self.caliObj[chan].setOR(2539.33)
                        
                    elif chan == 211: # BCMV OR
                        self.caliObj[chan].setOR(4861.33)
                    elif chan == 212: # BCMV AND
                        self.caliObj[chan].setAND(4861.33, 149.07)
                    elif chan == 216: # BCMV ORC
                        self.caliObj[chan].setOR(2512.33)

                    elif chan == 221: # BCMT OR
                        self.caliObj[chan].setOR(9127.)
                    elif chan == 222: # BCMT AND
                        self.caliObj[chan].setAND(9127., 549.73)
                    elif chan == 226: # BCMT ORC
                        self.caliObj[chan].setOR(4871.33)

                    elif chan == 1001: # BCM11H OR 
                        self.caliObj[chan].setOR(4909.67)
                    elif chan == 1002: # BCM11H AND
                        self.caliObj[chan].setAND(4909.67, 153.1)
                    elif chan == 1011: # BCM11V OR 
                        self.caliObj[chan].setOR(4883.33)
                    elif chan == 1012: # BCM11V AND
                        self.caliObj[chan].setAND(4883.33, 150.97)
                        
                        
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

        parser.add_option("-o", "--outputDir",
                          dest="outdir", metavar="DIR", default=self.outdir,
                          help='directory for output ntuple (default: %s)' % self.outdir)

        parser.add_option("--noStable",
                          action="store_false", default=self.stableOnly, dest="stable",
                          help="turn off stable beams requirements (default: stable only)")
        
        parser.add_option("--noCalib",
                          action="store_true", default=self.noCalib, dest="noCalib",
                          help="Don't calibrate raw luminosity per BCID (default: do)")
        
        parser.add_option("--noCurrent",
                          action="store_true", default=self.noCurrent, dest="noCurrent",
                          help="Don't save bunch currents by BCID to ntuple (default: do)")

        parser.add_option("--fibers",
                          action="store_true", default=self.fibers, dest="fibers",
                          help="Save LUCID fiber channels to ntuple (default: don't save)")

        parser.add_option("--noAfterglow",
                          action="store_false", default=self.afterglow, dest="after",
                          help="Turn off simple afterglow correction for PhysBG")
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.verbose = options.verbose
        self.correct = options.correct
        self.ntuple = options.ntuple
        self.writedb = options.writedb
        self.oflLumiTag = options.lumitag
        self.outdir = options.outdir
        self.stableOnly = options.stable
        self.noCalib = options.noCalib
        self.noCurrent = options.noCurrent
        self.fibers = options.fibers
        self.afterglow = options.after

        # Set channel list
        #self.chan2012 = options.chan2012

        if self.chan2012:
            self.lumiChanList = self.lumiChanList2012
        else:
            self.lumiChanList = self.lumiChanList2011

        if self.fibers:
            self.lumiChanList += self.fiberList
            
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

    def setCurrentScale(self, ver, a, b, c=0.):

        self.currVersion = ver
        self.currParA = a
        self.currParB = b
        self.currParC = c

        print 'Type: %s' % self.fType
        print 'Changing current calibration to %d %f %f %f' % (ver, a, b, c)
        
    def setOR(self, sigvis, sigtot=73000.):
        print 'Changing sigma_inel/sigma_vis from %f to %f' % (self.parVec[0], sigtot/sigvis)
        self.parVec[0] = sigtot/sigvis
        
    def setAND(self, sigvisOR, sigvisAND, sigtot=73000.):
        print 'Changing sigma_vis(OR)  from %f to %f' % (self.parVec[0], sigvisOR)
        print 'Changing sigma_vis(AND) from %f to %f' % (self.parVec[1], sigvisAND)
        self.parVec[0] = sigvisOR
        self.parVec[1] = sigvisAND
        if self.fType == 'LookupTable_EventAND_Lin':
            self.parVec[5] = sigtot/sigvisAND
        else:
            self.parVec[7] = sigtot/sigvisAND 
        
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

        self.nBCID = 3564

        # Control channel lists
        self.chan2011 = True
        self.chan2012 = False

        # Control ntuple content
        self.fibers = False
        self.oldBCM = False
        
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

        self.lbDataStruct = self.getLBDataStruct()

        # l = 64 bit unsigned int, L = 64 bit signed int
        self.tree.Branch('LBDATA', self.lbDataStruct, 'StartTime/l:EndTime/l:Run/i:LB/i:stable/i')
        
        self.tree.Branch('AvgBeam1', AddressOf(self.lbDataStruct, 'fAvgBeam1'), 'AvgBeam1/F')
        self.tree.Branch('AvgBeam2', AddressOf(self.lbDataStruct, 'fAvgBeam2'), 'AvgBeam2/F')
        self.tree.Branch('pmtA', AddressOf(self.lbDataStruct, 'fPmtA'), 'pmtA/F')
        self.tree.Branch('pmtC', AddressOf(self.lbDataStruct, 'fPmtC'), 'pmtC/F')
        self.tree.Branch('fibA', AddressOf(self.lbDataStruct, 'fFibA'), 'fibA/F')
        self.tree.Branch('fibC', AddressOf(self.lbDataStruct, 'fFibC'), 'fibC/F')

    # Pass an IObject object for a single ntuple entry
    def fillLBData(self, obj):
        self.lbDataStruct.fStartTime = obj.since() # Time in ns
        self.lbDataStruct.fEndTime = obj.until()   # Time in ns
        self.lbDataStruct.fRun = obj.payload()['RunLB'] >> 32
        self.lbDataStruct.fLB = obj.payload()['RunLB']&0xFFFFFFFF
        self.lbDataStruct.fAvgBeam1 = 0.
        self.lbDataStruct.fAvgBeam2 = 0.
        self.lbDataStruct.fPmtA = 0.
        self.lbDataStruct.fPmtC = 0.
        self.lbDataStruct.fFibA = 0.
        self.lbDataStruct.fFibC = 0.
        
    # Return LBData struct
    def getLBDataStruct(self):

        #
        # Define LBDATA tree
        # Time is in ms
        structStr = "struct LBDataStruct {\
                ULong64_t fStartTime;\
                ULong64_t fEndTime;\
                UInt_t fRun;\
                UInt_t fLB;\
                UInt_t fStable;\
                Float_t fAvgBeam1;\
                Float_t fAvgBeam2;\
                Float_t fPmtA;\
                Float_t fPmtC;\
                Float_t fFibA;\
                Float_t fFibC;\
            };"
        
        # Replace sizes if needed
        gROOT.ProcessLine(structStr)
        from ROOT import LBDataStruct
        return LBDataStruct()

    def getMuDataStruct(self):
        # Keep track of collisions in Status 
        gROOT.ProcessLine("struct MuStruct {\
        Float_t fMuToLumi;\
        UInt_t fStatus[3564];\
        Float_t fMuLucOR[3564];\
        Float_t fMuLucAND[3564];\
        Float_t fMuLucHitOR[3564];\
        Float_t fMuLucHitAND[3564];\
        Float_t fMuLucA[3564];\
        Float_t fMuLucC[3564];\
        \
        Float_t fMuLFiOR[3564];\
        Float_t fMuLFiAND[3564];\
        Float_t fMuLFiHitOR[3564];\
        Float_t fMuLFiHitAND[3564];\
        Float_t fMuLFiA[3564];\
        Float_t fMuLFiC[3564];\
        \
        Float_t fMuBcmHOR[3564];\
        Float_t fMuBcmHAND[3564];\
        Float_t fMuBcmHXORA[3564];\
        Float_t fMuBcmHXORC[3564];\
        Float_t fMuBcmHORA[3564];\
        Float_t fMuBcmHORC[3564];\
        Float_t fMuBcmHAND25[3564];\
        \
        Float_t fMuBcmVOR[3564];\
        Float_t fMuBcmVAND[3564];\
        Float_t fMuBcmVXORA[3564];\
        Float_t fMuBcmVXORC[3564];\
        Float_t fMuBcmVORA[3564];\
        Float_t fMuBcmVORC[3564];\
        Float_t fMuBcmVAND25[3564];\
        \
        Float_t fMuBcmTOR[3564];\
        Float_t fMuBcmTAND[3564];\
        Float_t fMuBcmTORC[3564];\
        \
        Float_t fMuBcm11HOR[3564];\
        Float_t fMuBcm11HAND[3564];\
        Float_t fMuBcm11HXORC[3564];\
        \
        Float_t fMuBcm11VOR[3564];\
        Float_t fMuBcm11VAND[3564];\
        Float_t fMuBcm11VXORC[3564];\
        \
        Float_t fMuZdcAND[3564];\
        Float_t fMuZdcOR[3564];\
        Float_t fMuZdcORA[3564];\
        Float_t fMuZdcORC[3564];\
        Float_t fBeam1[3564];\
        Float_t fBeam2[3564];\
        };")
        
        from ROOT import MuStruct
        return MuStruct()

    def getRawDataStruct(self):

        # Store all BCIDs
        gROOT.ProcessLine("struct RawStruct {\
        Float_t fRawLucOR[3564];\
        Float_t fRawLucAND[3564];\
        Float_t fRawLucHitOR[3564];\
        Float_t fRawLucHitAND[3564];\
        Float_t fRawLucA[3564];\
        Float_t fRawLucC[3564];\
        \
        Float_t fRawLFiOR[3564];\
        Float_t fRawLFiAND[3564];\
        Float_t fRawLFiHitOR[3564];\
        Float_t fRawLFiHitAND[3564];\
        Float_t fRawLFiA[3564];\
        Float_t fRawLFiC[3564];\
        \
        Float_t fRawBcmHOR[3564];\
        Float_t fRawBcmHAND[3564];\
        Float_t fRawBcmHXORA[3564];\
        Float_t fRawBcmHXORC[3564];\
        Float_t fRawBcmHORA[3564];\
        Float_t fRawBcmHORC[3564];\
        Float_t fRawBcmHAND25[3564];\
        \
        Float_t fRawBcmVOR[3564];\
        Float_t fRawBcmVAND[3564];\
        Float_t fRawBcmVXORA[3564];\
        Float_t fRawBcmVXORC[3564];\
        Float_t fRawBcmVORA[3564];\
        Float_t fRawBcmVORC[3564];\
        Float_t fRawBcmVAND25[3564];\
        \
        Float_t fRawBcmTOR[3564];\
        Float_t fRawBcmTAND[3564];\
        Float_t fRawBcmTORC[3564];\
        \
        Float_t fRawBcm11HOR[3564];\
        Float_t fRawBcm11HAND[3564];\
        Float_t fRawBcm11HXORC[3564];\
        \
        Float_t fRawBcm11VOR[3564];\
        Float_t fRawBcm11VAND[3564];\
        Float_t fRawBcm11VXORC[3564];\
        \
        Float_t fRawZdcAND[3564];\
        Float_t fRawZdcOR[3564];\
        Float_t fRawZdcORA[3564];\
        Float_t fRawZdcORC[3564];\
        };")

        from ROOT import RawStruct
        return RawStruct()

    # All BCID-dependent quantities
    def initBCIDData(self):
        

        self.bcidStruct = self.getMuDataStruct()
        self.rawStruct = self.getRawDataStruct()

        self.tree.Branch('muToLumi', AddressOf(self.bcidStruct, 'fMuToLumi'), 'muToLumi/F')
        self.tree.Branch('Status',   AddressOf(self.bcidStruct, 'fStatus'), 'Status[3564]/i')

        self.tree.Branch('RawLucOR',  AddressOf(self.rawStruct, 'fRawLucOR'), 'RawLucOR[3564]/F')
        self.tree.Branch('RawLucA',  AddressOf(self.rawStruct, 'fRawLucA'), 'RawLucA[3564]/F')
        self.tree.Branch('RawLucC',  AddressOf(self.rawStruct, 'fRawLucC'), 'RawLucC[3564]/F')
        self.tree.Branch('RawLucAND', AddressOf(self.rawStruct, 'fRawLucAND'), 'RawLucAND[3564]/F')
        self.tree.Branch('RawLucHitOR', AddressOf(self.rawStruct, 'fRawLucHitOR'), 'RawLucHitOR[3564]/F')
        self.tree.Branch('RawLucHitAND', AddressOf(self.rawStruct, 'fRawLucHitAND'), 'RawLucHitAND[3564]/F')

        if self.fibers:
            self.tree.Branch('RawLFiOR',  AddressOf(self.rawStruct, 'fRawLFiOR'), 'RawLFiOR[3564]/F')
            self.tree.Branch('RawLFiA',  AddressOf(self.rawStruct, 'fRawLFiA'), 'RawLFiA[3564]/F')
            self.tree.Branch('RawLFiC',  AddressOf(self.rawStruct, 'fRawLFiC'), 'RawLFiC[3564]/F')
            self.tree.Branch('RawLFiAND', AddressOf(self.rawStruct, 'fRawLFiAND'), 'RawLFiAND[3564]/F')
            self.tree.Branch('RawLFiHitOR', AddressOf(self.rawStruct, 'fRawLFiHitOR'), 'RawLFiHitOR[3564]/F')
            self.tree.Branch('RawLFiHitAND', AddressOf(self.rawStruct, 'fRawLFiHitAND'), 'RawLFiHitAND[3564]/F')
        
        self.tree.Branch('RawBcmHOR',  AddressOf(self.rawStruct, 'fRawBcmHOR'), 'RawBcmHOR[3564]/F')
        self.tree.Branch('RawBcmHAND', AddressOf(self.rawStruct, 'fRawBcmHAND'), 'RawBcmHAND[3564]/F')
        if self.chan2011:
            self.tree.Branch('RawBcmHXORA',  AddressOf(self.rawStruct, 'fRawBcmHXORA'), 'RawBcmHXORA[3564]/F')
            self.tree.Branch('RawBcmHXORC',  AddressOf(self.rawStruct, 'fRawBcmHXORC'), 'RawBcmHXORC[3564]/F')
        if self.chan2012:
            # self.tree.Branch('RawBcmHORA',  AddressOf(self.rawStruct, 'fRawBcmHORA'), 'RawBcmHORA[3564]/F')
            self.tree.Branch('RawBcmHORC',  AddressOf(self.rawStruct, 'fRawBcmHORC'), 'RawBcmHORC[3564]/F')
            self.tree.Branch('RawBcmHAND25',  AddressOf(self.rawStruct, 'fRawBcmHAND25'), 'RawBcmHAND25[3564]/F')
        
        self.tree.Branch('RawBcmVOR',  AddressOf(self.rawStruct, 'fRawBcmVOR'), 'RawBcmVOR[3564]/F')
        self.tree.Branch('RawBcmVAND', AddressOf(self.rawStruct, 'fRawBcmVAND'), 'RawBcmVAND[3564]/F')
        if self.chan2011:
            self.tree.Branch('RawBcmVXORA',  AddressOf(self.rawStruct, 'fRawBcmVXORA'), 'RawBcmVXORA[3564]/F')
            self.tree.Branch('RawBcmVXORC',  AddressOf(self.rawStruct, 'fRawBcmVXORC'), 'RawBcmVXORC[3564]/F')
        if self.chan2012:
            # self.tree.Branch('RawBcmVORA',  AddressOf(self.rawStruct, 'fRawBcmVORA'), 'RawBcmVORA[3564]/F')
            self.tree.Branch('RawBcmVORC',  AddressOf(self.rawStruct, 'fRawBcmVORC'), 'RawBcmVORC[3564]/F')
            self.tree.Branch('RawBcmVAND25',  AddressOf(self.rawStruct, 'fRawBcmVAND25'), 'RawBcmVAND25[3564]/F')

        if self.chan2012:
            self.tree.Branch('RawBcmTOR',  AddressOf(self.rawStruct, 'fRawBcmTOR'), 'RawBcmTOR[3564]/F')
            self.tree.Branch('RawBcmTAND', AddressOf(self.rawStruct, 'fRawBcmTAND'), 'RawBcmTAND[3564]/F')
            self.tree.Branch('RawBcmTORC',  AddressOf(self.rawStruct, 'fRawBcmTORC'), 'RawBcmTORC[3564]/F')

            if self.oldBCM:
                self.tree.Branch('RawBcm11HOR',  AddressOf(self.rawStruct, 'fRawBcm11HOR'), 'RawBcm11HOR[3564]/F')
                self.tree.Branch('RawBcm11HAND', AddressOf(self.rawStruct, 'fRawBcm11HAND'), 'RawBcm11HAND[3564]/F')
                self.tree.Branch('RawBcm11HXORC',  AddressOf(self.rawStruct, 'fRawBcm11HXORC'), 'RawBcm11HXORC[3564]/F')
            
                self.tree.Branch('RawBcm11VOR',  AddressOf(self.rawStruct, 'fRawBcm11VOR'), 'RawBcm11VOR[3564]/F')
                self.tree.Branch('RawBcm11VAND', AddressOf(self.rawStruct, 'fRawBcm11VAND'), 'RawBcm11VAND[3564]/F')
                self.tree.Branch('RawBcm11VXORC',  AddressOf(self.rawStruct, 'fRawBcm11VXORC'), 'RawBcm11VXORC[3564]/F')

        if self.chan2011:    
            self.tree.Branch('RawZdcOR',  AddressOf(self.rawStruct, 'fRawZdcOR'),  'RawZdcOR[3564]/F')
            self.tree.Branch('RawZdcORA', AddressOf(self.rawStruct, 'fRawZdcORA'), 'RawZdcORA[3564]/F')
            self.tree.Branch('RawZdcORC', AddressOf(self.rawStruct, 'fRawZdcORC'), 'RawZdcORC[3564]/F')
            self.tree.Branch('RawZdcAND', AddressOf(self.rawStruct, 'fRawZdcAND'), 'RawZdcAND[3564]/F')

        self.tree.Branch('MuLucOR',  AddressOf(self.bcidStruct, 'fMuLucOR'), 'MuLucOR[3564]/F')
        self.tree.Branch('MuLucA',  AddressOf(self.bcidStruct, 'fMuLucA'), 'MuLucA[3564]/F')
        self.tree.Branch('MuLucC',  AddressOf(self.bcidStruct, 'fMuLucC'), 'MuLucC[3564]/F')
        self.tree.Branch('MuLucAND', AddressOf(self.bcidStruct, 'fMuLucAND'), 'MuLucAND[3564]/F')
        self.tree.Branch('MuLucHitOR', AddressOf(self.bcidStruct, 'fMuLucHitOR'), 'MuLucHitOR[3564]/F')
        self.tree.Branch('MuLucHitAND', AddressOf(self.bcidStruct, 'fMuLucHitAND'), 'MuLucHitAND[3564]/F')

        if self.fibers:
            self.tree.Branch('MuLFiOR',  AddressOf(self.bcidStruct, 'fMuLFiOR'), 'MuLFiOR[3564]/F')
            self.tree.Branch('MuLFiA',  AddressOf(self.bcidStruct, 'fMuLFiA'), 'MuLFiA[3564]/F')
            self.tree.Branch('MuLFiC',  AddressOf(self.bcidStruct, 'fMuLFiC'), 'MuLFiC[3564]/F')
            self.tree.Branch('MuLFiAND', AddressOf(self.bcidStruct, 'fMuLFiAND'), 'MuLFiAND[3564]/F')
            self.tree.Branch('MuLFiHitOR', AddressOf(self.bcidStruct, 'fMuLFiHitOR'), 'MuLFiHitOR[3564]/F')
            self.tree.Branch('MuLFiHitAND', AddressOf(self.bcidStruct, 'fMuLFiHitAND'), 'MuLFiHitAND[3564]/F')
        
        self.tree.Branch('MuBcmHOR',  AddressOf(self.bcidStruct, 'fMuBcmHOR'), 'MuBcmHOR[3564]/F')
        self.tree.Branch('MuBcmHAND', AddressOf(self.bcidStruct, 'fMuBcmHAND'), 'MuBcmHAND[3564]/F')
        if self.chan2011:
            self.tree.Branch('MuBcmHXORA',  AddressOf(self.bcidStruct, 'fMuBcmHXORA'), 'MuBcmHXORA[3564]/F')
            self.tree.Branch('MuBcmHXORC',  AddressOf(self.bcidStruct, 'fMuBcmHXORC'), 'MuBcmHXORC[3564]/F')
        if self.chan2012:
            # self.tree.Branch('MuBcmHORA',  AddressOf(self.bcidStruct, 'fMuBcmHORA'), 'MuBcmHORA[3564]/F')
            self.tree.Branch('MuBcmHORC',  AddressOf(self.bcidStruct, 'fMuBcmHORC'), 'MuBcmHORC[3564]/F')
            self.tree.Branch('MuBcmHAND25',  AddressOf(self.bcidStruct, 'fMuBcmHAND25'), 'MuBcmHAND25[3564]/F')
        
        self.tree.Branch('MuBcmVOR',  AddressOf(self.bcidStruct, 'fMuBcmVOR'), 'MuBcmVOR[3564]/F')
        self.tree.Branch('MuBcmVAND', AddressOf(self.bcidStruct, 'fMuBcmVAND'), 'MuBcmVAND[3564]/F')
        if self.chan2011:
            self.tree.Branch('MuBcmVXORA',  AddressOf(self.bcidStruct, 'fMuBcmVXORA'), 'MuBcmVXORA[3564]/F')
            self.tree.Branch('MuBcmVXORC',  AddressOf(self.bcidStruct, 'fMuBcmVXORC'), 'MuBcmVXORC[3564]/F')
        if self.chan2012:
            # self.tree.Branch('MuBcmVORA',  AddressOf(self.bcidStruct, 'fMuBcmVORA'), 'MuBcmVORA[3564]/F')
            self.tree.Branch('MuBcmVORC',  AddressOf(self.bcidStruct, 'fMuBcmVORC'), 'MuBcmVORC[3564]/F')
            self.tree.Branch('MuBcmVAND25',  AddressOf(self.bcidStruct, 'fMuBcmVAND25'), 'MuBcmVAND25[3564]/F')

        if self.chan2012:
            self.tree.Branch('MuBcmTOR',  AddressOf(self.bcidStruct, 'fMuBcmTOR'), 'MuBcmTOR[3564]/F')
            self.tree.Branch('MuBcmTAND', AddressOf(self.bcidStruct, 'fMuBcmTAND'), 'MuBcmTAND[3564]/F')
            self.tree.Branch('MuBcmTORC',  AddressOf(self.bcidStruct, 'fMuBcmTORC'), 'MuBcmTORC[3564]/F')

            if self.oldBCM:
                self.tree.Branch('MuBcm11HOR',  AddressOf(self.bcidStruct, 'fMuBcm11HOR'), 'MuBcm11HOR[3564]/F')
                self.tree.Branch('MuBcm11HAND', AddressOf(self.bcidStruct, 'fMuBcm11HAND'), 'MuBcm11HAND[3564]/F')
                self.tree.Branch('MuBcm11HXORC',  AddressOf(self.bcidStruct, 'fMuBcm11HXORC'), 'MuBcm11HXORC[3564]/F')
            
                self.tree.Branch('MuBcm11VOR',  AddressOf(self.bcidStruct, 'fMuBcm11VOR'), 'MuBcm11VOR[3564]/F')
                self.tree.Branch('MuBcm11VAND', AddressOf(self.bcidStruct, 'fMuBcm11VAND'), 'MuBcm11VAND[3564]/F')
                self.tree.Branch('MuBcm11VXORC',  AddressOf(self.bcidStruct, 'fMuBcm11VXORC'), 'MuBcm11VXORC[3564]/F')
            
        if self.chan2011:
            self.tree.Branch('MuZdcOR',  AddressOf(self.bcidStruct, 'fMuZdcOR'),  'MuZdcOR[3564]/F')
            self.tree.Branch('MuZdcORA', AddressOf(self.bcidStruct, 'fMuZdcORA'), 'MuZdcORA[3564]/F')
            self.tree.Branch('MuZdcORC', AddressOf(self.bcidStruct, 'fMuZdcORC'), 'MuZdcORC[3564]/F')
            self.tree.Branch('MuZdcAND', AddressOf(self.bcidStruct, 'fMuZdcAND'), 'MuZdcAND[3564]/F')
            
        self.tree.Branch('Beam1', AddressOf(self.bcidStruct, 'fBeam1'), 'Beam1[3564]/F')
        self.tree.Branch('Beam2', AddressOf(self.bcidStruct, 'fBeam2'), 'Beam2[3564]/F')

    def clearBCIDData(self):

        for i in range(self.nBCID):
            self.rawStruct.fRawLucAND[i] = 0.  # 101
            self.bcidStruct.fMuLucAND[i] = 0.   
            self.rawStruct.fRawLucOR[i] = 0.   # 102 
            self.bcidStruct.fMuLucOR[i] = 0.
            
            self.rawStruct.fRawLucHitOR[i] = 0. # 103
            self.bcidStruct.fMuLucHitOR[i] = 0.
            self.rawStruct.fRawLucHitAND[i] = 0. # 104
            self.bcidStruct.fMuLucHitAND[i] = 0.
            
            self.rawStruct.fRawLucA[i] = 0.    # 105
            self.bcidStruct.fMuLucA[i] = 0.
            self.rawStruct.fRawLucC[i] = 0.    # 106
            self.bcidStruct.fMuLucC[i] = 0.

            # Lucid Fiber channels
            self.rawStruct.fRawLFiAND[i] = 0.  # 111
            self.bcidStruct.fMuLFiAND[i] = 0.   
            self.rawStruct.fRawLFiOR[i] = 0.   # 112 
            self.bcidStruct.fMuLFiOR[i] = 0.
            
            self.rawStruct.fRawLFiHitOR[i] = 0. # 113
            self.bcidStruct.fMuLFiHitOR[i] = 0.
            self.rawStruct.fRawLFiHitAND[i] = 0. # 114
            self.bcidStruct.fMuLFiHitAND[i] = 0.
            
            self.rawStruct.fRawLFiA[i] = 0.    # 115
            self.bcidStruct.fMuLFiA[i] = 0.
            self.rawStruct.fRawLFiC[i] = 0.    # 116
            self.bcidStruct.fMuLFiC[i] = 0.
            
            self.rawStruct.fRawBcmHAND[i] = 0. # 201
            self.bcidStruct.fMuBcmHAND[i] = 0.
            self.rawStruct.fRawBcmHOR[i] = 0.  # 202
            self.bcidStruct.fMuBcmHOR[i] = 0.
            self.rawStruct.fRawBcmHXORA[i] = 0.  # 203
            self.bcidStruct.fMuBcmHXORA[i] = 0.
            self.rawStruct.fRawBcmHXORC[i] = 0.  # 204
            self.bcidStruct.fMuBcmHXORC[i] = 0.
            self.rawStruct.fRawBcmHORA[i] = 0.  # 205
            self.bcidStruct.fMuBcmHORA[i] = 0.
            self.rawStruct.fRawBcmHORC[i] = 0.  # 206
            self.bcidStruct.fMuBcmHORC[i] = 0.
            self.rawStruct.fRawBcmHAND25[i] = 0.  # 207
            self.bcidStruct.fMuBcmHAND25[i] = 0.
            
            self.rawStruct.fRawBcmVAND[i] = 0. # 211
            self.bcidStruct.fMuBcmVAND[i] = 0.
            self.rawStruct.fRawBcmVOR[i] = 0.  # 212
            self.bcidStruct.fMuBcmVOR[i] = 0.
            self.rawStruct.fRawBcmVXORA[i] = 0.  # 213
            self.bcidStruct.fMuBcmVXORA[i] = 0.
            self.rawStruct.fRawBcmVXORC[i] = 0.  # 214
            self.bcidStruct.fMuBcmVXORC[i] = 0.
            self.rawStruct.fRawBcmVORA[i] = 0.  # 215
            self.bcidStruct.fMuBcmVORA[i] = 0.
            self.rawStruct.fRawBcmVORC[i] = 0.  # 216
            self.bcidStruct.fMuBcmVORC[i] = 0.
            self.rawStruct.fRawBcmVAND25[i] = 0.  # 217
            self.bcidStruct.fMuBcmVAND25[i] = 0.

            self.rawStruct.fRawBcmTAND[i] = 0. # 221
            self.bcidStruct.fMuBcmTAND[i] = 0.
            self.rawStruct.fRawBcmTOR[i] = 0.  # 222
            self.bcidStruct.fMuBcmTOR[i] = 0.
            self.rawStruct.fRawBcmTORC[i] = 0.  # 226
            self.bcidStruct.fMuBcmTORC[i] = 0.
            
            self.rawStruct.fRawZdcAND[i] = 0. # 411
            self.bcidStruct.fMuZdcAND[i] = 0.
            self.rawStruct.fRawZdcOR[i] = 0. # 412
            self.bcidStruct.fMuZdcOR[i] = 0.
            self.rawStruct.fRawZdcORA[i] = 0. # 413
            self.bcidStruct.fMuZdcORA[i] = 0.
            self.rawStruct.fRawZdcORC[i] = 0. # 414
            self.bcidStruct.fMuZdcORC[i] = 0.

            self.rawStruct.fRawBcm11HAND[i] = 0. # 1001
            self.bcidStruct.fMuBcm11HAND[i] = 0.
            self.rawStruct.fRawBcm11HOR[i] = 0.  # 1002
            self.bcidStruct.fMuBcm11HOR[i] = 0.
            self.rawStruct.fRawBcm11HXORC[i] = 0.  # 1004
            self.bcidStruct.fMuBcm11HXORC[i] = 0.

            self.rawStruct.fRawBcm11VAND[i] = 0. # 1011
            self.bcidStruct.fMuBcm11VAND[i] = 0.
            self.rawStruct.fRawBcm11VOR[i] = 0.  # 1012
            self.bcidStruct.fMuBcm11VOR[i] = 0.
            self.rawStruct.fRawBcm11VXORC[i] = 0.  # 1014
            self.bcidStruct.fMuBcm11VXORC[i] = 0.

            self.bcidStruct.fBeam1[i] = 0.
            self.bcidStruct.fBeam2[i] = 0.

    def fillBCIDData(self, chan, bcid, lraw, muval):
        
        if chan == 101:
            self.rawStruct.fRawLucAND[bcid] = lraw
            self.bcidStruct.fMuLucAND[bcid] = muval

        elif chan == 102:
            self.rawStruct.fRawLucOR[bcid] = lraw
            self.bcidStruct.fMuLucOR[bcid] = muval
                            
        elif chan == 103:
            self.rawStruct.fRawLucHitOR[bcid] = lraw
            self.bcidStruct.fMuLucHitOR[bcid] = muval
                            
        elif chan == 104:
            self.rawStruct.fRawLucHitAND[bcid] = lraw
            self.bcidStruct.fMuLucHitAND[bcid] = muval

        elif chan == 105:
            self.rawStruct.fRawLucA[bcid] = lraw
            self.bcidStruct.fMuLucA[bcid] = muval
                            
        elif chan == 106:
            self.rawStruct.fRawLucC[bcid] = lraw
            self.bcidStruct.fMuLucC[bcid] = muval
                             
        elif chan == 111:
            self.rawStruct.fRawLFiAND[bcid] = lraw
            self.bcidStruct.fMuLFiAND[bcid] = muval

        elif chan == 112:
            self.rawStruct.fRawLFiOR[bcid] = lraw
            self.bcidStruct.fMuLFiOR[bcid] = muval
                            
        elif chan == 113:
            self.rawStruct.fRawLFiHitOR[bcid] = lraw
            self.bcidStruct.fMuLFiHitOR[bcid] = muval
                            
        elif chan == 114:
            self.rawStruct.fRawLFiHitAND[bcid] = lraw
            self.bcidStruct.fMuLFiHitAND[bcid] = muval

        elif chan == 115:
            self.rawStruct.fRawLFiA[bcid] = lraw
            self.bcidStruct.fMuLFiA[bcid] = muval
                            
        elif chan == 116:
            self.rawStruct.fRawLFiC[bcid] = lraw
            self.bcidStruct.fMuLFiC[bcid] = muval
                            
        elif chan == 201:
            self.rawStruct.fRawBcmHOR[bcid] = lraw
            self.bcidStruct.fMuBcmHOR[bcid] = muval
                            
        elif chan == 202:
            self.rawStruct.fRawBcmHAND[bcid] = lraw
            self.bcidStruct.fMuBcmHAND[bcid] = muval

        elif chan == 203:
            self.rawStruct.fRawBcmHXORA[bcid] = lraw
            self.bcidStruct.fMuBcmHXORA[bcid] = muval
            
        elif chan == 204:
            self.rawStruct.fRawBcmHXORC[bcid] = lraw
            self.bcidStruct.fMuBcmHXORC[bcid] = muval
            
        elif chan == 205:
            self.rawStruct.fRawBcmHORA[bcid] = lraw
            self.bcidStruct.fMuBcmHORA[bcid] = muval
            
        elif chan == 206:
            self.rawStruct.fRawBcmHORC[bcid] = lraw
            self.bcidStruct.fMuBcmHORC[bcid] = muval
            
        elif chan == 207:
            self.rawStruct.fRawBcmHAND25[bcid] = lraw
            self.bcidStruct.fMuBcmHAND25[bcid] = muval
            
        elif chan == 211:
            self.rawStruct.fRawBcmVOR[bcid] = lraw
            self.bcidStruct.fMuBcmVOR[bcid] = muval
                            
        elif chan == 212:
            self.rawStruct.fRawBcmVAND[bcid] = lraw
            self.bcidStruct.fMuBcmVAND[bcid] = muval

        elif chan == 213:
            self.rawStruct.fRawBcmVXORA[bcid] = lraw
            self.bcidStruct.fMuBcmVXORA[bcid] = muval

        elif chan == 214:
            self.rawStruct.fRawBcmVXORC[bcid] = lraw
            self.bcidStruct.fMuBcmVXORC[bcid] = muval

        elif chan == 215:
            self.rawStruct.fRawBcmVORA[bcid] = lraw
            self.bcidStruct.fMuBcmVORA[bcid] = muval

        elif chan == 216:
            self.rawStruct.fRawBcmVORC[bcid] = lraw
            self.bcidStruct.fMuBcmVORC[bcid] = muval

        elif chan == 217:
            self.rawStruct.fRawBcmVAND25[bcid] = lraw
            self.bcidStruct.fMuBcmVAND25[bcid] = muval
            
        elif chan == 221:
            self.rawStruct.fRawBcmTOR[bcid] = lraw
            self.bcidStruct.fMuBcmTOR[bcid] = muval
                            
        elif chan == 222:
            self.rawStruct.fRawBcmTAND[bcid] = lraw
            self.bcidStruct.fMuBcmTAND[bcid] = muval

        elif chan == 226:
            self.rawStruct.fRawBcmTORC[bcid] = lraw
            self.bcidStruct.fMuBcmTORC[bcid] = muval

        elif chan == 411:
            self.rawStruct.fRawZdcAND[bcid] = lraw
            self.bcidStruct.fMuZdcAND[bcid] = muval
            
        elif chan == 412:
            self.rawStruct.fRawZdcOR[bcid] = lraw
            self.bcidStruct.fMuZdcOR[bcid] = muval
            
        elif chan == 413:
            self.rawStruct.fRawZdcORA[bcid] = lraw
            self.bcidStruct.fMuZdcORA[bcid] = muval
            
        elif chan == 414:
            self.rawStruct.fRawZdcORC[bcid] = lraw
            self.bcidStruct.fMuZdcORC[bcid] = muval

        elif chan == 1001:
            self.rawStruct.fRawBcm11HOR[bcid] = lraw
            self.bcidStruct.fMuBcm11HOR[bcid] = muval
                            
        elif chan == 1002:
            self.rawStruct.fRawBcm11HAND[bcid] = lraw
            self.bcidStruct.fMuBcm11HAND[bcid] = muval

        elif chan == 1004:
            self.rawStruct.fRawBcm11HXORC[bcid] = lraw
            self.bcidStruct.fMuBcm11HXORC[bcid] = muval

        elif chan == 1011:
            self.rawStruct.fRawBcm11VOR[bcid] = lraw
            self.bcidStruct.fMuBcm11VOR[bcid] = muval
                            
        elif chan == 1012:
            self.rawStruct.fRawBcm11VAND[bcid] = lraw
            self.bcidStruct.fMuBcm11VAND[bcid] = muval

        elif chan == 1014:
            self.rawStruct.fRawBcm11VXORC[bcid] = lraw
            self.bcidStruct.fMuBcm11VXORC[bcid] = muval
            
        else:
            print 'Channel %d unknown for bcid %d!' % (chan, bcid)
        
    # Pass an IObject object for a single BUNCHDATA entry, and a MaskObj instance
    def fillCurrentData(self, obj, mask):
        self.lbDataStruct.fAvgBeam1 = obj.payload()['B1BunchAverage']
        self.lbDataStruct.fAvgBeam2 = obj.payload()['B2BunchAverage']
        blob1 = obj.payload()['B1BunchIntensities']
        blob2 = obj.payload()['B2BunchIntensities']

        if blob1.size() != 0:
            bcidVec, bcidVal = unpackBCIDValues(blob1, mask.beam1, self.lbDataStruct.fAvgBeam1)

            for i in range(len(bcidVal)):

                bcid = bcidVec[i]

                # Protect against weird values
                if bcid >= self.nBCID:
                    print 'BCID %d found in beam 1 current data!' % bcid
                    continue

                self.bcidStruct.fBeam1[bcid] = bcidVal[i]
        
        if blob2.size() != 0:
            bcidVec, bcidVal = unpackBCIDValues(blob2, mask.beam2, self.lbDataStruct.fAvgBeam2)

            for i in range(len(bcidVal)):

                bcid = bcidVec[i]

                # Protect against weird values
                if bcid >= self.nBCID:
                    print 'BCID %d found in beam 2 current data!' % bcid
                    continue

                self.bcidStruct.fBeam2[bcid] = bcidVal[i]
                
# Executed from the command line
if __name__ == '__main__':
    mhm = CalibNtupleMaker()
    mhm.execute()

                
