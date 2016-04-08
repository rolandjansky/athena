#!/usr/bin/env python
#
# vdMScanMaker.py
#
# Main application script to generate ntuples of vdM scan data
# 27 May 2014 - Adapted to produce identical output as calibNtupleMaker12 for Benedetto
#
import os
import sys
import shutil
import array
import time, calendar

from optparse import OptionParser

# Utility to unpack BCID blobs
from AtlDataSummary.AtlDataSumLumiData import LumiBCIDData
from CoolLumiUtilities.LumiBlobConversion import unpackBCIDMask, unpackBCIDValues

# General COOL reader object
from CoolLumiUtilities.CoolDataReader import CoolDataReader

# Get our global DB handler object
from CoolLumiUtilities.LumiDBHandler import LumiDBHandler

from PyCool import cool

from ROOT import TFile, TTree, gROOT, AddressOf
        
# Simple utility for converting time strings
def timeVal(val):
    "Convert argument to time in seconds, treating as a literal or date"
    try:
        a=int(val)
        return a
    
    except ValueError:
        try:
            ts=time.strptime(val,'%Y-%m-%d:%H:%M:%S/%Z')
            return int(calendar.timegm(ts))*1000000000L
        
        # Try again with UTC attached
        except ValueError:
            try:
                ts=time.strptime(val+'/UTC', '%Y-%m-%d:%H:%M:%S/%Z')
                return int(calendar.timegm(ts))*1000000000L
            
            except ValueError,e:
                print "ERROR in time specification:",val,"- use e.g. 2007-05-25:14:01:00/UTC"
                return None

def timeString(iovkey):
    # Convert the IOVkey (63 bit) to a string representing time stamp
    if (iovkey == cool.ValidityKeyMin):
        return 'ValidityKeyMin'
    elif (iovkey == cool.ValidityKeyMax):
        return 'ValidityKeyMax'
    else:
        stime=int(round(iovkey/1000000000L))
        return time.strftime('%Y-%m-%d:%H:%M:%S/UTC', time.gmtime(stime))
    

