# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import TrigL2MuonSA.TrigL2MuonSAConf as MuonSA
from TrigL2MuonSA.TrigL2MuonSAMonitoring import TrigL2MuonSAMonitoring
from AthenaCommon.AppMgr import ServiceMgr,ToolSvc
from AthenaCommon.DetFlags import DetFlags
from TrigMuonBackExtrapolator.TrigMuonBackExtrapolatorConfig import MuonBackExtrapolatorForAlignedDet, MuonBackExtrapolatorForMisalignedDet,  MuonBackExtrapolatorForData
from TriggerJobOpts.TriggerFlags import TriggerFlags
from RegionSelector.RegSelToolConfig import makeRegSelTool_MDT
from RegionSelector.RegSelToolConfig import makeRegSelTool_RPC
from RegionSelector.RegSelToolConfig import makeRegSelTool_TGC
from RegionSelector.RegSelToolConfig import makeRegSelTool_CSC

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigL2MuonSAConfig')

theStationFitter     = MuonSA.TrigL2MuonSA__MuFastStationFitter(PtFromAlphaBeta = MuonSA.TrigL2MuonSA__PtFromAlphaBeta())

theDataPreparator    = MuonSA.TrigL2MuonSA__MuFastDataPreparator()
theDataPreparator.RPCDataPreparator  = MuonSA.TrigL2MuonSA__RpcDataPreparator( DecodeBS = DetFlags.readRDOBS.RPC_on())
theDataPreparator.MDTDataPreparator  = MuonSA.TrigL2MuonSA__MdtDataPreparator( DecodeBS = DetFlags.readRDOBS.MDT_on())
theDataPreparator.TGCDataPreparator  = MuonSA.TrigL2MuonSA__TgcDataPreparator( DecodeBS = DetFlags.readRDOBS.TGC_on())
theDataPreparator.CSCDataPreparator  = MuonSA.TrigL2MuonSA__CscDataPreparator( DecodeBS = DetFlags.readRDOBS.CSC_on())
theDataPreparator.STGCDataPreparator = MuonSA.TrigL2MuonSA__StgcDataPreparator(DecodeBS = DetFlags.readRDOBS.sTGC_on())
theDataPreparator.MMDataPreparator   = MuonSA.TrigL2MuonSA__MmDataPreparator(  DecodeBS = DetFlags.readRDOBS.Micromegas_on())

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
if not MuonGeometryFlags.hasSTGC():
    theDataPreparator.STGCDataPreparator.DoDecoding=False
    theDataPreparator.STGCDataPreparator.DecodeBS=False
    theDataPreparator.STGCDataPreparator.StgcPrepDataContainer=""
if not MuonGeometryFlags.hasMM():
    theDataPreparator.MMDataPreparator.DoDecoding=False
    theDataPreparator.MMDataPreparator.DecodeBS=False
    theDataPreparator.MMDataPreparator.MmPrepDataContainer=""
if not MuonGeometryFlags.hasCSC():
    theDataPreparator.CSCDataPreparator.DoDecoding=False
    theDataPreparator.CSCDataPreparator.DecodeBS=False
    theDataPreparator.CSCDataPreparator.CSCPrepDataContainer  = ""

