# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMuGirl.TrigMuGirlConf import TrigMuGirl,TrigMuGirlTool
from TrigMuGirl.TrigMuGirlFlags import trigMuGirlFlags 
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr
from AthenaCommon.SystemOfUnits import GeV

#added MuGirl 2008-03-4
#from TrigMuGirl.TrigMuGirlFlags import TrigmuGirlFlags
from RecExConfig.RecFlags import rec
from AthenaCommon.Configurable import *

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.CfgGetter import getPublicTool

#from AthenaCommon.AppMgr import ServiceMgr

#from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyParam
#MuidCaloEnergyParam = Rec__MuidCaloEnergyParam(name = 'MuidCaloEnergyParam')
#ToolSvc += MuidCaloEnergyParam

#from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyTool
#MuidCaloEnergyToolParam = Rec__MuidCaloEnergyTool(name = 'MuidCaloEnergyToolParam',
#                                                      CaloParamTool         = MuidCaloEnergyParam,
#                                                      EnergyLossMeasurement = False)
#ToolSvc += MuidCaloEnergyToolParam
        # the default instance depends on the calorimeter and indet DetFlags

#from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyMeas
#MuidCaloEnergyMeas = Rec__MuidCaloEnergyMeas(name = 'MuidCaloEnergyMeas',
#                                                 CaloParamTool = MuidCaloEnergyParam)
#ToolSvc += MuidCaloEnergyMeas
#ToolSvc += Rec__MuidCaloEnergyTool(name = 'MuidCaloEnergyTool',
#                                       CaloMeasTool  = MuidCaloEnergyMeas,
#                                       CaloParamTool = MuidCaloEnergyParam)
#
#from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyParam
#MuidCaloEnergyParam = Rec__MuidCaloEnergyParam(name = 'MuidCaloEnergyParam')
#ToolSvc += MuidCaloEnergyParam

#from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyTool
#MuidCaloEnergyToolParam = Rec__MuidCaloEnergyTool(name = 'MuidCaloEnergyToolParam',
#                                                      CaloParamTool         = MuidCaloEnergyParam,
#                                                      EnergyLossMeasurement = False)
#ToolSvc += MuidCaloEnergyToolParam
## the default instance depends on the calorimeter and indet DetFlags
#from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyMeas
#MuidCaloEnergyMeas = Rec__MuidCaloEnergyMeas(name = 'MuidCaloEnergyMeas',
#                                                 CaloParamTool = MuidCaloEnergyParam)
#ToolSvc += MuidCaloEnergyMeas
#MuidCaloEnergyTool = Rec__MuidCaloEnergyTool(name = 'MuidCaloEnergyTool',
#                                   CaloMeasTool  = MuidCaloEnergyMeas,
#                                   CaloParamTool = MuidCaloEnergyParam)
#ToolSvc += MuidCaloEnergyTool

# set up the extrapolator and fitter as used by Muid
from AthenaCommon.AppMgr import ServiceMgr

from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingVolumesSvc
ServiceMgr += Trk__TrackingVolumesSvc(BuildVolumesFromTagInfo = False)

from MuonRecExample import MuonRecTools
MuonExtrapolator              = MuonRecTools.getPublicTool('MuonExtrapolator')
MuonStraightLineExtrapolator  = MuonRecTools.getPublicTool('MuonStraightLineExtrapolator')
#from MuonTrackFinderTools.MuonTrackFinderToolsConf import Muon__MuonChamberHoleRecoveryTool
#MuonHoleRecovery = Muon__MuonChamberHoleRecoveryTool("MuonHoleRecovery")
#ToolSvc +=MuonHoleRecovery
#MuonHoleRecovery              = MuonRecTools.getPublicTool('MuonChamberHoleRecoveryTool') 

#from AthenaCommon.Include import include
#include ('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

from TrkExRungeKuttaIntersector.TrkExRungeKuttaIntersectorConf import \
     Trk__IntersectorWrapper as Propagator
TrigMuGirl_MuidPropagator = Propagator(name = 'TrigMuGirl_MuidPropagator')
ToolSvc += TrigMuGirl_MuidPropagator