class OflLumiMaker:

    def __init__(self):

        # Control output level
        self.verbose = False

        # Output root file name
        self.fileName = 'scan.root'

        # Input file with turn information (from Nitesh)
        self.turnFile = None

        # Location of scan data 
        self.tdaqDB = 'COOLONL_TDAQ/COMP200'
        self.tdaqMonpDB = 'COOLONL_TDAQ/MONP200'

        self.monp = False # Use MONP or COMP DB
        
        self.scanFolder = '/TDAQ/OLC/LHC/SCANDATA'     # vdM Scan parameter data
        self.beamFolder = '/TDAQ/OLC/LHC/BEAMPOSITION'
        self.fillFolder = '/TDAQ/OLC/LHC/FILLPARAMS'
        self.lbdataFolder = '/TDAQ/OLC/LHC/LBDATA'
        self.bunchFolder = '/TDAQ/OLC/LHC/BUNCHDATA'
        self.bunchLumiFolder = '/TDAQ/OLC/BUNCHLUMIS' 
        self.lumiFolder = '/TDAQ/OLC/LUMINOSITY'

        # Calendar time range to look for vdM scan data
        self.startTime = '2010-10-01:07:00:00/UTC'
        self.endTime ='2010-10-01:14:00:00/UTC'

        # Time as COOL IOV
        self.startTimeIOV = None
        self.endTimeIOV = None
        
        self.startLBIOV = None
        self.endLBIOV = None
        
        # Pointers to CoolDataReader return objects
        self.lhcData = None
        self.lumiData = None
        self.scanData = None
        self.lbdataData = None
        self.bunchdataData = None

        self.ions = False

        # Object to store BCID information
        self.maskObj = BCIDMask()        

        # Object to store the bunch group definition
        self.bgObj = BunchGroup()

        # Channels to store
        self.lumiChanList = [101,102,103,104,105,106,201,202,206,207,211,212,216,217,221,222,226,1001,1002,1004,1011,1012,1014]

    # Called in command-line mode
    def execute(self):

        # Handle command-line switches
        self.parseOpts()

        # Find the scan data
        self.findScanData()

        # Write the data to ntuple
        #nt = ScanNtupleHandler()
        #nt.fileName = self.fileName
        #nt.ions = self.ions
        
        #nt.open()
        #nt.init()
        #nt.fill(self) # Must pass self reference to get access to data
        #nt.close()

        nt = NtupleHandler()
        nt.chan2011 = False
        nt.chan2012 = True
        nt.fibers = False
        nt.fileName = self.fileName

        nt.open()
        nt.init()

        nfilled = 0

        # Sort raw data and save by [iov][algo]
        rawDict = dict()
        for obj in self.bunchLumi.data:

            channel = obj.channelId()
            iov = obj.since()

            if iov not in rawDict:
                rawDict[iov] = dict()
            rawDict[iov][channel] = obj

        # PMT current data
        pmtDict = dict()
        fibDict = dict() # Fiber currents
        for obj in self.pmtReader.data:
            if obj.channelId() == 0:
                pmtDict[obj.since()] = obj
            elif obj.channelId() == 1:
                fibDict[obj.since()] = obj
            else:
                print 'Found channel %d in pmtReader!' % obj.channel()

        # Bunch current data                
        currDict = dict()
        for obj in self.bunchData.data:
            currDict[obj.since()] = obj

        # Loop over scandata entries
        for plbobj in self.scanData.data:

            iov = plbobj.since()
            iovString = timeString(iov)

            runlb = plbobj.payload()['RunLB']
            run = plbobj.payload()['RunLB'] >> 32
            lb = plbobj.payload()['RunLB'] & 0xFFFFFFFF
            dtime = (plbobj.until()-plbobj.since())/1E9

            print '%s (%6d/%3d)  t=%5.2fs  Acq:%5.2f d=%6.3fmm IP:%2d Plane:%2d' % (iovString, run, lb, dtime, plbobj.payload()['AcquisitionFlag'], plbobj.payload()['NominalSeparation'], plbobj.payload()['ScanningIP'], plbobj.payload()['ScanInPlane']),

            # Check if beams are moving at IP1
            if run==0 and plbobj.payload()['ScanningIP'] == 1 and plbobj.payload()['AcquisitionFlag'] < 1.:
                print " --> moving"
                continue
            print

            nt.fillLBData(plbobj)
            nt.lbDataStruct.fStable = True # Ensure true

            if iov not in rawDict:
                print "Can't find time %s in raw lumi!" % iovString
                continue

            nt.clearBCIDData()

            # Make sure BCID mask is valid
            if not self.updateBCIDMask(iov):
                print "Couldn't find valid BCID mask data for IOV %s!" % iov
                continue

            # Make sure BunchGroup is valid
            #if run > 0 and not self.updateBunchGroup(runlb):
            #    print "Couldn't find valid BunchGroup data for %s (%d/%d)!" % (iov, run, lb)                

            if iov in pmtDict:
                nt.lbDataStruct.fPmtA = pmtDict[iov].payload()['CurrentSideA']
                nt.lbDataStruct.fPmtC = pmtDict[iov].payload()['CurrentSideC']
                # print 'Found Run %d LB %d PMTA %f PMTC %f' % (run, lb, nt.lbDataStruct.fPmtA, nt.lbDataStruct.fPmtC)

            if iov in fibDict:
                nt.lbDataStruct.fFibA = fibDict[iov].payload()['CurrentSideA']
                nt.lbDataStruct.fFibC = fibDict[iov].payload()['CurrentSideC']
                # print 'Found Run %d LB %d FIBA %f FIBC %f' % (run, lb, nt.lbDataStruct.fFibA, nt.lbDataStruct.fFibC)

            if iov in currDict:
                nt.fillCurrentData(currDict[iov], self.maskObj)

            for chan in self.lumiChanList:

                if chan not in rawDict[iov]:
                    print "Can't find channel", chan, "in", iovString
                    continue

                rawobj = rawDict[iov][chan]

                if chan != rawobj.channelId():
                    print 'Channel', chan, '!=', rawobj.channelId(), '!'
                    continue

                # Get raw lumi
                normValue = rawobj.payload()['AverageRawInstLum']
                blobValue = rawobj.payload()['BunchRawInstLum']
                bcidVec, rawLumi = unpackBCIDValues(blobValue, self.maskObj.coll, normValue)

                # dict to hold BCID lumi keyed by BCID
                for i in range(len(rawLumi)):

                    # Check if this is in our bunch group (only really need to fill this once)
                    bcid = bcidVec[i]

                    # Protect against any weird values
                    if bcid >= nt.nBCID:
                        print 'BCID %d found >= %d!' % (bcid, nt.nBCID)
                        continue

                    #if bcid in self.bgObj.bg:
                    #    nt.bcidStruct.fStatus[bcid] = 1
                    #else:
                    #    nt.bcidStruct.fStatus[bcid] = 0

                    # Now need to save bcid, mu, and calibLumi 
                    lraw = rawLumi[i]
                    muval = 0. # Uncalibrated
                    nt.fillBCIDData(chan, bcid, lraw, muval)

                # End of loop over BCIDs

            # End of loop over channels
            nt.tree.Fill()

        # End of loop over IOVs
        nt.close()

        
    def parseOpts(self):

        parser = OptionParser(usage="usage: %prog [options]", add_help_option=False)

        parser.add_option("-?", "--usage", action="store_true", default=False, dest="help",
                          help="show this help message and exit")
        
        parser.add_option("-v", "--verbose",
                     action="store_true", default=self.verbose, dest="verbose",
                     help="turn on verbose output")

        parser.add_option("-o", "--output",
                          dest="outfile", metavar="FILE", default=self.fileName,
                          help="specify output ROOT file name - default: "+self.fileName)

        parser.add_option("--startTime",
                          dest="starttime", metavar="TIME",
                          help="set starting time (YYYY-MM-DD:HH:MM:SS)")

        parser.add_option("--endTime",
                          dest="endtime", metavar="TIME",
                          help="set ending time (YYYY-MM-DD:HH:MM:SS)")

        parser.add_option("--ions",
                          dest="ions", default=self.ions, action="store_true",
                          help="Use Heavy Ion variable list (not used currently!)")
        
        parser.add_option("--monp",
                          dest="monp", default=self.monp, action="store_true",
                          help="Use MONP200 rather than COMP200 DB for per-bunch lumi - default: "+str(self.monp))
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.verbose = options.verbose
        self.ions = options.ions
        self.monp = options.monp
         
        # Parse times
        if options.starttime != None:
            self.startTime = options.starttime
        if options.endtime != None:    
            self.endTime = options.endtime

        self.fileName = options.outfile

        
    def findScanData(self):
        print 'vdMScanMaker.findScanData() called'

        # Based on the (text) starting and ending times, find the available scan entries in COOL

        # First, convert time strings to COOL IOV times
        self.startTimeIOV = timeVal(self.startTime)
        if self.startTimeIOV == None:
            print 'OflLumiMaker.findScanData - Error converting start time', self.startTime,'!'
            sys.exit()
            
        self.endTimeIOV = timeVal(self.endTime)
        if self.endTimeIOV == None:
            print 'OflLumiMaker.findScanData - Error converting end time', self.endTime,'!'
            sys.exit()
            
        # Load the scan data
        self.scanData = CoolDataReader(self.tdaqDB, self.scanFolder)
        self.scanData.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        if not self.scanData.readData():
            print 'OflLumiMaker.findScanData - No scan data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()

        # for obj in self.scanData.data:
        #     run = obj.payload()['RunLB'] >> 32
        #     lb = obj.payload()['RunLB'] & 0xFFFFFFFF
        #     print '%s (%6d/%3d)  t=%5.2fs  Acq:%5.2f d=%6.3fmm IP:%2d Plane:%2d' % (timeString(obj.since()), run, lb, (obj.until()-obj.since())/1E9, obj.payload()['AcquisitionFlag'], obj.payload()['NominalSeparation'], obj.payload()['ScanningIP'], obj.payload()['ScanInPlane'])

        # Load the beam positions
        # self.beamData = CoolDataReader(self.tdaqDB, self.beamFolder)
        # self.beamData.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        # if not self.beamData.readData():
        #     print 'OflLumiMaker.findScanData - No beam separation data found in range', self.startTime, 'to', self.endTime,'!'
        #     sys.exit()
            
        # if self.verbose:
        #     for obj in self.beamData.data:
        #         run = obj.payload()['RunLB'] >> 32
        #         lb = obj.payload()['RunLB'] & 0xFFFFFFFF
        #         print run, lb, obj.payload()['B1_PositionAtIP_H'], obj.payload()['B1_PositionAtIP_V'], obj.payload()['B2_PositionAtIP_H'], obj.payload()['B2_PositionAtIP_V']
                
        # Load the fill parameters
        self.fillData = CoolDataReader(self.tdaqDB, self.fillFolder)
        self.fillData.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        if not self.fillData.readData():
            print 'OflLumiMaker.findScanData - No fill parameters data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()
            
        if self.verbose:
            for obj in self.fillData.data:
                print obj.payload()['Beam1Bunches'], obj.payload()['Beam2Bunches'], obj.payload()['LuminousBunches']


        # Load the lbdata parameters
        self.lbdataData = CoolDataReader(self.tdaqDB, self.lbdataFolder)
        self.lbdataData.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        if not self.lbdataData.readData():
            print 'OflLumiMaker.findScanData - No LBDATA data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()
            
        if self.verbose:
            for obj in self.lbdataData.data:
                print 'LBDATA', obj.channelId(), obj.payload()['Beam1Intensity'], obj.payload()['Beam2Intensity']

        # Load the BUNCHDATA parameters
        if self.monp:
            self.bunchData = CoolDataReader(self.tdaqMonpDB, self.bunchFolder)
        else:
            self.bunchData = CoolDataReader(self.tdaqDB, self.bunchFolder)

        self.bunchData.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        self.bunchData.setChannelId(1) # 0 = BPTX, 1 = Fast BCT

        if not self.bunchData.readData():
            print 'OflLumiMaker.findScanData - No BUNCHDATA data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()
            
        if self.verbose:
            for obj in self.bunchData.data:
                print 'BUNCHDATA', obj.channelId(), obj.payload()['B1BunchAverage'], obj.payload()['B2BunchAverage']

        # Load the BUNCHLUMI parameters
        if self.monp:
            self.bunchLumi = CoolDataReader(self.tdaqMonpDB, self.bunchLumiFolder)
        else:
            self.bunchLumi = CoolDataReader(self.tdaqDB, self.bunchLumiFolder)

        self.bunchLumi.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        if not self.bunchLumi.readData():
            print 'OflLumiMaker.findScanData - No BUNCHLUMIS data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()
            
        if self.verbose:
            for obj in self.bunchLumi.data:
                print 'BUNCHLUMI', obj.channelId(), obj.payload()['AverageRawInstLum']

        # Load the luminosity data
        self.lumiData = CoolDataReader(self.tdaqDB, self.lumiFolder)
        self.lumiData.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        if not self.lumiData.readData():
            print 'OflLumiMaker.findScanData - No LUMINOSITY data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()

        # Load the Lucid currents
        self.pmtReader = CoolDataReader('COOLONL_TDAQ/COMP200', '/TDAQ/OLC/LUCIDCURRENTS')
        self.pmtReader.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        self.pmtReader.setChannel([0, 1]) # Load total PMT and fiber currents
        if not self.pmtReader.readData():
            print 'No PMT current data found in range', self.startTime, 'to', self.endTime, '!'

        if self.verbose:
            for obj in self.pmtReader.data:
                print 'LUCIDCURRENTS', obj.channelId(), obj.payload()['CurrentSideA'], obj.payload()['CurrentSideC']

    def updateBCIDMask(self, startTime):

        if not self.maskObj.isValid(startTime):

            self.maskObj.clearValidity()

            # Find the proper mask object
            maskData = None
            for mask in self.fillData.data:
                if not mask.since() <= startTime < mask.until(): continue
                self.maskObj.setMask(mask)
                break

            if not self.maskObj.isValid(startTime):
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
    olm = OflLumiMaker()
    olm.execute()

                
