# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# ScanNtupleHandler.py
#
# Definition of vdM scan ntuple
#

# Utility to unpack BCID blobs
from AtlDataSummary.AtlDataSumLumiData import LumiBCIDData

from ROOT import TFile, TTree, gROOT, AddressOf
from array import array

class ScanNtupleHandler:

    def __init__(self):

        self.fileName = 'scan.root'
        self.treeName = 'vdMScanData'
        self.file = None
        self.tree = None

        self.bcidData = LumiBCIDData()

        # Master list of tags to give to algorithms
        self.algDict = dict()
        self.algDict[0] = 'pref'
        self.algDict[101] = 'lucidEvtAND'
        self.algDict[102] = 'lucidEvtOR'
        self.algDict[103] = 'lucidHitOR'
        self.algDict[104] = 'lucidHitAND'
        self.algDict[105] = 'lucidEvtA'
        self.algDict[106] = 'lucidEvtC'

        self.algDict[111] = 'fiberEvtAND'
        self.algDict[112] = 'fiberEvtOR'
        self.algDict[113] = 'fiberHitOR'
        self.algDict[114] = 'fiberHitAND'
        self.algDict[115] = 'fiberEvtA'
        self.algDict[116] = 'fiberEvtC'
        
        self.algDict[201] = 'bcmHEvtOR'
        self.algDict[202] = 'bcmHEvtAND'
        # self.algDict[203] = 'bcmXORA'
        # self.algDict[204] = 'bcmHXORC' # Remove in 2012
        self.algDict[206] = 'bcmHORC'
        self.algDict[207] = 'bcmHAND25'
        
        self.algDict[211] = 'bcmVEvtOR'
        self.algDict[212] = 'bcmVEvtAND'
        # self.algDict[214] = 'bcmVXORC' # Remove in 2012
        self.algDict[216] = 'bcmVORC'
        self.algDict[217] = 'bcmVAND25'
        
        self.algDict[221] = 'bcmTEvtOR'
        self.algDict[222] = 'bcmTEvtAND'
        self.algDict[226] = 'bcmTORC'
        
        self.algDict[301] = 'mbtsEvtOR'
        self.algDict[302] = 'mbtsEvtAND'
        # self.algDict[303] = 'mbtsHitOR'

        self.algDict[401] = 'zdcEvtAND'
        self.algDict[402] = 'zdcEvtORA'
        self.algDict[403] = 'zdcEvtORC'

        self.algDict[411] = 'zdclEvtAND'
        self.algDict[412] = 'zdclEvtOR'
        self.algDict[413] = 'zdclEvtORA'
        self.algDict[414] = 'zdclEvtORC'
        
        self.algDict[501] = 'fcalA'
        self.algDict[502] = 'fcalC'
        self.algDict[521] = 'rpc'

        self.algDict[1001] = 'bcm11HEvtOR'
        self.algDict[1002] = 'bcm11HEvtAND'
        self.algDict[1004] = 'bcm11HXORC'

        self.algDict[1011] = 'bcm11VEvtOR'
        self.algDict[1012] = 'bcm11VEvtAND'
        self.algDict[1014] = 'bcm11VXORC'


        # Algorithms with bunch-by-bunch information
        self.bbbAlgDict = dict()
        self.bbbAlgDict[101] = 'lucidEvtAND'
        self.bbbAlgDict[102] = 'lucidEvtOR'
        self.bbbAlgDict[103] = 'lucidHitOR'
        self.bbbAlgDict[104] = 'lucidHitAND'
        self.bbbAlgDict[105] = 'lucidEvtA'
        self.bbbAlgDict[106] = 'lucidEvtC'
        
        self.bbbAlgDict[111] = 'fiberEvtAND'
        self.bbbAlgDict[112] = 'fiberEvtOR'
        self.bbbAlgDict[113] = 'fiberHitOR'
        self.bbbAlgDict[114] = 'fiberHitAND'
        self.bbbAlgDict[115] = 'fiberEvtA'
        self.bbbAlgDict[116] = 'fiberEvtC'
        
        self.bbbAlgDict[201] = 'bcmHEvtOR'
        self.bbbAlgDict[202] = 'bcmHEvtAND'
        # self.bbbAlgDict[203] = 'bcmXORA'
        # self.bbbAlgDict[204] = 'bcmHXORC' # Remove for 2012
        self.bbbAlgDict[206] = 'bcmHORC'
        self.bbbAlgDict[207] = 'bcmHAND25'
        
        self.bbbAlgDict[211] = 'bcmVEvtOR'
        self.bbbAlgDict[212] = 'bcmVEvtAND'
        # self.bbbAlgDict[214] = 'bcmVXORC' # Remove for 2012
        self.bbbAlgDict[216] = 'bcmVORC'
        self.bbbAlgDict[217] = 'bcmVAND25'

        self.bbbAlgDict[221] = 'bcmTEvtOR'
        self.bbbAlgDict[222] = 'bcmTEvtAND'
        self.bbbAlgDict[226] = 'bcmTORC'

        self.bbbAlgDict[411] = 'zdclEvtAND'
        self.bbbAlgDict[412] = 'zdclEvtOR'
        self.bbbAlgDict[413] = 'zdclEvtORA'
        self.bbbAlgDict[414] = 'zdclEvtORC'

        self.bbbAlgDict[1001] = 'bcm11HEvtOR'
        self.bbbAlgDict[1002] = 'bcm11HEvtAND'
        self.bbbAlgDict[1004] = 'bcm11HXORC'

        self.bbbAlgDict[1011] = 'bcm11VEvtOR'
        self.bbbAlgDict[1012] = 'bcm11VEvtAND'
        self.bbbAlgDict[1014] = 'bcm11VXORC'
        
    def open(self):
        print 'ScanNtupleHandler.open() called'

        self.file = TFile(self.fileName, 'recreate')
        self.tree = TTree(self.treeName, self.treeName)

    def init(self):
        print 'ScanNtupleHandler.init() called'

        self.initScanData()
        self.initBeamPosition()
        self.initFillParams()
        self.initLbdata()
        self.initBunchData()
        self.initBunchLumi()
        self.initLumiData()
        self.initTurnData()
        
    # Must pass data object as a reference where extracted COOL data can be found
    # This relies on naming conventions, such as:
    #   data.scanData
    #   data.beamData
    #   data.fillData
    #   data.lbdataData
    #
    # Each of these should be a list of IObject values as returned by CoolDataReader
    def fill(self, data):
        print 'ScanNtupleHandler.fill() called'

        nfilled = 0 
        # First we need to fill the scan data
        for obj in data.scanData.data:

            # Make cuts here if desired?

            # Clear out the BCID object
            self.bcidData.clear()

            # Fill the scan data 
            self.fillScanData(obj)

            # Now fill other data.
            # Must match by IOV, so pass original scan data object, and the relevant data list

            # BEAMPOSITION
            self.fillBeamPosition(obj, data.beamData.data)

            # FILLPARAMS
            self.fillFillParams(obj, data.fillData.data)

            # LBDATA
            self.fillLbdata(obj, data.lbdataData.data)

            # BUNCHDATA
            self.fillBunchData(obj, data.bunchData.data)

            # BUNCHLUMI
            self.fillBunchLumi(obj, data.bunchLumi.data)
            
            # LUMINOSITY
            self.fillLumiData(obj, data.lumiData.data)

            # Turns
            if data.turnData != None:
                self.fillTurnData(obj, data.turnData.data)
            
            self.tree.Fill()
            nfilled += 1

        print 'ScanNtupleHandler.fill() - filled', nfilled, 'entries'
        
    def close(self):
        print 'ScanNtupleHandler.close() called'
        
        self.file.Write()
        self.file.Close()
        
    def initScanData(self):

        #
        # Define SCANDATA tree
        # Time is in ms
        structStr = "struct ScanDataStruct {\
                ULong64_t fStartTime;\
                ULong64_t fEndTime;\
                UInt_t fScanRun;\
                UInt_t fScanLB;\
                Float_t fStepProgress;\
                Float_t fScanningIP;\
                Float_t fAcquisitionFlag;\
                Float_t fMovingBeam;\
                Float_t fNominalSeparation;\
                Float_t fScanInPlane;\
            };"
        
        # Replace sizes if needed
        gROOT.ProcessLine(structStr)
        from ROOT import ScanDataStruct
        self.scanDataStruct = ScanDataStruct()

        # l = 64 bit unsigned int, L = 64 bit signed int
        self.tree.Branch('SCANDATA', self.scanDataStruct, 'StartTime/l:EndTime/l:ScanRun/i:ScanLB/i:StepProgress/F:ScanningIP/F:AcquisitionFlag/F:MovingBeam/F:NominalSeparation/F:ScanInPlane/F')

    # Pass an IObject object for a single ntuple entry
    def fillScanData(self, obj):
        self.scanDataStruct.fStartTime = obj.since() # Time in ns
        self.scanDataStruct.fEndTime = obj.until()   # Time in ns
        self.scanDataStruct.fScanRun = obj.payload()['RunLB'] >> 32
        self.scanDataStruct.fScanLB = obj.payload()['RunLB']&0xFFFFFFFF
        self.scanDataStruct.fStepProgress = obj.payload()['StepProgress']
        self.scanDataStruct.fScanningIP = obj.payload()['ScanningIP']
        self.scanDataStruct.fAcquisitionFlag = obj.payload()['AcquisitionFlag']
        self.scanDataStruct.fMovingBeam = obj.payload()['MovingBeam']
        self.scanDataStruct.fNominalSeparation = obj.payload()['NominalSeparation']
        self.scanDataStruct.fScanInPlane = obj.payload()['ScanInPlane']
        
    def initBeamPosition(self):

        #
        # Define BEAMPOSITION tree
        gROOT.ProcessLine("struct BeamPositionStruct {\
            Float_t fB1PositionH;\
            Float_t fB1PositionV;\
            Float_t fB2PositionH;\
            Float_t fB2PositionV;\
            Float_t fB1AngleH;\
            Float_t fB1AngleV;\
            Float_t fB2AngleH;\
            Float_t fB2AngleV;\
        };")
        from ROOT import BeamPositionStruct
        self.beamPositionStruct = BeamPositionStruct()

        self.tree.Branch('BEAMPOSITION', self.beamPositionStruct, 'B1PositionH/F:B1PositionV/F:B2PositionH/F:B2PositionV/F:B1AngleH/F:B1AngleV/F:B2AngleH/F:B2AngleV/F')

    def fillBeamPosition(self, timeobj, data):

        # Must match by IOV
        # These should match exactly, so just look for the same start time
        found = False
        for obj in data:
            if timeobj.since() != obj.since(): continue

            found = True
            self.beamPositionStruct.fB1PositionH = obj.payload()['B1_PositionAtIP_H']
            self.beamPositionStruct.fB1PositionV = obj.payload()['B1_PositionAtIP_V']
            self.beamPositionStruct.fB2PositionH = obj.payload()['B2_PositionAtIP_H']
            self.beamPositionStruct.fB2PositionV = obj.payload()['B2_PositionAtIP_V']
            self.beamPositionStruct.fB1AngleH = obj.payload()['B1_AngleAtIP_H']
            self.beamPositionStruct.fB1AngleV = obj.payload()['B1_AngleAtIP_V']
            self.beamPositionStruct.fB2AngleH = obj.payload()['B2_AngleAtIP_H']
            self.beamPositionStruct.fB2AngleV = obj.payload()['B2_AngleAtIP_V']
            break

            if not found:
                print "scanNtupleHandler.fillBeamPosition - Didn't find beam position data to match scan data!"
                self.beamPositionStruct.fB1PositionH = 0. 
                self.beamPositionStruct.fB1PositionV = 0. 
                self.beamPositionStruct.fB2PositionH = 0. 
                self.beamPositionStruct.fB2PositionV = 0. 
                self.beamPositionStruct.fB1AngleH = 0. 
                self.beamPositionStruct.fB1AngleV = 0. 
                self.beamPositionStruct.fB2AngleH = 0. 
                self.beamPositionStruct.fB2AngleV = 0. 


    def initFillParams(self):

        #
        # Define FILLPARAMS tree
        gROOT.ProcessLine("struct FillParamsStruct {\
            Int_t fB1Bunches;\
            Int_t fB1BCIDs[3564];\
            Int_t fB2Bunches;\
            Int_t fB2BCIDs[3564];\
            Int_t fLuminousBunches;\
            Int_t fLuminousBCIDs[3564];\
        };")
        from ROOT import FillParamsStruct
        self.fillParamsStruct = FillParamsStruct()

        self.fillParamsStruct.fB1Bunches = 0
        self.fillParamsStruct.fB2Bunches = 0
        self.fillParamsStruct.fLuminousBunches = 0
        # self.fB1BCIDs = array('i', 3564*[0])
        # self.fB2BCIDs = array('i', 3564*[0])
        # self.fLuminousBCIDs = array('i', 3564*[0])
        
        self.tree.Branch('FILLPARAMSB1', AddressOf(self.fillParamsStruct, 'fB1Bunches'), 'B1Bunches/I:B1BCIDs[B1Bunches]/I')
        self.tree.Branch('FILLPARAMSB2', AddressOf(self.fillParamsStruct, 'fB2Bunches'), 'B2Bunches/I:B2BCIDs[B2Bunches]/I')
        self.tree.Branch('FILLPARAMS', AddressOf(self.fillParamsStruct, 'fLuminousBunches'), 'LuminousBunches/I:LuminousBCIDs[LuminousBunches]/I')

        print 'Defined FillParams data type'
        #print 'type(fB1Bunches):', type(self.fillParamsStruct.fB1Bunches)
        #print 'type(fB1BCIDs):', type(self.fillParamsStruct.fB1BCIDs)
        
    def fillFillParams(self, timeobj, data):

        # Must match by IOV
        # FILLPARAMS is slow LHC data, so look for range that encloses start of timeobj IOV
        found = False
        for obj in data:
            # Check that start of scan step is enclosed in object IOV
            if timeobj.since() < obj.since(): continue
            if timeobj.since() >= obj.until(): continue 

            found = True
            self.fillParamsStruct.fB1Bunches = obj.payload()['Beam1Bunches']
            self.fillParamsStruct.fB2Bunches = obj.payload()['Beam2Bunches']
            self.fillParamsStruct.fLuminousBunches = obj.payload()['LuminousBunches']

            # Now need to unpack the BCID blob
            self.bcidData.fillMask(obj.payload())

            # Get the BCID lists and enter into ntuple arrays
            
            # self.fillParamsStruct.fB1BCIDs = array('i', list(self.bcidData.b1BCID))
            i = 0
            for bcid in self.bcidData.b1BCID:
                self.fillParamsStruct.fB1BCIDs[i] = bcid
                i += 1
                
            # self.fillParamsStruct.fB2BCIDs = array('i', list(self.bcidData.b2BCID))
            i = 0
            for bcid in self.bcidData.b2BCID:
                self.fillParamsStruct.fB2BCIDs[i] = bcid
                i += 1
                
            # self.fillParamsStruct.fLuminousBCIDs = array('i', list(self.bcidData.colBCID))
            i = 0
            for bcid in self.bcidData.colBCID:
                self.fillParamsStruct.fLuminousBCIDs[i] = bcid
                i += 1
                
        if not found:
            print "scanNtupleHandler.fillFillParams - Didn't find FILLPARAMS data to match scan data!"
            self.fillParamsStruct.fB1Bunches = 0
            self.fillParamsStruct.fB2Bunches = 0
            self.fillParamsStruct.fLuminousBunches = 0

            for i in range(3564):
                self.fillParamsStruct.fB1BCIDs[i] = 0
                self.fillParamsStruct.fB2BCIDs[i] = 0
                self.fillParamsStruct.fLuminousBCIDs[i] = 0
            
    def initLbdata(self):

        #
        # Define LBDATA tree
        gROOT.ProcessLine("struct LbdataStruct {\
            Float_t fB1IntensityBPTX;\
            Float_t fB2IntensityBPTX;\
            Float_t fB1IntensityAllBPTX;\
            Float_t fB2IntensityAllBPTX;\
            Float_t fB1IntensityBCT;\
            Float_t fB2IntensityBCT;\
            Float_t fB1IntensityAllBCT;\
            Float_t fB2IntensityAllBCT;\
            Float_t fB1IntensityAllDCCT;\
            Float_t fB2IntensityAllDCCT;\
        };")
        from ROOT import LbdataStruct
        self.lbdataStruct = LbdataStruct()

        self.tree.Branch('BPTX_LBDATA', AddressOf(self.lbdataStruct, 'fB1IntensityBPTX'), 'BPTX_B1Intensity/F:BPTX_B2Intensity/F:BPTX_B1IntensityAll/F:BPTX_B2IntensityAll/F')
        self.tree.Branch('BCT_LBDATA', AddressOf(self.lbdataStruct, 'fB1IntensityBCT'), 'BCT_B1Intensity/F:BCT_B2Intensity/F:BCT_B1IntensityAll/F:BCT_B2IntensityAll/F')
        self.tree.Branch('DCCT_LBDATA', AddressOf(self.lbdataStruct, 'fB1IntensityAllDCCT'), 'DCCT_B1IntensityAll/F:DCCT_B2IntensityAll/F')

    def fillLbdata(self, timeobj, data):

        # Must match by IOV
        # LBDATA is fast data, so just match directly
        foundBPTX = False
        foundBCT = False
        foundDCCT = False
        
        for obj in data:
            if timeobj.since() != obj.since(): continue

            if obj.channelId() == 0:
                foundBPTX = True
            
                self.lbdataStruct.fB1IntensityBPTX = obj.payload()['Beam1Intensity']
                self.lbdataStruct.fB2IntensityBPTX = obj.payload()['Beam2Intensity']
                self.lbdataStruct.fB1IntensityAllBPTX = obj.payload()['Beam1IntensityAll']
                self.lbdataStruct.fB2IntensityAllBPTX = obj.payload()['Beam2IntensityAll']
                
            elif obj.channelId() == 1:
                foundBCT = True
            
                self.lbdataStruct.fB1IntensityBCT = obj.payload()['Beam1Intensity']
                self.lbdataStruct.fB2IntensityBCT = obj.payload()['Beam2Intensity']
                self.lbdataStruct.fB1IntensityAllBCT = obj.payload()['Beam1IntensityAll']
                self.lbdataStruct.fB2IntensityAllBCT = obj.payload()['Beam2IntensityAll']
                
            elif obj.channelId() == 2:
                foundDCCT = True
            
                self.lbdataStruct.fB1IntensityAllDCCT = obj.payload()['Beam1IntensityAll']
                self.lbdataStruct.fB2IntensityAllDCCT = obj.payload()['Beam2IntensityAll']
                
            else:
                print 'scanNtupleHandler.fillLbdata - Found unknown channel', obj.channelId(), '!'

            if foundBCT and foundBPTX and foundDCCT : break


        if (not foundBPTX) and (not foundBCT) and (not foundDCCT):
            print "scanNtupleHandler.fillLBdata - Couldn't find LBDATA!"
        elif not foundBCT:
            print "scanNtupleHandler.fillLBdata - Couldn't find BCT data in LBDATA!"
        elif not foundBPTX:
            print "scanNtupleHandler.fillLBdata - Couldn't find BPTX data in LBDATA!"
        elif not foundDCCT:
            print "scanNtupleHandler.fillLBdata - Couldn't find DCCT data in LBDATA!"

        if not foundBCT:
            self.lbdataStruct.fB1IntensityBCT = 0.
            self.lbdataStruct.fB2IntensityBCT = 0.
            self.lbdataStruct.fB1IntensityAllBCT = 0.
            self.lbdataStruct.fB2IntensityAllBCT = 0.
            
        if not foundBPTX:
            self.lbdataStruct.fB1IntensityBPTX = 0.
            self.lbdataStruct.fB2IntensityBPTX = 0.
            self.lbdataStruct.fB1IntensityAllBPTX = 0.
            self.lbdataStruct.fB2IntensityAllBPTX = 0.

        if not foundDCCT:
            self.lbdataStruct.fB1IntensityAllDCCT = 0.
            self.lbdataStruct.fB2IntensityAllDCCT = 0.
                
    def initBunchData(self):

        #
        # Define BUNCHDATA tree
        gROOT.ProcessLine("struct BunchDataStruct {\
            UInt_t  fValidBPTX;\
            Float_t fB1BunchAverageBPTX;\
            Float_t fB2BunchAverageBPTX;\
            Int_t   fB1BunchesBPTX;\
            Int_t   fB2BunchesBPTX;\
            Float_t fB1BunchIntensitiesBPTX[3564];\
            Int_t   fB1BCIDsBPTX[3564];\
            Float_t fB2BunchIntensitiesBPTX[3564];\
            Int_t   fB2BCIDsBPTX[3564];\
            UInt_t  fValidBCT;\
            Float_t fB1BunchAverageBCT;\
            Float_t fB2BunchAverageBCT;\
            Int_t   fB1BunchesBCT;\
            Int_t   fB2BunchesBCT;\
            Int_t   fB1BCIDsBCT[3564];\
            Float_t fB1BunchIntensitiesBCT[3564];\
            Int_t   fB2BCIDsBCT[3564];\
            Float_t fB2BunchIntensitiesBCT[3564];\
        };")
        from ROOT import BunchDataStruct
        self.bunchDataStruct = BunchDataStruct()

        # Not sure why this is necessary
        self.bunchDataStruct.fB1BunchesBPTX = 0
        self.bunchDataStruct.fB2BunchesBPTX = 0
        self.bunchDataStruct.fB1BunchesBCT = 0
        self.bunchDataStruct.fB2BunchesBCT = 0
        
        self.tree.Branch('BCT_BUNCHDATA', AddressOf(self.bunchDataStruct, 'fValidBCT'), 'BCT_Valid/i:BCT_B1BunchAverage/F:BCT_B2BunchAverage/F:BCT_B1Bunches/I:BCT_B2Bunches/I')
        self.tree.Branch('BCT_B1BCID', AddressOf(self.bunchDataStruct, 'fB1BCIDsBCT'), 'BCT_B1BCID[BCT_B1Bunches]/I')
        self.tree.Branch('BCT_B2BCID', AddressOf(self.bunchDataStruct, 'fB2BCIDsBCT'), 'BCT_B2BCID[BCT_B2Bunches]/I')
        self.tree.Branch('BCT_B1BunchIntensity',  AddressOf(self.bunchDataStruct, 'fB1BunchIntensitiesBCT'), 'BCT_B1BunchIntensity[BCT_B1Bunches]/F')
        self.tree.Branch('BCT_B2BunchIntensity',  AddressOf(self.bunchDataStruct, 'fB2BunchIntensitiesBCT'), 'BCT_B2BunchIntensity[BCT_B2Bunches]/F')

        self.tree.Branch('BPTX_BUNCHDATA', AddressOf(self.bunchDataStruct, 'fValidBPTX'), 'BPTX_Valid/i:BPTX_B1BunchAverage/F:BPTX_B2BunchAverage/F:BPTX_B1Bunches/I:BPTX_B2Bunches/I')
        self.tree.Branch('BPTX_B1BCID', AddressOf(self.bunchDataStruct, 'fB1BCIDsBPTX'), 'BPTX_B1BCID[BPTX_B1Bunches]/I')
        self.tree.Branch('BPTX_B2BCID', AddressOf(self.bunchDataStruct, 'fB2BCIDsBPTX'), 'BPTX_B2BCID[BPTX_B2Bunches]/I')
        self.tree.Branch('BPTX_B1BunchIntensity',  AddressOf(self.bunchDataStruct, 'fB1BunchIntensitiesBPTX'), 'BPTX_B1BunchIntensity[BPTX_B1Bunches]/F')
        self.tree.Branch('BPTX_B2BunchIntensity',  AddressOf(self.bunchDataStruct, 'fB2BunchIntensitiesBPTX'), 'BPTX_B2BunchIntensity[BPTX_B2Bunches]/F')


    def fillBunchData(self, timeobj, data):

        # Must match by IOV
        # LBDATA is fast data, so just match directly
        foundBPTX = False
        foundBCT = False
        
        for obj in data:
            if timeobj.since() != obj.since(): continue

            # Now need to unpack the BCID blob
            self.bcidData.fillCurrents(obj.payload())

            if obj.channelId() == 0:
                foundBPTX = True
            
                self.bunchDataStruct.fB1BunchAverageBPTX = obj.payload()['B1BunchAverage']
                self.bunchDataStruct.fB2BunchAverageBPTX = obj.payload()['B2BunchAverage']
                self.bunchDataStruct.fValidBPTX = obj.payload()['Valid']

                self.bunchDataStruct.fB1BunchesBPTX = len(self.bcidData.b1Curr)
                # Following line causes problems?
                # Try the brute force apprach again. 
                # self.bunchDataStruct.fB1BCIDsBPTX = array('i', self.bcidData.b1Curr.keys())
                # self.bunchDataStruct.fB1BunchIntensitiesBPTX = array('f', self.bcidData.b1Curr.values())
                
                i = 0
                for (bcid, val) in self.bcidData.b1Curr.iteritems():
                    self.bunchDataStruct.fB1BCIDsBPTX[i] = bcid
                    self.bunchDataStruct.fB1BunchIntensitiesBPTX[i] = val
                    i += 1

                self.bunchDataStruct.fB2BunchesBPTX = len(self.bcidData.b2Curr)
                # self.bunchDataStruct.fB2BCIDsBPTX = array('i', self.bcidData.b2Curr.keys())
                # self.bunchDataStruct.fB2BunchIntensitiesBPTX = array('f', self.bcidData.b2Curr.values())

                i = 0
                for (bcid, val) in self.bcidData.b2Curr.iteritems():
                    self.bunchDataStruct.fB2BCIDsBPTX[i] = bcid
                    self.bunchDataStruct.fB2BunchIntensitiesBPTX[i] = val
                    i += 1

            elif obj.channelId() == 1:
                foundBCT = True

                self.bunchDataStruct.fB1BunchAverageBCT = obj.payload()['B1BunchAverage']
                self.bunchDataStruct.fB2BunchAverageBCT = obj.payload()['B2BunchAverage']
                self.bunchDataStruct.fValidBCT = obj.payload()['Valid']

                self.bunchDataStruct.fB1BunchesBCT = len(self.bcidData.b1Curr)
                #self.bunchDataStruct.fB1BCIDsBCT = array('i', self.bcidData.b1Curr.keys())
                #self.bunchDataStruct.fB1BunchIntensitiesBCT = array('f', self.bcidData.b1Curr.values())
                
                i = 0
                for (bcid, val) in self.bcidData.b1Curr.iteritems():
                    self.bunchDataStruct.fB1BCIDsBCT[i] = bcid
                    self.bunchDataStruct.fB1BunchIntensitiesBCT[i] = val
                    i += 1

                self.bunchDataStruct.fB2BunchesBCT = len(self.bcidData.b2Curr)
                # self.bunchDataStruct.fB2BCIDsBCT = array('i', self.bcidData.b2Curr.keys())
                # self.bunchDataStruct.fB2BunchIntensitiesBCT = array('f', self.bcidData.b2Curr.values())
                
                i = 0
                for (bcid, val) in self.bcidData.b2Curr.iteritems():
                    self.bunchDataStruct.fB2BCIDsBCT[i] = bcid
                    self.bunchDataStruct.fB2BunchIntensitiesBCT[i] = val
                    i += 1
                    
            else:
                print 'scanNtupleHandler.fillLbdata - Found unknown channel', obj.channelId(), '!'

            if foundBCT and foundBPTX: break


        if (not foundBPTX) and (not foundBCT):
            print "scanNtupleHandler.fillBunchData - Couldn't find BUNCHDATA!"
        elif not foundBCT:
            print "scanNtupleHandler.fillBunchData - Couldn't find BCT data in BUNCHDATA!"
        elif not foundBPTX:
            print "scanNtupleHandler.fillBunchData - Couldn't find BPTX data in BUNCHDATA!"

        if not foundBCT:
            self.bunchDataStruct.fB1BunchAverageBCT = 0.
            self.bunchDataStruct.fB2BunchAverageBCT = 0.
            self.bunchDataStruct.fValidBCT = -1
            self.bunchDataStruct.fB1BunchesBCT = 0
            self.bunchDataStruct.fB2BunchesBCT = 0
            
        if not foundBPTX:
            self.bunchDataStruct.fB1BunchAverageBPTX = 0.
            self.bunchDataStruct.fB2BunchAverageBPTX = 0.
            self.bunchDataStruct.fValidBPTX = -1
            self.bunchDataStruct.fB1BunchesBPTX = 0
            self.bunchDataStruct.fB2BunchesBPTX = 0

        # print 'BCT Avg:', self.bunchDataStruct.fB1BunchAverageBCT
        # for i in range(self.bunchDataStruct.fB1BunchesBCT):
            # print '     BCID:', self.bunchDataStruct.fB1BCIDsBCT[i], self.bunchDataStruct.fB2BunchIntensitiesBCT[i]

    def initBunchLumi(self):

        #
        # Define BUNCHDATA tree
        gROOT.ProcessLine("struct BunchLumiStruct {\
            UInt_t  fChannel;\
            UInt_t  fValid;\
            Float_t fAverageRawInstLum;\
            Int_t   fLuminousBunches;\
            Int_t   fLuminousBCIDs[3564];\
            Float_t fBunchRawInstLum[3564];\
        };")
        from ROOT import BunchLumiStruct

        self.bunchLumiStruct = dict()
        self.fLuminousBCIDs = dict()
        self.fBunchRawInstLum = dict()
        
        for (chId, algstr) in self.bbbAlgDict.iteritems():
            print 'scanNtupleHandler.initLumiData - initializing', algstr, 'as channel', chId
            self.bunchLumiStruct[chId] = BunchLumiStruct()
            self.bunchLumiStruct[chId].fLuminousBunches = 0

            self.fLuminousBCIDs[chId] = array('i', 3564*[0])
            self.fBunchRawInstLum[chId] = array('f', 3564*[0.])
            
            branchString1 = 'ALG_Channel/i:ALG_Valid/i:ALG_AverageRawInstLum/F:ALG_LuminousBunches/I'
            branchString2 = 'ALG_LuminousBCIDs[ALG_LuminousBunches]/I'
            branchString3 = 'ALG_BunchRawInstLum[ALG_LuminousBunches]/F'

            self.tree.Branch(algstr+'_BUNCHDATA', self.bunchLumiStruct[chId], branchString1.replace('ALG', algstr))
            #self.tree.Branch(algstr+'_BCID', AddressOf(self.bunchLumiStruct[chId], 'fLuminousBCIDs'), branchString2.replace('ALG', algstr))
            #self.tree.Branch(algstr+'_BunchRawInstLum', AddressOf(self.bunchLumiStruct[chId], 'fBunchRawInstLum'), branchString3.replace('ALG', algstr))
            self.tree.Branch(algstr+'_BCID', self.fLuminousBCIDs[chId], branchString2.replace('ALG', algstr))
            self.tree.Branch(algstr+'_BunchRawInstLum', self.fBunchRawInstLum[chId], branchString3.replace('ALG', algstr))
            
    def fillBunchLumi(self, timeobj, data):

        # Must match by IOV - exact match
        # Keep track of every algorithm separately
        foundAny = False
        
        found = dict()
        for chId in self.bbbAlgDict.iterkeys():
            found[chId] = False

        for obj in data:
            if timeobj.since() != obj.since(): continue

            # Figure out which channel
            chId = obj.channelId()
            if not chId in self.bbbAlgDict:
                print 'scanNtupleHandler.fillBunchLumi - Unknown lumi channel', chId, '!'
                continue

            foundAny = True

            # Unpack lumi blob
            self.bcidData.fillRawLumi(obj.payload())
            
            found[chId] = True
            self.bunchLumiStruct[chId].fChannel = obj.channelId()
            self.bunchLumiStruct[chId].fValid = obj.payload()['Valid']
            self.bunchLumiStruct[chId].fAverageRawInstLum = obj.payload()['AverageRawInstLum']

            self.bunchLumiStruct[chId].fLuminousBunches = len(self.bcidData.rawLumi)
            #self.fLuminousBCIDs[chId] = array('i', self.bcidData.rawLumi.keys())
            #self.fBunchRawInstLum[chId] = array('f', self.bcidData.rawLumi.values())
            #if obj.channelId() == 102:
            #    print 'Channel: %d, Luminous bunches: %d' % (obj.channelId(), len(self.bcidData.rawLumi))
            
            i = 0
            bcidList = self.bcidData.rawLumi.keys()
            for bcid in sorted(bcidList):
                val = self.bcidData.rawLumi[bcid]
                self.fLuminousBCIDs[chId][i] = bcid
                self.fBunchRawInstLum[chId][i] = val
                i += 1