from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
TrigMuGirl_MuidMaterialUpdator = Trk__MaterialEffectsUpdator(name = "TrigMuGirl_MuidMaterialEffectsUpdator")
ToolSvc += TrigMuGirl_MuidMaterialUpdator

from TrkExTools.TrkExToolsConf import Trk__Navigator
TrigMuGirl_MuidNavigator = Trk__Navigator(name                = 'TrigMuGirl_MuidNavigator',
                                    TrackingGeometrySvc = ServiceMgr.AtlasTrackingGeometrySvc
                                    )
ToolSvc += TrigMuGirl_MuidNavigator

from TrkExTools.TrkExToolsConf import Trk__Extrapolator
TrigMuGirl_MuidExtrapolator = Trk__Extrapolator( 
    name = 'TrigMuGirl_MuidExtrapolator', 
    Propagators = [ TrigMuGirl_MuidPropagator ],
    MaterialEffectsUpdators = [ TrigMuGirl_MuidMaterialUpdator ],
    Navigator=TrigMuGirl_MuidNavigator)
ToolSvc += TrigMuGirl_MuidExtrapolator

# material allocation 
from TrkiPatFitter.TrkiPatFitterConf import Trk__MaterialAllocator 
TrigMuGirl_MuidMaterialAllocator = Trk__MaterialAllocator( 
    name                      = 'TrigMuGirl_MuidMaterialAllocator', 
    AggregateMaterial         = True, 
    Extrapolator              = TrigMuGirl_MuidExtrapolator, 
    SpectrometerExtrapolator  = MuonExtrapolator, 
    TrackingGeometrySvc       = ServiceMgr.AtlasTrackingGeometrySvc) 
ToolSvc += TrigMuGirl_MuidMaterialAllocator 
 	



from TrkiPatFitter.TrkiPatFitterConf import Trk__iPatFitter
TrigMuGirl_iPatFitter = Trk__iPatFitter(
    name                      = 'TrigMuGirl_iPatFitter',
    AggregateMaterial         = True,
    FullCombinedFit           = True,
    MaterialAllocator         = TrigMuGirl_MuidMaterialAllocator)
    #Extrapolator              = TrigMuGirl_MuidExtrapolator,
    #MuonMaterialExtrapolator  = MuonExtrapolator,
    #TrackingGeometrySvc       = ServiceMgr.AtlasTrackingGeometrySvc)
TrigMuGirl_iPatSLFitter = Trk__iPatFitter(
    name                      = 'TrigMuGirl_iPatSLFitter',
    FullCombinedFit           = True,
    LineFit                   = True,
    MaterialAllocator         = TrigMuGirl_MuidMaterialAllocator)
    #Extrapolator              = MuonStraightLineExtrapolator,
    #MuonMaterialExtrapolator  = MuonStraightLineExtrapolator,
    #TrackingGeometrySvc       = ServiceMgr.AtlasTrackingGeometrySvc)


ToolSvc += TrigMuGirl_iPatFitter
ToolSvc += TrigMuGirl_iPatSLFitter

TrigMuGirl_MuidTrackCleaner = CfgMgr.Muon__MuonTrackCleaner(name = 'TrigMuGirl_MuidTrackCleaner',
                                               PullCut     = 3.0,
                                               PullCutPhi  = 4.0,
                                               Fitter      = TrigMuGirl_iPatFitter,
                                               SLFitter    = TrigMuGirl_iPatSLFitter)
ToolSvc += TrigMuGirl_MuidTrackCleaner




from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
MuonTrackSummaryHelperTool    = MuonRecTools.getPublicTool("MuonTrackSummaryHelper")
#if not hasattr( ToolSvc,' InDetTrackSummaryHelperTool' ):


from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
from AthenaCommon.DetFlags import DetFlags 
if not DetFlags.detdescr.ID_on():
    TrigMuGirl_MuidTrackSummaryTool = Trk__TrackSummaryTool(
        name = "MuidTrackSummaryTool",
        MuonSummaryHelperTool  = MuonTrackSummaryHelperTool,
        doSharedHits           = False)
