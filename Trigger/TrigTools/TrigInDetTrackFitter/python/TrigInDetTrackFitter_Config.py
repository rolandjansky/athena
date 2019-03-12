# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigInDetCombinedTrackFitter
from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigL2HighPtTrackFitter
from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigL2LowPtTrackFitter
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags


from TrkExTools.TrkExToolsConf import Trk__Extrapolator

class ConfiguredTrigL2_Extrapolator(Trk__Extrapolator) :
    __slots__ = []
    def __init__(self, name = 'ConfiguredTrigL2_Extrapolator') :
        Trk__Extrapolator.__init__(self, name)
        from AthenaCommon.AppMgr import ToolSvc
        from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

        from InDetRecExample.TrackingCommon import createAndAddCondAlg, getRIO_OnTrackErrorScalingCondAlg
        createAndAddCondAlg(getRIO_OnTrackErrorScalingCondAlg,'RIO_OnTrackErrorScalingCondAlg')
        
        from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
        TrigL2_StepPropagator = Trk__STEP_Propagator(name = 'TrigL2_StepPropagator')
        ToolSvc += TrigL2_StepPropagator
        from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator
        TrigL2_RKPropagator = Trk__RungeKuttaPropagator(name = 'TrigL2_RKPropagator')
        ToolSvc += TrigL2_RKPropagator
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        if InDetTrigFlags.propagatorType() is "STEP":
            TrigL2_Propagator = TrigL2_StepPropagator
        else:
            TrigL2_Propagator = TrigL2_RKPropagator
        
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        AtlasTrackingGeometrySvc  = svcMgr.AtlasTrackingGeometrySvc
        from TrkExTools.TrkExToolsConf import Trk__Navigator
        TrigL2_Navigator = Trk__Navigator(name = 'TrigL2_Navigator',TrackingGeometrySvc = AtlasTrackingGeometrySvc)
        ToolSvc += TrigL2_Navigator
        from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
        TrigL2_MaterialUpdator = Trk__MaterialEffectsUpdator(name = "TrigL2_MaterialEffectsUpdator")
        ToolSvc += TrigL2_MaterialUpdator
        TrigL2_SubPropagators = []
        TrigL2_SubUpdators = []
        TrigL2_SubPropagators += [ TrigL2_Propagator.name() ]
        TrigL2_SubUpdators    += [ TrigL2_MaterialUpdator.name() ]
        TrigL2_SubPropagators += [ TrigL2_Propagator.name() ]
        TrigL2_SubUpdators    += [ TrigL2_MaterialUpdator.name() ]
        TrigL2_SubPropagators += [ TrigL2_StepPropagator.name() ]
        TrigL2_SubUpdators    += [ TrigL2_MaterialUpdator.name() ]
        self.Propagators             = [ TrigL2_RKPropagator, TrigL2_StepPropagator]
        self.MaterialEffectsUpdators = [ TrigL2_MaterialUpdator ]
        self.Navigator               = TrigL2_Navigator
        self.SubPropagators          = TrigL2_SubPropagators
        self.SubMEUpdators           = TrigL2_SubUpdators
        #self.DoCaloDynamic           = False #Obsolete




from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator

class ConfiguredTrigL2_InDetRotCreator(Trk__RIO_OnTrackCreator) :
    __slots__ = []
    def __init__(self, name = 'ConfiguredTrigL2_InDetRotCreator') :
        Trk__RIO_OnTrackCreator.__init__(self,name)
        from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
        from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__PixelClusterOnTrackTool
        from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        
        # SiLorentzAngleTool
        from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
        sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()

        if InDetTrigFlags.doCommissioning() :
            myL2_SCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool("TrigL2_SCT_ClusterOnTrackTool",
                                                                        CorrectionStrategy = 0,
                                                                        ErrorStrategy      = 0,
                                                                        LorentzAngleTool   = sctLorentzAngleToolSetup.SCTLorentzAngleTool)
            myL2_PixelClusterOnTrackTool = InDet__PixelClusterOnTrackTool("TrigL2_PixelClusterOnTrackTool",
                                                                          ErrorStrategy = 0,
                                                                          LorentzAngleTool = ToolSvc.InDetTrigPixelLorentzAngleTool)

        else:
            myL2_SCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool("TrigL2_SCT_ClusterOnTrackTool",
                                                                        CorrectionStrategy = 0,
                                                                        ErrorStrategy      = 2,
                                                                        LorentzAngleTool   = sctLorentzAngleToolSetup.SCTLorentzAngleTool)
            myL2_PixelClusterOnTrackTool = InDet__PixelClusterOnTrackTool("TrigL2_PixelClusterOnTrackTool",
                                                                          ErrorStrategy = 1,
                                                                          LorentzAngleTool = ToolSvc.InDetTrigPixelLorentzAngleTool)

                    
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += myL2_PixelClusterOnTrackTool
        ToolSvc += myL2_SCT_ClusterOnTrackTool        
        self.Mode='indet'
        self.ToolPixelCluster = myL2_PixelClusterOnTrackTool
        self.ToolSCT_Cluster = myL2_SCT_ClusterOnTrackTool




class ConfiguredTrigL2LowPtTrackFitter(TrigL2LowPtTrackFitter) :
    __slots__ = []
    def __init__(self, name = 'ConfiguredTrigL2LowPtTrackFitter') :
        TrigL2LowPtTrackFitter.__init__(self,name)
        from AthenaCommon.AppMgr import ToolSvc
        offlineExtrapolator = ConfiguredTrigL2_Extrapolator()
        ToolSvc += offlineExtrapolator
        offlineRotCreator = ConfiguredTrigL2_InDetRotCreator()
        ToolSvc += offlineRotCreator
        self.useROTs=False
        self.ROTcreator=offlineRotCreator
        self.TrackExtrapolatorTool=offlineExtrapolator


        

class ConfiguredTrigL2_TrackFitter(TrigInDetCombinedTrackFitter) :
    __slots__ = []
    def __init__(self, name = "ConfiguredTrigL2_TrackFitter") :
        TrigInDetCombinedTrackFitter.__init__(self, name)
        from AthenaCommon.AppMgr import ToolSvc
                
        offlineRotCreator = ConfiguredTrigL2_InDetRotCreator()
        ToolSvc += offlineRotCreator

        trigL2HighPtTrackFitter = TrigL2HighPtTrackFitter(name='TrigL2HighPtTrackFitter',
                                                                    useROTs=False,
                                                                    ROTcreator=offlineRotCreator)
        trigL2LowPtTrackFitter = ConfiguredTrigL2LowPtTrackFitter()
        
        ToolSvc += trigL2HighPtTrackFitter
        ToolSvc += trigL2LowPtTrackFitter
        
        self.HighPtTrackFitter = trigL2HighPtTrackFitter
        self.LowPtTrackFitter = trigL2LowPtTrackFitter