#Need different PRD collection names to run offline and Run 2 trigger in same job
if not TriggerFlags.doMT():
    from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
    MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataTool(name = "TrigMdtRdoToPrepDataTool",OutputCollection="TrigMDT_DriftCircles")
    ToolSvc += MdtRdoToMdtPrepDataTool
    theDataPreparator.MDTDataPreparator.MdtPrepDataProvider =  MdtRdoToMdtPrepDataTool
    theDataPreparator.MDTDataPreparator.MDTPrepDataContainer = MdtRdoToMdtPrepDataTool.OutputCollection

    if MuonGeometryFlags.hasCSC():
        from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
        CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataTool(name = "TrigCscRdoToPrepDataTool", OutputCollection="TrigCSC_Measurements")
        ToolSvc += CscRdoToCscPrepDataTool
        theDataPreparator.CSCDataPreparator.CscPrepDataProvider =  CscRdoToCscPrepDataTool
        theDataPreparator.CSCDataPreparator.CSCPrepDataContainer = CscRdoToCscPrepDataTool.OutputCollection

    from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
    TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataTool(name = "TrigTgcRdoToPrepDataTool",OutputCollection="TrigTGC_Measurements",OutputCoinCollection="TrigerT1CoinDataCollection")
    ToolSvc += TgcRdoToTgcPrepDataTool
    theDataPreparator.TGCDataPreparator.TgcPrepDataProvider =  TgcRdoToTgcPrepDataTool
    theDataPreparator.TGCDataPreparator.TGCPrepDataContainer = TgcRdoToTgcPrepDataTool.OutputCollection

    from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
    #InputCollection is really the output RPC coin collection...
    RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name = "TrigRpcRdoToPrepDataTool", TriggerOutputCollection="TrigRPC_Measurements", InputCollection="TrigRPC_triggerHits")
    ToolSvc += RpcRdoToRpcPrepDataTool
    theDataPreparator.RPCDataPreparator.RpcPrepDataProvider =  RpcRdoToRpcPrepDataTool
    theDataPreparator.RPCDataPreparator.RpcPrepDataContainer = RpcRdoToRpcPrepDataTool.TriggerOutputCollection

    if MuonGeometryFlags.hasSTGC():
        from MuonSTGC_CnvTools.MuonSTGC_CnvToolsConf import Muon__StgcRdoToPrepDataTool
        #InputCollection is really the output sTGC coin collection...
        StgcRdoToStgcPrepDataTool = Muon__StgcRdoToPrepDataTool(name = "TrigStgcRdoToPrepDataTool", TriggerOutputCollection="TrigSTGC_Measurements", InputCollection="TrigSTGC_triggerHits")
        ToolSvc += StgcRdoToStgcPrepDataTool
        theDataPreparator.STGCDataPreparator.StgcPrepDataProvider =  StgcRdoToStgcPrepDataTool
        theDataPreparator.STGCDataPreparator.StgcPrepDataContainer = StgcRdoToStgcPrepDataTool.TriggerOutputCollection

    if MuonGeometryFlags.hasMM():
        from MuonMM_CnvTools.MuonMM_CnvToolsConf import Muon__MmRdoToPrepDataTool
        #InputCollection is really the output MM coin collection...
        MmRdoToMmPrepDataTool = Muon__MmRdoToPrepDataTool(name = "TrigMmRdoToPrepDataTool", TriggerOutputCollection="TrigMM_Measurements", InputCollection="TrigMM_triggerHits")
        ToolSvc += MmRdoToMmPrepDataTool
        # theDataPreparator.MMDataPreparator.MmPrepDataProvider =  MmRdoToMmPrepDataTool
        theDataPreparator.MMDataPreparator.MmPrepDataContainer = MmRdoToMmPrepDataTool.TriggerOutputCollection



ToolSvc += MuonBackExtrapolatorForAlignedDet()
ToolSvc += MuonBackExtrapolatorForMisalignedDet()
ToolSvc += MuonBackExtrapolatorForData()

ToolSvc += MuonSA.TrigL2MuonSA__CscSegmentMaker()
ToolSvc += MuonSA.TrigL2MuonSA__CscRegDict()

class PtBarrelLUTSvc(MuonSA.TrigL2MuonSA__PtBarrelLUTSvc):
    def __init__(self,name = 'PtBarrelLUTSvc'):
        super(PtBarrelLUTSvc ,self).__init__(name)
        self.LUTfile = "pt_barrel.lut"
        self.SP_LUTfile = "pt_barrelSP_new.lut"

class PtBarrelLUTSvc_MC(MuonSA.TrigL2MuonSA__PtBarrelLUTSvc):
    def __init__(self,name = 'PtBarrelLUTSvc_MC'):
        super(PtBarrelLUTSvc_MC ,self).__init__(name)
        self.LUTfile = "pt_barrel.mc10.lut"

class PtEndcapLUTSvc(MuonSA.TrigL2MuonSA__PtEndcapLUTSvc):
    def __init__(self,name = 'PtEndcapLUTSvc'):
        super(PtEndcapLUTSvc ,self).__init__(name)
        self.FileName = "pt_endcap.lut"
        self.EMeanLUT = "pt_comb_mean.lut"
        self.ESigmaLUT = "pt_comb_sigma.lut"

class PtEndcapLUTSvc_MC(MuonSA.TrigL2MuonSA__PtEndcapLUTSvc):
    def __init__(self,name = 'PtEndcapLUTSvc_MC'):
        super(PtEndcapLUTSvc_MC ,self).__init__(name)
        self.FileName = "pt_endcap.mc10.lut"
        self.EMeanLUT = "pt_comb_mean.lut"
        self.ESigmaLUT = "pt_comb_sigma.lut"