else: 
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool,  InDetTrigHoleSearchTool
    TrigMuGirl_MuidTrackSummaryTool = Trk__TrackSummaryTool(
        name = "MuidTrackSummaryTool",
        InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool,
        InDetHoleSearchTool = InDetTrigHoleSearchTool,
        MuonSummaryHelperTool  = MuonTrackSummaryHelperTool,
        doSharedHits           = False)
ToolSvc += TrigMuGirl_MuidTrackSummaryTool


from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
TrigMuGirl_MuidParticleCreatorTool = Trk__TrackParticleCreatorTool(
     name = "TrigMuGirl_MuidParticleCreatorTool",
     #Extrapolator     = muidExtrapolator,
     TrackSummaryTool = TrigMuGirl_MuidTrackSummaryTool)
ToolSvc += TrigMuGirl_MuidParticleCreatorTool


#from MuonIdentificationHelper.MuonIdentificationHelperConf  import Rec__MuonIdentificationHelper
#TrigMuGirl_MuonIdentificationHelperTool = Rec__MuonIdentificationHelper (
#     name = "TrigMuGirl_MuonIdentificationHelperTool",
#     ParticleCreatorTool = TrigMuGirl_MuidParticleCreatorTool )
#ToolSvc += TrigMuGirl_MuonIdentificationHelperTool


class TrigMuGirlConfig (TrigMuGirl):
    __slots__ = ()
#
#    def __init__(self, name = "TrigMuGirl"):
#        super( TrigMuGirl, self ).__init__( name )
#    def __new__( cls, *args, **kwargs ):
#        newargs = ['%s_%s' % (cls.getType(),args[0]) ] + list(args)
#        return super( TrigMuGirlConfig, cls ).__new__( cls, *newargs, **kwargs )
#    __slots__ = []
     
