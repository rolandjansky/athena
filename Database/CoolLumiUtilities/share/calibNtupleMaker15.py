#!/usr/bin/env python
#
# calibNtupleMaker.py
#
# Produce an ntuple of per-bcid luminosities using the raw lumi data
# 23 May 2011 Modified to write out all BCID
#
# Updated for use in 2012.  Removed historical stuff from 2011
# Updated again for 2015, don't bother calibrating anything now
#
# Eric Torrence - 27 April 2011
#
import sys
import array
import string

from optparse import OptionParser

# Get our global DB handler object
from CoolLumiUtilities.LumiDBHandler import LumiDBHandler
from CoolLumiUtilities.CoolDataReader import CoolDataReader

from CoolLumiUtilities.LumiBlobConversion import unpackBCIDMask, unpackBCIDValues
from CoolLumiUtilities.LumiBlobConversion import unpackLiveFraction, unpackBunchGroup

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import forceOpen,ensureFolder,athenaDesc

from ROOT import TFile, TH1D, TTree, gROOT, AddressOf

class CalibNtupleMaker:

    def __init__(self):

        # Control output level
        self.verbose = False

        # Luminosity Channels
        self.lumiChanList = []

        # Luminosity Channel Map
        self.lumiChanNames = dict()

        self.lumiMapFile = 'defaultChannels.txt'

        # Stable only
        self.stableOnly = False

        # Write output ntuple
        self.ntuple = True

        # Write extra current information
        self.writeExtraCurrents = True

        # List of (integer) run numbers specified on the command line
        self.runList = []

        # Dict of (lblo, lbhi) pairs giving extent of StableBeams
        self.stableDict = dict()

        # Utlity routine for handling COOL connections
        self.dbHandler = LumiDBHandler()
        
        # Data readers
        self.maskReader = None
        self.lumiReader = None
        self.caliReader = None
        self.lblbReader = None
        self.currReader = None
        self.lhcReader  = None
        self.bgReader = None
        self.pmtReader = None
        self.lbdataReader = None
        
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

        # Fill channel list
        self.fillChannelList()

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
                nt.chanMap = self.lumiChanNames

                nt.writeExtraCurrents = self.writeExtraCurrents
                    
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

            # LHC Current objects keyed by [runlb]
            currDict = dict()
            for obj in self.currReader.data:
                runlb = obj.payload()['RunLB']
                if runlb not in currDict:
                    currDict[runlb] = dict()
                currDict[runlb][obj.channelId()] = obj
                
            #if self.writeExtraCurrents:
            lbdataDict = dict()
            for obj in self.lbdataReader.data:
                runlb = obj.payload()['RunLB']
                if runlb not in lbdataDict:
                    lbdataDict[runlb] = dict()
                lbdataDict[runlb][obj.channelId()] = obj

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

                        # These change slowly, so checking them every run/lb is OK
                        # Make sure BCID mask is valid
                        if not self.updateBCIDMask(startTime):
                            print "Couldn't find valid BCID mask data for Run %d LB %d!" % (run, lb)
                            continue

                        # Do this here so the BCID mask is up to date
                        if self.ntuple:
                            if runlb in currDict:
                                if 1 in currDict[runlb]:
                                    nt.fillCurrentData(currDict[runlb], self.maskObj, 1)
                                if 0 in currDict[runlb] and self.writeExtraCurrents:
                                    nt.fillCurrentData(currDict[runlb], self.maskObj, 0)

                            if runlb in lbdataDict:
                                nt.fillMoreCurrentData(lbdataDict[runlb])
                                
                    # Now we want to start extracting bunch-by-bunch information

                    # Get raw lumi
                    normValue = obj.payload()['AverageRawInstLum']
                    blobValue = obj.payload()['BunchRawInstLum']
                    bcidVec, rawLumi = unpackBCIDValues(blobValue)

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
                            nt.bcidArray['Status'][bcid] = 1
                        else:
                            nt.bcidArray['Status'][bcid] = 0

                        # Now need to save bcid, mu, and calibLumi 
                        lraw = rawLumi[i]

                        nt.fillBCIDData(chan, bcid, lraw)
                        
                    # End loop over BCIDs
                    
                # End of loop over channels
                nt.tree.Fill()

            # End of loop over LBs
            nt.close()

        # End of loop over runs                
                
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
        self.loadBCIDLumi(iov)
        self.loadBCIDCurrents(iov)
        self.loadOtherCurrents(iov)

    def loadLBLB(self, run):
        if self.verbose: print 'Loading LBLB data'

        # Instantiate new COOL data reader if not already done
        if self.lblbReader == None:
            self.lblbReader = CoolDataReader('COOLONL_TRIGGER/CONDBR2', '/TRIGGER/LUMI/LBLB')

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
            self.bgReader = CoolDataReader('COOLONL_TRIGGER/CONDBR2', '/TRIGGER/LVL1/BunchGroupContent')

        self.bgReader.setIOVRangeFromRun(run)
        self.bgReader.readData()

        if self.verbose:
            print 'Read %d bunch group records' % len(self.bgReader.data)
            
    def loadBCIDMask(self, iov):
        if self.verbose: print 'Loading BCID masks'

        # Instantiate new COOL data reader if not already done
        if self.maskReader == None:
            self.maskReader = CoolDataReader('COOLONL_TDAQ/CONDBR2', '/TDAQ/OLC/LHC/FILLPARAMS')
            
        self.maskReader.setIOVRange(iov[0], iov[1])
        self.maskReader.readData()

        if self.verbose:
            print 'Read %d BCID Mask records' % len(self.maskReader.data)

    def loadBCIDLumi(self, iov):
        if self.verbose: print 'Loading BCID luminosity values'

        # Instantiate new COOL data reader if not already done
        if self.lumiReader == None:
            # Switch at start of September
            self.lumiReader = CoolDataReader('COOLONL_TDAQ/CONDBR2', '/TDAQ/OLC/BUNCHLUMIS')

        #self.lumiReader.verbose = True
        self.lumiReader.setIOVRange(iov[0], iov[1])
        self.lumiReader.setChannel(self.lumiChanList)
        self.lumiReader.readData()
        #self.lumiReader.verbose = False

        if self.verbose:
            print 'Read %d Lumi records' % len(self.lumiReader.data)
            

    # Bunch currents
    def loadBCIDCurrents(self, iov):
        if self.verbose: print 'Loading Bunch Current information'

        if self.currReader == None:
            # self.currReader = CoolDataReader('COOLONL_TDAQ/MONP200', '/TDAQ/OLC/LHC/BUNCHDATA')
            self.currReader = CoolDataReader('COOLONL_TDAQ/CONDBR2', '/TDAQ/OLC/LHC/BUNCHDATA')

        self.currReader.setIOVRange(iov[0], iov[1])
        if self.writeExtraCurrents:
            self.currReader.setChannel([0,1]) # 0 = BPTX, 1 = Fast BCT
        else:
            self.currReader.setChannelId(1) # 0 = BPTX, 1 = Fast BCT
        self.currReader.readData()

        if self.verbose:
            print 'Read %d Current records' % len(self.currReader.data)

    def loadOtherCurrents(self, iov):        
        if self.verbose: print 'Loading LBDATA Bunch Current information'

        if self.lbdataReader == None:
            self.lbdataReader = CoolDataReader('COOLONL_TDAQ/CONDBR2', '/TDAQ/OLC/LHC/LBDATA')

        self.lbdataReader.setIOVRange(iov[0], iov[1])
        self.lbdataReader.setChannel([0,1,2,3]) # 0 = BPTX, 1 = Fast BCT
        self.lbdataReader.readData()

        if self.verbose:
            print 'Read %d LBDATA Current records' % len(self.lbdataReader.data)

    # Information about stable beams
    def loadLHCData(self, iov):
        if self.verbose: print 'Loading LHC information'

        # Instantiate new COOL data reader if not already done
        if self.lhcReader == None:
            self.lhcReader = CoolDataReader('COOLOFL_DCS/CONDBR2', '/LHC/DCS/FILLSTATE')
            
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

        parser.add_option("--noNtuple",
                          action="store_false", default=self.ntuple, dest='ntuple',
                          help="Don't store output ntuple (default: Do)")

        parser.add_option("-o", "--outputDir",
                          dest="outdir", metavar="DIR", default=self.outdir,
                          help='directory for output ntuple (default: %s)' % self.outdir)

        parser.add_option("--noStable",
                          action="store_false", default=self.stableOnly, dest="stable",
                          help="turn off stable beams requirements (default: stable only)")

        parser.add_option("--channelList",
                          dest='chanlist', metavar="FILE", default=self.lumiMapFile,
                          help='file to read channel list from (default: %s)' % self.lumiMapFile)

        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.verbose = options.verbose
        self.outdir = options.outdir
        self.stableOnly = options.stable
        self.lumiMapFile = options.chanlist

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

    def fillChannelList(self):

        print 'Reading channel list from', self.lumiMapFile

        # Make sure these are empty
        self.lumiChanList = []
        self.lumiChanNames = dict()

        f = open(self.lumiMapFile)

        for line in f.readlines():

            line = string.lstrip(line)
            # Skip obvious comments
            if len(line) == 0: continue
            if line[0] == "!": continue
            if line[0] == "#": continue

            # Now parse
            tokens = line.split()
            chanID = int(tokens[0])
            chanName = tokens[1]
            print 'Found Channel %d: %s' % (chanID, chanName)
            self.lumiChanList.append(chanID)
            self.lumiChanNames[chanID] = chanName

        # End of loop over channels
        f.close()

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

        # Map of channel names keyed by (int) channel number
        self.chanMap = dict()

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
        
        self.tree.Branch('BPTXBeam1All', AddressOf(self.lbDataStruct, 'fBPTXBeam1'), 'BPTXBeam1All/F')
        self.tree.Branch('BPTXBeam2All', AddressOf(self.lbDataStruct, 'fBPTXBeam2'), 'BPTXBeam2All/F')
        self.tree.Branch('BCTBeam1All', AddressOf(self.lbDataStruct, 'fBCTBeam1'), 'BCTBeam1All/F')
        self.tree.Branch('BCTBeam2All', AddressOf(self.lbDataStruct, 'fBCTBeam2'), 'BCTBeam2All/F')
        self.tree.Branch('DCCTBeam1All', AddressOf(self.lbDataStruct, 'fDCCTBeam1'), 'DCCTBeam1All/F')
        self.tree.Branch('DCCTBeam2All', AddressOf(self.lbDataStruct, 'fDCCTBeam2'), 'DCCTBeam2All/F')
        self.tree.Branch('DCCT24Beam1All', AddressOf(self.lbDataStruct, 'fDCCT24Beam1'), 'DCCT24Beam1All/F')
        self.tree.Branch('DCCT24Beam2All', AddressOf(self.lbDataStruct, 'fDCCT24Beam2'), 'DCCT24Beam2All/F')

    # Pass an IObject object for a single ntuple entry
    def fillLBData(self, obj):
        self.lbDataStruct.fStartTime = obj.since() # Time in ns
        self.lbDataStruct.fEndTime = obj.until()   # Time in ns
        self.lbDataStruct.fRun = obj.payload()['RunLB'] >> 32
        self.lbDataStruct.fLB = obj.payload()['RunLB']&0xFFFFFFFF
        self.lbDataStruct.fBPTXBeam1 = 0.
        self.lbDataStruct.fBPTXBeam2 = 0.
        self.lbDataStruct.fBCTBeam1 = 0.
        self.lbDataStruct.fBCTBeam2 = 0.
        self.lbDataStruct.fDCCTBeam1 = 0.
        self.lbDataStruct.fDCCTBeam2 = 0.
        self.lbDataStruct.fDCCT24Beam1 = 0.
        self.lbDataStruct.fDCCT24Beam2 = 0.
        
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
                Float_t fBPTXBeam1;\
                Float_t fBPTXBeam2;\
                Float_t fBCTBeam1;\
                Float_t fBCTBeam2;\
                Float_t fDCCTBeam1;\
                Float_t fDCCTBeam2;\
                Float_t fDCCT24Beam1;\
                Float_t fDCCT24Beam2;\
            };"
        
        # Replace sizes if needed
        gROOT.ProcessLine(structStr)
        from ROOT import LBDataStruct
        return LBDataStruct()

    # All BCID-dependent quantities
    def initBCIDData(self):

        # Try this a different way
        self.bcidArray = dict()

        # First get channel numbers from map
        chanList = sorted(self.chanMap.keys())
        for chan in chanList:

            # First create some memory for this
            self.bcidArray[chan] = array.array('f', self.nBCID*[0.])

            # Next, create the branch
            name = self.chanMap[chan]
            format = '%s[%d]/F' % (name, self.nBCID)
            self.tree.Branch(name, self.bcidArray[chan], format)

        # Do a few special extras also
        self.bcidArray['Status'] = array.array('i', self.nBCID*[0])
        self.bcidArray['Beam1'] = array.array('f', self.nBCID*[0.])
        self.bcidArray['Beam2'] = array.array('f', self.nBCID*[0.])
        self.tree.Branch('Status', self.bcidArray['Status'], 'Status[3564]/I')
        self.tree.Branch('Beam1', self.bcidArray['Beam1'], 'Beam1[3564]/F')
        self.tree.Branch('Beam2', self.bcidArray['Beam2'], 'Beam2[3564]/F')

        if self.writeExtraCurrents:
            self.bcidArray['BPTXBeam1'] = array.array('f', self.nBCID*[0.])
            self.bcidArray['BPTXBeam2'] = array.array('f', self.nBCID*[0.])
            self.tree.Branch('BPTXBeam1', self.bcidArray['BPTXBeam1'], 'BPTXBeam1[3564]/F')
            self.tree.Branch('BPTXBeam2', self.bcidArray['BPTXBeam2'], 'BPTXBeam2[3564]/F')


    def clearBCIDData(self):

        # Must use [:] to specify original location of array
        # Without this, the memory location would change.  
        # Hope this is quicker than iterating and assigning each value to zero
        for chan in self.bcidArray:
            if chan == 'Status':
                self.bcidArray[chan][:] = array.array('i', self.nBCID*[0])
            else:
                self.bcidArray[chan][:] = array.array('f', self.nBCID*[0.])


    def fillBCIDData(self, chan, bcid, lraw):

        if chan not in self.chanMap.keys():
            print 'Unknown channel', chan

        # Fill our storage
        self.bcidArray[chan][bcid] = lraw
        
        
    def fillMoreCurrentData(self, obj):

        if 0 in obj:
            self.lbDataStruct.fBPTXBeam1 = obj[0].payload()['Beam1IntensityAll']
            self.lbDataStruct.fBPTXBeam2 = obj[0].payload()['Beam2IntensityAll']

        if 1 in obj:
            self.lbDataStruct.fBCTBeam1 = obj[1].payload()['Beam1IntensityAll']
            self.lbDataStruct.fBCTBeam2 = obj[1].payload()['Beam2IntensityAll']

        if 2 in obj:
            self.lbDataStruct.fDCCTBeam1 = obj[2].payload()['Beam1IntensityAll']
            self.lbDataStruct.fDCCTBeam2 = obj[2].payload()['Beam2IntensityAll']

        if 3 in obj:
            self.lbDataStruct.fDCCT24Beam1 = obj[3].payload()['Beam1IntensityAll']
            self.lbDataStruct.fDCCT24Beam2 = obj[3].payload()['Beam2IntensityAll']

    # Pass an IObject object for a single BUNCHDATA entry, and a MaskObj instance
    def fillCurrentData(self, obj, mask, chan=1):

        blob1 = obj[chan].payload()['B1BunchIntensities']
        blob2 = obj[chan].payload()['B2BunchIntensities']

        if blob1.size() != 0:
            bcidVec, bcidVal = unpackBCIDValues(blob1, mask.beam1, obj[chan].payload()['B1BunchAverage'])

            for i in range(len(bcidVal)):

                bcid = bcidVec[i]

                # Protect against weird values
                if bcid >= self.nBCID:
                    print 'BCID %d found in beam 1 current data!' % bcid
                    continue

                if chan==1:
                    self.bcidArray['Beam1'][bcid] = bcidVal[i]
                elif chan==0:
                    self.bcidArray['BPTXBeam1'][bcid] = bcidVal[i]

        if blob2.size() != 0:
            bcidVec, bcidVal = unpackBCIDValues(blob2, mask.beam2, obj[chan].payload()['B2BunchAverage'])

            for i in range(len(bcidVal)):

                bcid = bcidVec[i]

                # Protect against weird values
                if bcid >= self.nBCID:
                    print 'BCID %d found in beam 2 current data!' % bcid
                    continue

                if chan==1:
                    self.bcidArray['Beam2'][bcid] = bcidVal[i]
                elif chan==0:
                    self.bcidArray['BPTXBeam2'][bcid] = bcidVal[i]

# Executed from the command line
if __name__ == '__main__':
    mhm = CalibNtupleMaker()
    mhm.execute()

                