class AlignmentBarrelLUTSvc(MuonSA.TrigL2MuonSA__AlignmentBarrelLUTSvc):
    def __init__(self,name = 'AlignmentBarrelLUTSvc'):
        super(AlignmentBarrelLUTSvc ,self).__init__(name)
        self.LUTfile = "dZ_barrel.lut"



# declare the needed services
ServiceMgr += PtEndcapLUTSvc()
ServiceMgr += PtEndcapLUTSvc_MC()
ServiceMgr += PtBarrelLUTSvc()
ServiceMgr += PtBarrelLUTSvc_MC()
ServiceMgr += AlignmentBarrelLUTSvc()

### for athenaMT ###
class TrigL2MuonSAMTConfig(MuonSA.MuFastSteering):

    __slot__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s' % (cls.getType(),args[0]) ] + list(args)
        return super( TrigL2MuonSAMTConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigL2MuonSAMTConfig, self ).__init__( name )

        self.DataPreparator    = theDataPreparator
        self.PatternFinder     = MuonSA.TrigL2MuonSA__MuFastPatternFinder()
        self.StationFitter     = theStationFitter
        self.TrackFitter       = MuonSA.TrigL2MuonSA__MuFastTrackFitter()
        self.TrackExtrapolator = MuonSA.TrigL2MuonSA__MuFastTrackExtrapolator()

        self.R_WIDTH_TGC_FAILED = 200
        self.R_WIDTH_RPC_FAILED = 400

        self.DoCalibrationStream = False

        self.USE_ROIBASEDACCESS_CSC = True

        #################################
        # Temporally RoI based data access is disabled for NSW
        self.USE_ROIBASEDACCESS_STGC = False
        self.USE_ROIBASEDACCESS_MM = False
        #################################

        self.RpcErrToDebugStream = True

        MuonSA.MuFastSteering.topoRoad = True
        MuonSA.MuFastSteering.dEtasurrRoI = 0.14 # wide enough comparing to L1 RoI in barrel
        MuonSA.MuFastSteering.dPhisurrRoI = 0.14 # wide enough comparing to L1 RoI in barrel

        if TriggerFlags.run2Config=='2016':
          self.UseEndcapInnerFromBarrel = False
        else:
          self.UseEndcapInnerFromBarrel = True

        if ( args[0]== '900GeV' ):
            self.WinPt = 4.0
            self.Scale_Road_BarrelInner  = 3
            self.Scale_Road_BarrelMiddle = 3
            self.Scale_Road_BarrelOuter  = 3
        else:
            self.WinPt = 6.0
            self.Scale_Road_BarrelInner  = 1
            self.Scale_Road_BarrelMiddle = 1
            self.Scale_Road_BarrelOuter  = 1

        if ( args[0]== 'MuonCalib' ):
            self.DoCalibrationStream = True
            self.MuonCalDataScouting = False
            self.MuonCalBufferSize   = 1024*1024

        if ( args[0]== 'MuonCalibDataScouting' ):
            self.DoCalibrationStream = True
            self.MuonCalDataScouting = True
            self.MuonCalBufferSize   = 1024*1024

        self.Timing = False

        # Default backextrapolator is for MC Misaligned Detector
        self.BackExtrapolator = MuonBackExtrapolatorForMisalignedDet()

        # Setup MonTool for monitored variables in AthenaMonitoring package
        # defined which histogram are created at TrigL2MuonSAMonitoring.py
        self.MonTool = TrigL2MuonSAMonitoring()

        def setDefaults(cls,handle):
            if hasattr(handle,'BackExtrapolator'):
                if handle.BackExtrapolator.name().find("AlignedBackExtrapolator")!=-1:
                    log.info("using BackExtrapolatorLUT for Aligned Detector")
                if handle.BackExtrapolator.name().find("MisalignedBackExtrapolator")!=-1:
                    log.info("using BackExtrapolatorLUT for Misligned Detector")
                if handle.BackExtrapolator.name().find("DataBackExtrapolator")!=-1:
                    log.info("using BackExtrapolatorLUT for Data")
                    
        if TriggerFlags.run2Config=='2016':
            self.StationFitter.PtFromAlphaBeta.useCscPt = False
            self.StationFitter.PtFromAlphaBeta.AvoidMisalignedCSCs = True
        else:
            self.StationFitter.PtFromAlphaBeta.useCscPt = True
            self.StationFitter.PtFromAlphaBeta.AvoidMisalignedCSCs = False