#    def __new__( cls, *args, **kwargs ):
#        newargs = ['%s_%s' % (cls.getType(),args[0]) ] + list(args)
#        return super( TrigMuGirlConfig, cls ).__new__( cls, *newargs, **kwargs )
    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s' % (cls.getType(),args[0]) ] + list(args)
        return super( TrigMuGirlConfig, cls ).__new__( cls, *newargs, **kwargs )
 
    def __init__( self, name, *args, **kwargs ):
        super( TrigMuGirlConfig, self ).__init__( name )

        self.doCSC=trigMuGirlFlags.doCSC()

        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon.AppMgr import ServiceMgr

        TrigMuGirlMuonHoleRecovery = CfgMgr.Muon__MuonChamberHoleRecoveryTool("TrigMuGirlMuonHoleRecovery") 
        if not trigMuGirlFlags.doCSC(): 
            TrigMuGirlMuonHoleRecovery.CscRotCreator = None 
        ToolSvc +=TrigMuGirlMuonHoleRecovery 


        from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyParam
        TrigMuGirlCaloEnergyParam = Rec__MuidCaloEnergyParam(name = 'TrigMuGirlCaloEnergyParam')
        ToolSvc += TrigMuGirlCaloEnergyParam

        from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyMeas
        TrigMuGirlCaloEnergyMeas = Rec__MuidCaloEnergyMeas(name = 'TrigMuGirlCaloEnergyMeas',  CaloParamTool = TrigMuGirlCaloEnergyParam, UseCaloNoiseTool = False,CaloNoiseTool='')
        ToolSvc += TrigMuGirlCaloEnergyMeas

        from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyTool
        TrigMuGirlCaloEnergyTool = Rec__MuidCaloEnergyTool(name = 'TrigMuGirlCaloEnergyTool', CaloParamTool=TrigMuGirlCaloEnergyParam,CaloMeasTool=TrigMuGirlCaloEnergyMeas, EnergyLossMeasurement = False,MopParametrization    = True)
        ToolSvc += TrigMuGirlCaloEnergyTool

        from MuidCaloScatteringTools.MuidCaloScatteringToolsConf import Rec__MuidCaloTrackStateOnSurface
        TrigMuGirlCaloTSOS = Rec__MuidCaloTrackStateOnSurface(name = 'TrigMuGirlCaloTSOS', CaloEnergyDeposit=TrigMuGirlCaloEnergyTool, CaloEnergyParam = TrigMuGirlCaloEnergyTool )
        ToolSvc += TrigMuGirlCaloTSOS

        from MuidTrackBuilder.MuidTrackBuilderConf import Rec__CombinedMuonTrackBuilder
        TrigMuGirlTrackBuilder= Rec__CombinedMuonTrackBuilder(name = 'TrigMuGirlTrackBuilder', CaloEnergyParam=TrigMuGirlCaloEnergyTool,CaloTSOS=TrigMuGirlCaloTSOS,ReallocateMaterial=False,Cleaner=TrigMuGirl_MuidTrackCleaner,Fitter=TrigMuGirl_iPatFitter,SLFitter=TrigMuGirl_iPatSLFitter,MuonHoleRecovery=TrigMuGirlMuonHoleRecovery)
        ToolSvc += TrigMuGirlTrackBuilder

        from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilderTool
        TrigMuGirlCscClusterBuilder = CscThresholdClusterBuilderTool("TrigMuGirlCscClusterBuilder")
        TrigMuGirlCscClusterBuilder.threshold = 20000
        TrigMuGirlCscClusterBuilder.strip_fitter = getPublicTool("CalibCscStripFitter")
        TrigMuGirlCscClusterBuilder.default_fitter = getPublicTool("SimpleCscClusterFitter")
        TrigMuGirlCscClusterBuilder.precision_fitter = getPublicTool("QratCscClusterFitter")
        TrigMuGirlCscClusterBuilder.split_fitter = getPublicTool("CscSplitClusterFitter")
        ToolSvc += TrigMuGirlCscClusterBuilder
        #print MuGirlCscClusterBuilder

        from MuGirlCandidate.MuGirlCandidateConfig import MuGirlNS__CandidateToolConfig
        TrigMuGirlCandidateTool = MuGirlNS__CandidateToolConfig("TrigMuGirlCandidateTool")
        ToolSvc += TrigMuGirlCandidateTool
        TrigMuGirlCandidateTool.MuidCaloEnergy= TrigMuGirlCaloEnergyTool
        TrigMuGirlCandidateTool.CscClusterProviderTool = TrigMuGirlCscClusterBuilder
        #TrigMuGirlCandidateTool.OutputLevel =2
        TrigMuGirlCandidateTool.doCSC = self.doCSC 
        TrigMuGirlCandidateTool.writeChamberT0s = False 
        self.MuGirlCandidate = TrigMuGirlCandidateTool

        from MuGirlSelection.MuGirlSelectionConf import MuGirlNS__ANNSelectionTool
        TrigMuGirlSelectionTool = MuGirlNS__ANNSelectionTool("TrigMuGirlSelectionTool")
        ToolSvc += TrigMuGirlSelectionTool
        TrigMuGirlSelectionTool.CandidateTool = TrigMuGirlCandidateTool
        self.ANNSelectionTool = TrigMuGirlSelectionTool

        from MuGirlParticleCreatorTool.MuGirlParticleCreatorToolConf import MuGirlNS__MuGirlParticleCreatorTool
        TrigMuGirlParticleCreatorTool  = MuGirlNS__MuGirlParticleCreatorTool("TrigMuGirlParticleCreatorTool")
        ToolSvc += TrigMuGirlParticleCreatorTool
        TrigMuGirlParticleCreatorTool.MuidCaloEnergyTool= TrigMuGirlCaloEnergyTool
        TrigMuGirlParticleCreatorTool.doFill= False
        self.MuGirlParticleCreatorTool = TrigMuGirlParticleCreatorTool

        from MuGirlGlobalFit.MuGirlGlobalFitConf import MuGirlNS__GlobalFitTool
        TrigMuGirlGlobalFitTool = MuGirlNS__GlobalFitTool("TrigMuGirlGlobalFitTool")
        ToolSvc += TrigMuGirlGlobalFitTool
        TrigMuGirlGlobalFitTool.trackFitter= TrigMuGirlTrackBuilder
        #TrigMuGirlGlobalFitTool.OutputLevel=2

        from MuGirlGlobalFit.MuGirlGlobalFitConf import MuGirlNS__GlobalFitTool
        TrigMuGirlStauGlobalFitTool = MuGirlNS__GlobalFitTool("TrigMuGirlStauGlobalFitTool")
        ToolSvc += TrigMuGirlStauGlobalFitTool
        TrigMuGirlStauGlobalFitTool.trackFitter= TrigMuGirlTrackBuilder
        #TrigMuGirlStauGlobalFitTool.OutputLevel=2

        from MuGirlGlobalFit.MuGirlGlobalFitConf import MuGirlNS__GlobalFitTool
        TrigMuGirlSAGlobalFitTool = MuGirlNS__GlobalFitTool("TrigMuGirlSAGlobalFitTool")
        ToolSvc += TrigMuGirlSAGlobalFitTool
        TrigMuGirlSAGlobalFitTool.trackFitter= TrigMuGirlTrackBuilder
        #TrigMuGirlSAGlobalFitTool.OutputLevel=2

        self.MuGirlGlobalFIT = TrigMuGirlGlobalFitTool
        self.MuGirlGlobalStauFIT = TrigMuGirlStauGlobalFitTool
        self.MuGirlGlobalMuonFeatureFIT = TrigMuGirlSAGlobalFitTool


        from TrigMuGirl.TrigMuGirlMonitoring import TrigMuGirlValidationMonitoring
        validation = TrigMuGirlValidationMonitoring()
        from TrigMuGirl.TrigMuGirlMonitoring import TrigMuGirlOnlineMonitoring
        online = TrigMuGirlOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 1000]
        time.NumberOfHistBins = 1000
        self.AthenaMonTools = [ validation, online, time ]

        #from TrackToCalo.ExtrapolTrackToCaloToolBase import ExtrapolTrackToCaloToolFactory
        #MuGirlExtrapolTrackToCaloTool = ExtrapolTrackToCaloToolFactory(depth="showerdefault")
        #ToolSvc += MuGirlExtrapolTrackToCaloTool
        #self.ExtrapolTrackToCaloTool = MuGirlExtrapolTrackToCaloTool

        
        from MuGirlStau.MuGirlStauConfig import MuGirlNS__StauToolConfig
        TrigMuGirlStauTool = MuGirlNS__StauToolConfig("TrigMuGirlStauTool")
        TrigMuGirlStauTool.StauGlobalFitTool = TrigMuGirlStauGlobalFitTool
        TrigMuGirlStauTool.particleCreatorTool = TrigMuGirl_MuidParticleCreatorTool
        ToolSvc += TrigMuGirlStauTool
        self.MuGirlStauTool = TrigMuGirlStauTool

        if ( args[0] == 'Slow' ):
             self.doMuonFeature = True
             self.doSAFit = True
             self.doStau = True
