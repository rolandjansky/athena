# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigL2MuonSA.TrigL2MuonSAConf import *
from TrigL2MuonSA.TrigL2MuonSAMonitoring import *
from AthenaCommon.AppMgr import *
from AthenaCommon.AppMgr import ServiceMgr
from TrigTimeMonitor.TrigTimeHistToolConfig import *
from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
from TGCgeometry.TGCgeometryConfig import TGCgeometryConfig
from TrigMuonBackExtrapolator.TrigMuonBackExtrapolatorConfig import *

theDataPreparator    = TrigL2MuonSA__MuFastDataPreparator()
thePatternFinder     = TrigL2MuonSA__MuFastPatternFinder()
theStationFitter     = TrigL2MuonSA__MuFastStationFitter()
theTrackFitter       = TrigL2MuonSA__MuFastTrackFitter()
theTrackExtrapolator = TrigL2MuonSA__MuFastTrackExtrapolator()

from AthenaCommon.AppMgr import ToolSvc

ToolSvc += theDataPreparator
ToolSvc += thePatternFinder
ToolSvc += theStationFitter
ToolSvc += theTrackFitter
ToolSvc += theTrackExtrapolator

ToolSvc += MuonBackExtrapolatorForAlignedDet()
ToolSvc += MuonBackExtrapolatorForMisalignedDet()
ToolSvc += MuonBackExtrapolatorForData()

class PtBarrelLUTSvc(TrigL2MuonSA__PtBarrelLUTSvc):
    def __init__(self,name = 'PtBarrelLUTSvc'):
        super(PtBarrelLUTSvc ,self).__init__(name)
        self.LUTfile = "pt_barrel.lut"
        self.SP_LUTfile = "pt_barrelSP.lut"

class PtBarrelLUTSvc_MC(TrigL2MuonSA__PtBarrelLUTSvc):
    def __init__(self,name = 'PtBarrelLUTSvc_MC'):
        super(PtBarrelLUTSvc_MC ,self).__init__(name)
        self.LUTfile = "pt_barrel.mc10.lut"

class PtEndcapLUTSvc(TrigL2MuonSA__PtEndcapLUTSvc):
    def __init__(self,name = 'PtEndcapLUTSvc'):
        super(PtEndcapLUTSvc ,self).__init__(name)
        self.FileName = "pt_endcap.lut"

class PtEndcapLUTSvc_MC(TrigL2MuonSA__PtEndcapLUTSvc):                            
    def __init__(self,name = 'PtEndcapLUTSvc_MC'):
        super(PtEndcapLUTSvc_MC ,self).__init__(name)
        self.FileName = "pt_endcap.mc10.lut"

class AlignmentBarrelLUTSvc(TrigL2MuonSA__AlignmentBarrelLUTSvc):
    def __init__(self,name = 'AlignmentBarrelLUTSvc'):
        super(AlignmentBarrelLUTSvc ,self).__init__(name)
        self.LUTfile = "dZ_barrel.lut"

# declare the needed services
ServiceMgr += PtEndcapLUTSvc()
ServiceMgr += PtEndcapLUTSvc_MC()
ServiceMgr += PtBarrelLUTSvc()
ServiceMgr += PtBarrelLUTSvc_MC()
ServiceMgr += AlignmentBarrelLUTSvc()

class TrigL2MuonSAConfig(MuFastSteering):

    __slot__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s' % (cls.getType(),args[0]) ] + list(args)
        return super( TrigL2MuonSAConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigL2MuonSAConfig, self ).__init__( name )

        self.DataPreparator    = theDataPreparator
        self.PatternFinder     = thePatternFinder
        self.StationFitter     = theStationFitter
        self.TrackFitter       = theTrackFitter
        self.TrackExtrapolator = theTrackExtrapolator

        self.R_WIDTH_TGC_FAILED = 200
        self.R_WIDTH_RPC_FAILED = 30

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

        # Default backextrapolator is for MC Misaligned Detector
        self.BackExtrapolator = MuonBackExtrapolatorForMisalignedDet()

        # adding Geometry Services
        from AthenaCommon.AppMgr import ServiceMgr
        
        tgc_geo = TGCgeometryConfig()
        self.TGC_GeometrySvc = tgc_geo
        if tgc_geo not in ServiceMgr:  ServiceMgr += tgc_geo
        
        # Histograms for monitored variables
        validation = TrigL2MuonSAValidationMonitoring()
        online     = TrigL2MuonSAOnlineMonitoring()
        cosmic     = TrigL2MuonSACosmicMonitoring()

        time       = TrigTimeHistToolConfig('TimeHisto')

        self.AthenaMonTools = [ validation, online, cosmic, time ]

        def setDefaults(cls,handle):
            if hasattr(handle,'BackExtrapolator'):
                if handle.BackExtrapolator.name().find("AlignedBackExtrapolator")!=-1:
                    print self.name," using BackExtrapolatorLUT for Aligned Detector"
                if handle.BackExtrapolator.name().find("MisalignedBackExtrapolator")!=-1:
                    print self.name," using BackExtrapolatorLUT for Misligned Detector"
                if handle.BackExtrapolator.name().find("DataBackExtrapolator")!=-1:
                    print self.name," using BackExtrapolatorLUT for Data"
