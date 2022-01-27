# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import TrigL2MuonSA.TrigL2MuonSAConf as MuonSA
from TrigL2MuonSA.TrigL2MuonSAMonitoring import TrigL2MuonSAMonitoring
from AthenaCommon.AppMgr import ServiceMgr,ToolSvc
from TrigMuonBackExtrapolator.TrigMuonBackExtrapolatorConfig import MuonBackExtrapolatorForAlignedDet, MuonBackExtrapolatorForMisalignedDet,  MuonBackExtrapolatorForData
from AthenaConfiguration.AllConfigFlags import ConfigFlags

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigL2MuonSAConfig')

theStationFitter     = MuonSA.TrigL2MuonSA__MuFastStationFitter(PtFromAlphaBeta = MuonSA.TrigL2MuonSA__PtFromAlphaBeta())
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
if not MuonGeometryFlags.hasSTGC() and not MuonGeometryFlags.hasMM():
    theStationFitter.NswStationFitter=""

from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConf import LVL1__TrigT1RPCRecRoiTool
trigRpcRoiTool = LVL1__TrigT1RPCRecRoiTool("RPCRecRoiTool", UseRun3Config=ConfigFlags.Trigger.enableL1MuonPhase1)

theDataPreparator    = MuonSA.TrigL2MuonSA__MuFastDataPreparator(TrigT1RPCRecRoiTool = trigRpcRoiTool)
theDataPreparator.RPCDataPreparator  = MuonSA.TrigL2MuonSA__RpcDataPreparator( TrigT1RPCRecRoiTool = trigRpcRoiTool)
theDataPreparator.MDTDataPreparator  = MuonSA.TrigL2MuonSA__MdtDataPreparator()
theDataPreparator.TGCDataPreparator  = MuonSA.TrigL2MuonSA__TgcDataPreparator()
theDataPreparator.RpcRoadDefiner     = MuonSA.TrigL2MuonSA__RpcRoadDefiner()
theDataPreparator.TgcRoadDefiner     = MuonSA.TrigL2MuonSA__TgcRoadDefiner()

if MuonGeometryFlags.hasSTGC():
    theDataPreparator.STGCDataPreparator = MuonSA.TrigL2MuonSA__StgcDataPreparator()
else:
    theDataPreparator.STGCDataPreparator=""
if MuonGeometryFlags.hasMM():
    theDataPreparator.MMDataPreparator   = MuonSA.TrigL2MuonSA__MmDataPreparator()
else:
    theDataPreparator.MMDataPreparator=""
if MuonGeometryFlags.hasCSC():
    theDataPreparator.CSCDataPreparator  = MuonSA.TrigL2MuonSA__CscDataPreparator()
else:
    theDataPreparator.CSCDataPreparator.CSCPrepDataContainer  = ""

theDataPreparator.RPCDataPreparator.RpcClusterPreparator = MuonSA.TrigL2MuonSA__RpcClusterPreparator(TrigT1RPCRecRoiTool = trigRpcRoiTool)

from RegionSelector.RegSelToolConfig import makeRegSelTool_MDT
theDataPreparator.MDTDataPreparator.RegSel_MDT = makeRegSelTool_MDT()
from RegionSelector.RegSelToolConfig import makeRegSelTool_RPC
theDataPreparator.RPCDataPreparator.RegSel_RPC = makeRegSelTool_RPC()
theDataPreparator.RpcRoadDefiner.RegionSelectionTool = makeRegSelTool_MDT()
theDataPreparator.TgcRoadDefiner.RegionSelectionTool = makeRegSelTool_MDT()
theDataPreparator.ClusterRoadDefiner.RegionSelectionTool = makeRegSelTool_MDT()
if MuonGeometryFlags.hasSTGC():
    from RegionSelector.RegSelToolConfig import makeRegSelTool_sTGC
    theDataPreparator.STGCDataPreparator.RegSel_STGC = makeRegSelTool_sTGC()
if MuonGeometryFlags.hasMM():
    from RegionSelector.RegSelToolConfig import makeRegSelTool_MM
    theDataPreparator.MMDataPreparator.RegSel_MM = makeRegSelTool_MM()

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
        if MuonGeometryFlags.hasSTGC() or MuonGeometryFlags.hasMM():
          self.UseRun3LUT = True
        else:
          self.UseRun3LUT = False

class PtEndcapLUTSvc_MC(MuonSA.TrigL2MuonSA__PtEndcapLUTSvc):
    def __init__(self,name = 'PtEndcapLUTSvc_MC'):
        super(PtEndcapLUTSvc_MC ,self).__init__(name)
        self.FileName = "pt_endcap.mc10.lut"
        self.EMeanLUT = "pt_comb_mean.lut"
        self.ESigmaLUT = "pt_comb_sigma.lut"
        if MuonGeometryFlags.hasSTGC() or MuonGeometryFlags.hasMM():
          self.UseRun3LUT = True
        else:
          self.UseRun3LUT = False

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
class TrigL2MuonSAConfig(MuonSA.MuFastSteering):

    __slot__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s' % (cls.getType(),args[0]) ] + list(args)
        return super( TrigL2MuonSAConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigL2MuonSAConfig, self ).__init__( name )

        self.DataPreparator    = theDataPreparator
        from MuonCnvExample.MuonCalibConfig import MdtCalibrationTool
        self.PatternFinder     = MuonSA.TrigL2MuonSA__MuFastPatternFinder(CalibrationTool=MdtCalibrationTool())
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

        self.StationFitter.PtFromAlphaBeta.useCscPt = True
        self.StationFitter.PtFromAlphaBeta.AvoidMisalignedCSCs = False