#        from MuGirlParticleCreatorTool.MuGirlParticleCreatorToolConf import MuGirlNS__MuGirlParticleCreatorTool
#        MuGirlParticleCreatorTool = MuGirlNS__MuGirlParticleCreatorTool(name = 'MuGirlParticleCreatorTool')
##        MuGirlParticleCreatorTool.doNTuple = self.doNTuple
##        MuGirlParticleCreatorTool.doTruth = self.doTruth
##        MuGirlParticleCreatorTool.PerformanceTruthTool = MuGirlPerformanceTruthTool
#        ToolSvc += MuGirlParticleCreatorTool
#        self.MuGirlParticleCreatorTool = MuGirlParticleCreatorTool

#        self.doGlobalFit=True
#TrigMuGirl = TrigMuGirlConfig()

class TrigMuGirlToolConfig (TrigMuGirlTool): 
    __slots__ = ()

    def __init__( self, name="TrigMuGirlTool" ):
        super( TrigMuGirlToolConfig, self ).__init__( name )

        self.doCSC=trigMuGirlFlags.doCSC()

        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon.AppMgr import ServiceMgr

        TrigMuGirlMuonHoleRecovery = CfgMgr.Muon__MuonChamberHoleRecoveryTool("TrigMuGirlMuonHoleRecovery")
        if not trigMuGirlFlags.doCSC(): 
            TrigMuGirlMuonHoleRecovery.CscRotCreator = None
        ToolSvc +=TrigMuGirlMuonHoleRecovery


        from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyParam
        TrigMuGirlCaloEnergyParam = Rec__MuidCaloEnergyParam(name = 'TrigMuGirlCaloEnergyParam')
        ToolSvc += TrigMuGirlCaloEnergyParam

        from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyMeas
        TrigMuGirlCaloEnergyMeas = Rec__MuidCaloEnergyMeas(name = 'TrigMuGirlCaloEnergyMeas',  CaloParamTool = TrigMuGirlCaloEnergyParam, UseCaloNoiseTool = False,CaloNoiseTool='')
        ToolSvc += TrigMuGirlCaloEnergyMeas

        from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyTool
        TrigMuGirlCaloEnergyTool = Rec__MuidCaloEnergyTool(name = 'TrigMuGirlCaloEnergyTool', CaloParamTool=TrigMuGirlCaloEnergyParam,CaloMeasTool=TrigMuGirlCaloEnergyMeas, EnergyLossMeasurement = False,MopParametrization    = True)
        ToolSvc += TrigMuGirlCaloEnergyTool

        from MuidCaloScatteringTools.MuidCaloScatteringToolsConf import Rec__MuidCaloTrackStateOnSurface
        TrigMuGirlCaloTSOS = Rec__MuidCaloTrackStateOnSurface(name = 'TrigMuGirlCaloTSOS', CaloEnergyDeposit=TrigMuGirlCaloEnergyTool, CaloEnergyParam = TrigMuGirlCaloEnergyTool )
        ToolSvc += TrigMuGirlCaloTSOS

        from MuidTrackBuilder.MuidTrackBuilderConf import Rec__CombinedMuonTrackBuilder
        TrigMuGirlTrackBuilder= Rec__CombinedMuonTrackBuilder(name = 'TrigMuGirlTrackBuilder', CaloEnergyParam=TrigMuGirlCaloEnergyTool,CaloTSOS=TrigMuGirlCaloTSOS,ReallocateMaterial=False,Cleaner=TrigMuGirl_MuidTrackCleaner,Fitter=TrigMuGirl_iPatFitter,SLFitter=TrigMuGirl_iPatSLFitter,MuonHoleRecovery=TrigMuGirlMuonHoleRecovery)
        ToolSvc += TrigMuGirlTrackBuilder

        from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilderTool
        TrigMuGirlCscClusterBuilder = CscThresholdClusterBuilderTool("TrigMuGirlCscClusterBuilder")
        TrigMuGirlCscClusterBuilder.threshold = 20000
        TrigMuGirlCscClusterBuilder.strip_fitter = getPublicTool("CalibCscStripFitter")
        TrigMuGirlCscClusterBuilder.default_fitter = getPublicTool("SimpleCscClusterFitter")
        TrigMuGirlCscClusterBuilder.precision_fitter = getPublicTool("QratCscClusterFitter")
        TrigMuGirlCscClusterBuilder.split_fitter = getPublicTool("CscSplitClusterFitter")
        ToolSvc += TrigMuGirlCscClusterBuilder
        #print MuGirlCscClusterBuilder

        from MuGirlCandidate.MuGirlCandidateConfig import MuGirlNS__CandidateToolConfig
        TrigMuGirlCandidateTool = MuGirlNS__CandidateToolConfig("TrigMuGirlCandidateTool")
        ToolSvc += TrigMuGirlCandidateTool
        TrigMuGirlCandidateTool.MuidCaloEnergy= TrigMuGirlCaloEnergyTool
        TrigMuGirlCandidateTool.CscClusterProviderTool = TrigMuGirlCscClusterBuilder
        #TrigMuGirlCandidateTool.OutputLevel =2
        TrigMuGirlCandidateTool.doCSC = self.doCSC
        TrigMuGirlCandidateTool.writeChamberT0s = False
        self.MuGirlCandidate = TrigMuGirlCandidateTool

        from MuGirlSelection.MuGirlSelectionConf import MuGirlNS__ANNSelectionTool
        TrigMuGirlSelectionTool = MuGirlNS__ANNSelectionTool("TrigMuGirlSelectionTool")
        ToolSvc += TrigMuGirlSelectionTool
        TrigMuGirlSelectionTool.CandidateTool = TrigMuGirlCandidateTool
        self.ANNSelectionTool = TrigMuGirlSelectionTool

        from MuGirlParticleCreatorTool.MuGirlParticleCreatorToolConf import MuGirlNS__MuGirlParticleCreatorTool
        TrigMuGirlParticleCreatorTool  = MuGirlNS__MuGirlParticleCreatorTool("TrigMuGirlParticleCreatorTool")
        ToolSvc += TrigMuGirlParticleCreatorTool
        TrigMuGirlParticleCreatorTool.MuidCaloEnergyTool= TrigMuGirlCaloEnergyTool
        TrigMuGirlParticleCreatorTool.doFill= False
        #TrigMuGirlParticleCreatorTool.MuonIdentificationHelper= TrigMuGirl_MuonIdentificationHelperTool
        self.MuGirlParticleCreatorTool = TrigMuGirlParticleCreatorTool

        from MuGirlGlobalFit.MuGirlGlobalFitConf import MuGirlNS__GlobalFitTool
        TrigMuGirlGlobalFitTool = MuGirlNS__GlobalFitTool("TrigMuGirlGlobalFitTool")
        ToolSvc += TrigMuGirlGlobalFitTool
        TrigMuGirlGlobalFitTool.trackFitter= TrigMuGirlTrackBuilder
        #TrigMuGirlGlobalFitTool.OutputLevel=2

        from MuGirlGlobalFit.MuGirlGlobalFitConf import MuGirlNS__GlobalFitTool
        TrigMuGirlStauGlobalFitTool = MuGirlNS__GlobalFitTool("TrigMuGirlStauGlobalFitTool")
        ToolSvc += TrigMuGirlStauGlobalFitTool
        TrigMuGirlStauGlobalFitTool.trackFitter= TrigMuGirlTrackBuilder
        #TrigMuGirlStauGlobalFitTool.OutputLevel=2

        from MuGirlGlobalFit.MuGirlGlobalFitConf import MuGirlNS__GlobalFitTool
        TrigMuGirlSAGlobalFitTool = MuGirlNS__GlobalFitTool("TrigMuGirlSAGlobalFitTool")
        ToolSvc += TrigMuGirlSAGlobalFitTool
        TrigMuGirlSAGlobalFitTool.trackFitter= TrigMuGirlTrackBuilder
        #TrigMuGirlSAGlobalFitTool.OutputLevel=2

        self.MuGirlGlobalFIT = TrigMuGirlGlobalFitTool
        self.MuGirlGlobalStauFIT = TrigMuGirlStauGlobalFitTool
        self.MuGirlGlobalMuonFeatureFIT = TrigMuGirlSAGlobalFitTool

        #from TrackToCalo.ExtrapolTrackToCaloToolBase import ExtrapolTrackToCaloToolFactory
        #MuGirlExtrapolTrackToCaloTool = ExtrapolTrackToCaloToolFactory(depth="showerdefault")
        #ToolSvc += MuGirlExtrapolTrackToCaloTool
        #self.ExtrapolTrackToCaloTool = MuGirlExtrapolTrackToCaloTool


        from MuGirlStau.MuGirlStauConfig import MuGirlNS__StauToolConfig
        TrigMuGirlStauTool = MuGirlNS__StauToolConfig("TrigMuGirlStauTool")
        TrigMuGirlStauTool.StauGlobalFitTool = TrigMuGirlStauGlobalFitTool
        TrigMuGirlStauTool.particleCreatorTool = TrigMuGirl_MuidParticleCreatorTool
        ToolSvc += TrigMuGirlStauTool
        self.MuGirlStauTool = TrigMuGirlStauTool

