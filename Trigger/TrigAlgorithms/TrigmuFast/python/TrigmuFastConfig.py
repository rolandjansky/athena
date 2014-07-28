# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigmuFast.TrigmuFastConf import *
from TrigmuFast.TrigmuFastMonitoring import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
from TGCgeometry.TGCgeometryConfig import TGCgeometryConfig
from TrigMuonBackExtrapolator.TrigMuonBackExtrapolatorConfig import *

from AthenaCommon.AppMgr import ToolSvc

ToolSvc += MuonBackExtrapolatorForAlignedDet()
ToolSvc += MuonBackExtrapolatorForMisalignedDet()


class MuRoadsSvc_Loose(MuRoadsSvc):
    
    def __init__(self,name = 'MuRoadsSvc_900GeV'):
        super(MuRoadsSvc_Loose ,self).__init__(name)

        self.ScaleBarrelMiddleRoads = 3
        self.ScaleBarrelInnerRoads = 3
        self.ScaleBarrelOuterRoads = 3

class MuLUTSvc_MC(MuLUTSvc):

    def __init__(self,name = 'MuLUTSvc_MC'):
        super(MuLUTSvc_MC ,self).__init__(name)

        self.LUTfile = "lut_rad.mc10.data"

class PtLUTSvc_MC(PtLUTSvc):

    def __init__(self,name = 'PtLUTSvc_MC'):
        super(PtLUTSvc_MC ,self).__init__(name)

        self.FileName = "TGCpt.mc10.lut"

class PtLUTSvc_pol2(PtLUTSvc):

    def __init__(self,name = 'PtLUTSvc_pol2'):
        super(PtLUTSvc_pol2 ,self).__init__(name)

        self.FileName = "TGCpt_pol2.lut"

class PtLUTSvc_pol2_MC(PtLUTSvc):

    def __init__(self,name = 'PtLUTSvc_pol2_MC'):
        super(PtLUTSvc_pol2_MC ,self).__init__(name)

        self.FileName = "TGCpt_pol2.mc10.lut"


# declare the needed services
ServiceMgr += PtLUTSvc()
ServiceMgr += PtLUTSvc_pol2()
ServiceMgr += PtLUTSvc_MC()
ServiceMgr += PtLUTSvc_pol2_MC()
ServiceMgr += MuLUTSvc()
ServiceMgr += MuLUTSvc_MC()
ServiceMgr += MuRoadsSvc()
ServiceMgr += MuRoadsSvc_Loose()


class TrigmuFastConfig (muFast):

    __slots__ = []
    
    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s' % (cls.getType(),args[0]) ] + list(args)
        return super( TrigmuFastConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigmuFastConfig, self ).__init__( name )

        self.histoPathBase = ""

        self.UseCalibration = True
        if ( muonByteStreamFlags.RpcDataType()=='atlas' ):
            self.ROBmapperFile = "AtlasRoiROBmap.data"
            self.MaskUncCMAch = False
            self.BackExtrapolator = MuonBackExtrapolatorForMisalignedDet()

        if ( args[0] == 'Cosmic' ):
            self.CosmicRun = True
        
        if ( args[0] == 'HALO' ):
            self.CosmicRun = True

        if ( args[0] == 'Mcal' ):
            self.DoMuonCalStream = True

        if ( args[0].find("OutBCID") != -1 ):
            self.CreateRoIFromDescriptor = True

        if (not globalflags.InputFormat.is_bytestream()):
            self.DetMaskCheck = False

        if athenaCommonFlags.isOnline():
            self.OnlineRun = True

        # adding Geometry Services
        from AthenaCommon.AppMgr import ServiceMgr
        
        tgc_geo = TGCgeometryConfig()
        self.TGC_Geometry = tgc_geo
        if tgc_geo not in ServiceMgr:  ServiceMgr += tgc_geo


        validation = TrigmuFastValidationMonitoring()
        online     = TrigmuFastOnlineMonitoring()
        cosmic     = TrigmuFastCosmicMonitoring()

        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 5]
	#time.NumberOfHistBins = 50
	
        self.AthenaMonTools = [ validation, online, cosmic, time ]

        if ( globalflags.DetDescrVersion().startswith('DC1') or globalflags.DetDescrVersion().startswith('DC2') ):
            self.ROBmapperFile = "RoiROBmap-P03.data"
            self.BackExtrapolator = MuonBackExtrapolatorForAlignedDet()

        fieldty=globalflags.DetDescrVersion()[13:15].strip()
        #if fieldty!="00":
        self.UseTiltedBField=True

        if ( globalflags.DetDescrVersion().find("CSC")!=-1 ):
            self.BackExtrapolator = MuonBackExtrapolatorForAlignedDet()

# leave it in TriggrRelease/Modifier.py
#        if globalflags.DataSource.is_data():
#            self.RpcTimeDelay=-88
#        else:
#            self.UseLUTForMC=True

        def setDefaults(cls,handle):
            if hasattr(handle,'BackExtrapolator'):
                if handle.BackExtrapolator.name().find("AlignedBackExtrapolator")!=-1:
                    print self.name," using BackExtrapolator for Aligned Detector"
                if handle.BackExtrapolator.name().find("MisalignedBackExtrapolator")!=-1:
                    print self.name," using BackExtrapolator for Misligned Detector" 