#                 self.bunchLumiStruct[chId].fLuminousBCIDs[i] = bcid
#                 self.bunchLumiStruct[chId].fBunchRawInstLum[i] = val

        if not foundAny:
            print 'scanNtupleHandler.fillBunchLumi - Found no BUNCHLUMIS data to match IOV!'

        else:
            for (chId, algstr) in self.bbbAlgDict.iteritems():
                if not found[chId]:
                    print 'scanNtupleHandler.fillBunchLumi - Found no BUNCHLUMIS data for', algstr, '!'

        for chId in self.bbbAlgDict.iterkeys():
            if not found[chId]:
                self.bunchLumiStruct[chId].fChannel = chId
                self.bunchLumiStruct[chId].fValid = 0xFFFFFFFF
                self.bunchLumiStruct[chId].fAverageRawInstLum = 0.
                self.bunchLumiStruct[chId].fLuminousBunches = 0
                
    def initLumiData(self):
        print 'scanNtupleHandler.initLumiData() called'
        
        #
        # Define LUMINOSITY tree
        # Try to abstract this for different lumi algorithms
        gROOT.ProcessLine("struct LumiDataStruct {\
            UInt_t fChannel;\
            Float_t fLBAvInstLumPhys;\
            Float_t fLBAvEvtsPerBXPhys;\
            Float_t fLBAvRawInstLumPhys;\
            Float_t fLBAvInstLumAll;\
            Float_t fLBAvRawInstLumAll;\
            Float_t fLBAvEvtsPerBXAll;\
            Float_t fLBAvOLCInstLum;\
            Float_t fLBAvOLCRawInstLum;\
            Float_t fLBAvOLCEvtsPerBX;\
            UInt_t fNOrbPhys;\
            UInt_t fNOrbAll;\
            UInt_t fNOrbOLC;\
            UInt_t fDetectorState;\
            UInt_t fValid;\
        };")
        from ROOT import LumiDataStruct

        self.lumiStruct = dict()
        for (chId, algstr) in self.algDict.iteritems():
            print 'scanNtupleHandler.initLumiData - initializing', algstr, 'as channel', chId
            self.lumiStruct[chId] = LumiDataStruct()
            branchString = 'ALG_LumiChannel/i:ALG_LBAvInstLumPhys/F:ALG_LBAvEvtsPerBXPhys/F:ALG_LBAvRawInstLumPhys/F:ALG_LBAvInstLumAll/F:ALG_LBAvRawInstLumAll/F:ALG_LBAvEvtsPerBXAll/F:ALG_LBAvOLCInstLum/F:ALG_LBAvOLCRawInstLum/F:ALG_LBAvOLCEvtsPerBX/F:ALG_NOrbPhys/i:ALG_NOrbAll/i:ALG_NOrbOLC/i:ALG_DetectorState/i:ALG_LumiValid/i'

            self.tree.Branch(algstr+'_LUMI', self.lumiStruct[chId], branchString.replace('ALG', algstr))

    def fillLumiData(self, timeobj, data):

        # Must match by IOV - exact match
        # Keep track of every algorithm separately
        foundAny = False
        
        found = dict()
        for chId in self.algDict.iterkeys():
            found[chId] = False

        for obj in data:
            if timeobj.since() != obj.since(): continue

            # Figure out which channel
            chId = obj.channelId()
            if chId == 5:
                continue # Ignore CMS luminosity
            
            elif not chId in self.algDict:
                print 'scanNtupleHandler.fillLumiData - Unknown lumi channel', chId, '!'
                continue

            foundAny = True
            
            # Fill the data for this algorithm
            found[chId] = True
            self.lumiStruct[chId].fChannel = obj.channelId()
            self.lumiStruct[chId].fLBAvInstLumPhys = obj.payload()['LBAvInstLumPhys']
            self.lumiStruct[chId].fLBAvEvtsPerBXPhys = obj.payload()['LBAvEvtsPerBXPhys']
            self.lumiStruct[chId].fLBAvRawInstLumPhys = obj.payload()['LBAvRawInstLumPhys']
            self.lumiStruct[chId].fLBAvInstLumAll = obj.payload()['LBAvInstLumAll']
            self.lumiStruct[chId].fLBAvEvtsPerBXAll = obj.payload()['LBAvEvtsPerBXAll']
            self.lumiStruct[chId].fLBAvRawInstLumAll = obj.payload()['LBAvRawInstLumAll']
            self.lumiStruct[chId].fLBAvOLCInstLum = obj.payload()['LBAvOLCInstLum']
            self.lumiStruct[chId].fLBAvOLCEvtsPerBX = obj.payload()['LBAvOLCEvtsPerBX']
            self.lumiStruct[chId].fLBAvOLCRawInstLum = obj.payload()['LBAvOLCRawInstLum']
            self.lumiStruct[chId].fDetectorState = obj.payload()['DetectorState']
            try:
                self.lumiStruct[chId].fNOrbPhys = obj.payload()['NOrbPhys']
                self.lumiStruct[chId].fNOrbAll = obj.payload()['NOrbAll']
                self.lumiStruct[chId].fNOrbOLC = obj.payload()['NOrbOLC']
            except Exception, e:
                print e
                self.lumiStruct[chId].fNOrbPhys = 0
                self.lumiStruct[chId].fNOrbAll = 0
                self.lumiStruct[chId].fNOrbOLC = 0
                
            self.lumiStruct[chId].fValid = obj.payload()['Valid']

        if not foundAny:
            print 'scanNtupleHandler.fillLumiData - Found no LUMINOSITY data to match IOV!'

        else:
            for (chId, algstr) in self.algDict.iteritems():
                if not found[chId]:
                    print 'scanNtupleHandler.fillLumiData - Found no LUMINOSITY data for', algstr, '!'

        for chId in self.algDict.iterkeys():
            if not found[chId]:
                self.lumiStruct[chId].fChannel = chId
                self.lumiStruct[chId].fLBAvInstLumPhys = 0.
                self.lumiStruct[chId].fLBAvEvtsPerBXPhys = 0.
                self.lumiStruct[chId].fLBAvRawInstLumPhys = 0.
                self.lumiStruct[chId].fLBAvInstLumAll = 0.
                self.lumiStruct[chId].fLBAvEvtsPerBXAll = 0.
                self.lumiStruct[chId].fLBAvRawInstLumAll = 0.
                self.lumiStruct[chId].fLBAvOLCInstLum = 0.
                self.lumiStruct[chId].fLBAvOLCEvtsPerBX = 0.
                self.lumiStruct[chId].fLBAvOLCRawInstLum = 0.
                self.lumiStruct[chId].fNOrbPhys = 0
                self.lumiStruct[chId].fNOrbAll = 0
                self.lumiStruct[chId].fNOrbOLC = 0
                self.lumiStruct[chId].fDetectorState = 0
                self.lumiStruct[chId].fValid = 0xFFFFFFFF

    # turn and count data per algorithm from Nitesh's files
    def initTurnData(self):
        print 'scanNtupleHandler.initTurnData() called'
        
        #
        # Define tree
        # Try to abstract this for different lumi algorithms
        gROOT.ProcessLine("struct TurnDataStruct {\
            Float_t fTurnsPhys;\
            Float_t fCountsPhys;\
            Float_t fTurnsAll;\
            Float_t fCountsAll;\
            Float_t fTurnsOLC;\
            Float_t fCountsOLC;\
        };")
        from ROOT import TurnDataStruct

        self.turnStruct = dict()
        for (chId, algstr) in self.algDict.iteritems():
            if chId == 0: continue
            print 'scanNtupleHandler.initTurnData - initializing', algstr, 'as channel', chId
            self.turnStruct[chId] = TurnDataStruct()
            branchString = 'ALG_BXPhys/F:ALG_CountsPhys/F:ALG_BXAll/F:ALG_CountsAll/F:ALG_BXOLC/F:ALG_CountsOLC/F'

            self.tree.Branch(algstr+'_TURN', self.turnStruct[chId], branchString.replace('ALG', algstr))


    def fillTurnData(self, timeobj, data):

        # Must match by IOV - exact match
        # Keep track of every algorithm separately
        foundAny = False
        
        found = dict()
        for chId in self.algDict.iterkeys():
            found[chId] = False

        # Insert invalid values here
        for chId in self.algDict.iterkeys():
            if chId == 0: continue
            self.turnStruct[chId].fTurnsPhys = -1.
            self.turnStruct[chId].fCountsPhys = -1.
            self.turnStruct[chId].fTurnsAll = -1.
            self.turnStruct[chId].fCountsAll = -1.
            self.turnStruct[chId].fTurnsOLC = -1.
            self.turnStruct[chId].fCountsOLC = -1.
            
        # Look for our lumi block
        run = timeobj.payload()['RunLB'] >> 32
        lb = timeobj.payload()['RunLB'] & 0xFFFFFFFF
        
        if not lb in data:
            print 'scanNtupleHandler.fillTurnData - Found no turn data to match IOV %d/%d !' % (run, lb)

        else:

            # Search for channels based on our list
            for (chId, chName) in self.algDict.iteritems():

                if chId == 0: continue

                # Change my naming to match Nitesh's naming
                chStrBase = chName.replace('Evt', 'Event')
                chStrBase = chStrBase.replace('bcmH', 'bcm')
                chStrBase = chStrBase.replace('bcmXORC', 'bcmEventXORC')
                
                # Three types of data in tree
                for ext in ['_OLCLBAv', '_PHYS', '_LBAvALL']:
                    chStr = chStrBase+ext
                    if not chStr in data[lb]:
                        print 'scanNtupleHandler.fillTurnData - no data in lb', lb, 'for', chStr
                        continue

                    (turns, counts) = data[lb][chStr]

                    if ext == '_OLCLBAv':
                        self.turnStruct[chId].fTurnsOLC = turns
                        self.turnStruct[chId].fCountsOLC = counts

                    if ext == '_PHYS':
                        self.turnStruct[chId].fTurnsPhys = turns
                        self.turnStruct[chId].fCountsPhys = counts

                    if ext == '_LBAvALL':
                        self.turnStruct[chId].fTurnsAll = turns
                        self.turnStruct[chId].fCountsAll = counts

                        
                    
