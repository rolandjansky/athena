# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigDiMuon.TrigDiMuonConf import *
from TrigDiMuon.TrigDiMuonMonitoring import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr
from TGCgeometry.TGCgeometryConfig import *

#from AthenaCommon.AppMgr import ToolSvc
## extrapolator
#TrigDiMuonExtrapolator = TrigDiMuonExtrapolator()
#TrigDiMuonExtrapolator.OutputLevel = 2
#ToolSvc += TrigDiMuonExtrapolator

class TrigDiMuon_RoI (TrigDiMuonFast):

    __slots__ = []

    def __init__( self, name = "TrigDiMuon_RoI" ):
        super( TrigDiMuon_RoI, self ).__init__( name )

        # adding Geometry Services
        from AthenaCommon.AppMgr import ServiceMgr

        tgc_geo = TGCgeometryConfig()
        self.TGC_Geometry = tgc_geo
        if tgc_geo not in ServiceMgr:  ServiceMgr += tgc_geo

        # monitoring
        validation = TrigDiMuonValidationMonitoring_RoI()
        online     = TrigDiMuonOnlineMonitoring_RoI()

        self.AthenaMonTools = [ validation, online ]

        # Disable Tag-and-Probe
        self.doTagAndProbe = False

        #############################################
        ## TrigDiMuonExtrapolator calibration
        #############################################
        from TrigDiMuonExtrapolatorConfig import TrigDiMuonExtrapolator_calib01
        extrapolator = TrigDiMuonExtrapolator_calib01()
        if extrapolator not in ServiceMgr.ToolSvc: ServiceMgr.ToolSvc += extrapolator

        self.Extrapolator = extrapolator
        print extrapolator



class TrigDiMuon_FS (TrigDiMuonFast):

    __slots__ = []

    def __init__( self, name = "TrigDiMuon_FS" ):
        super( TrigDiMuon_FS, self ).__init__( name )

        # adding Geometry Services
        from AthenaCommon.AppMgr import ServiceMgr

        tgc_geo = TGCgeometryConfig()
        self.TGC_Geometry = tgc_geo
        if tgc_geo not in ServiceMgr:  ServiceMgr += tgc_geo

        # monitoring
        validation = TrigDiMuonValidationMonitoring_FS()
        online     = TrigDiMuonOnlineMonitoring_FS()

        self.AthenaMonTools = [ validation, online ]

        # Disable Tag-and-Probe
        self.doTagAndProbe = False

        # Disable RoI retrieval
        self.retrieveRoI = False

        #############################################
        ## TrigDiMuonExtrapolator calibration
        #############################################
        from TrigDiMuonExtrapolatorConfig import TrigDiMuonExtrapolator_calib01
        extrapolator = TrigDiMuonExtrapolator_calib01()
        if extrapolator not in ServiceMgr.ToolSvc: ServiceMgr.ToolSvc += extrapolator

        self.Extrapolator = extrapolator
        print extrapolator