class TrigL2MuonSAConfig(MuonSA.MuFastSteering):

    __slot__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s' % (cls.getType(),args[0]) ] + list(args)
        return super( TrigL2MuonSAConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigL2MuonSAConfig, self ).__init__( name )

        self.DataPreparator    = theDataPreparator
        self.PatternFinder     = MuonSA.TrigL2MuonSA__MuFastPatternFinder()
        self.StationFitter     = theStationFitter
        self.TrackFitter       = MuonSA.TrigL2MuonSA__MuFastTrackFitter()
        self.TrackExtrapolator = MuonSA.TrigL2MuonSA__MuFastTrackExtrapolator()

        import MuonCnvExample.MuonCablingConfig # noqa: F401 configuration of Run 2 cabling by import
        self.DataPreparator.MDTDataPreparator.RegSel_MDT = makeRegSelTool_MDT()
        self.DataPreparator.RPCDataPreparator.RegSel_RPC = makeRegSelTool_RPC()
        self.DataPreparator.TGCDataPreparator.RegSel_TGC = makeRegSelTool_TGC()
        self.DataPreparator.CSCDataPreparator.RegSel_CSC = makeRegSelTool_CSC()

        self.R_WIDTH_TGC_FAILED = 200
        self.R_WIDTH_RPC_FAILED = 400

        self.DoCalibrationStream = False

        self.USE_ROIBASEDACCESS_CSC = True

        self.RpcErrToDebugStream = True

        MuonSA.MuFastSteering.topoRoad = True
        MuonSA.MuFastSteering.dEtasurrRoI = 0.14
        MuonSA.MuFastSteering.dPhisurrRoI = 0.14

        if TriggerFlags.run2Config=='2016':
          self.UseEndcapInnerFromBarrel = False
        else:
          self.UseEndcapInnerFromBarrel = True

        if ( args[0]== '900GeV' ):
            self.WinPt = 4.0
            self.Scale_Road_BarrelInner  = 3
            self.Scale_Road_BarrelMiddle = 3
            self.Scale_Road_BarrelOuter  = 3
        else:
            self.WinPt = 6.0
            self.Scale_Road_BarrelInner  = 1
            self.Scale_Road_BarrelMiddle = 1
            self.Scale_Road_BarrelOuter  = 1

        if ( args[0]== 'MuonCalib' ):
            self.DoCalibrationStream = True
            self.MuonCalDataScouting = False
            self.MuonCalBufferSize   = 1024*1024

        if ( args[0]== 'MuonCalibDataScouting' ):
            self.DoCalibrationStream = True
            self.MuonCalDataScouting = True
            self.MuonCalBufferSize   = 1024*1024

        self.Timing = False

        # Default backextrapolator is for MC Misaligned Detector
        self.BackExtrapolator = MuonBackExtrapolatorForMisalignedDet()

        # Monitoring system used by Run-2
        # Histograms for monitored variables
        #from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        #validation = TrigL2MuonSAValidationMonitoring()
        #online     = TrigL2MuonSAOnlineMonitoring()
        #cosmic     = TrigL2MuonSACosmicMonitoring()
        #time       = TrigTimeHistToolConfig('TimeHisto')

        #self.AthenaMonTools = [ validation, online, cosmic, time ]
        self.AthenaMonTools = [ ]


        def setDefaults(cls,handle):
            if hasattr(handle,'BackExtrapolator'):
                if handle.BackExtrapolator.name().find("AlignedBackExtrapolator")!=-1:
                    log.info("using BackExtrapolatorLUT for Aligned Detector")
                if handle.BackExtrapolator.name().find("MisalignedBackExtrapolator")!=-1:
                    log.info("using BackExtrapolatorLUT for Misligned Detector")
                if handle.BackExtrapolator.name().find("DataBackExtrapolator")!=-1:
                    log.info("using BackExtrapolatorLUT for Data")
                    
        if TriggerFlags.run2Config=='2016':
            self.StationFitter.PtFromAlphaBeta.useCscPt = False
            self.StationFitter.PtFromAlphaBeta.AvoidMisalignedCSCs = True
        else:
            self.StationFitter.PtFromAlphaBeta.useCscPt = True
            self.StationFitter.PtFromAlphaBeta.AvoidMisalignedCSCs = True

