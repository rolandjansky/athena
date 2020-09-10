# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# TrigMuonEF configurables
#
from TrigMuonEF import TrigMuonEFConf
from TrigMuonEF import TrigMuonEFMonitoring
from TriggerJobOpts.TriggerFlags  import TriggerFlags
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr
from AthenaCommon import CfgGetter
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.SystemOfUnits import GeV,mm
from AthenaCommon.Logging import logging
log = logging.getLogger('TrigMuonEFConfig')

from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

from RecExConfig.RecFlags import rec
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags

from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

from MuonRecExample.MuonRecFlags import muonRecFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

#Offline calorimeter isolation tool
#from TrackInCaloTools import TrackInCaloTools
#TMEFTrackInCaloTools = TrackInCaloTools.TrackInCaloTools(name='MuonIsoTrackInCaloTools')
#ToolSvc += TMEFTrackInCaloTools

#from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
#TMEFCaloIsolationTool = xAOD__CaloIsolationTool(name='MuonIsoCaloIsolationTool',
#                                                ExtrapolTrackToCaloTool = TMEFTrackInCaloTools)

#ToolSvc += TMEFCaloIsolationTool

from egammaRec.Factories import PublicToolFactory

from AthenaCommon.GlobalFlags import globalflags
isMC = not globalflags.DataSource()=='data'
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool as ICT
IsoCorrectionTool = PublicToolFactory(ICT,
                                name = "NewLeakageCorrTool",
                                IsMC = isMC)

from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool

# tool to collect topo clusters in cone
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
CaloClustersInConeTool = PublicToolFactory(xAOD__CaloClustersInConeTool,
                                     CaloClusterLocation = "CaloCalTopoClusters")

from CaloClusterCorrection import CaloClusterCorrectionConf as Cccc
TrigCaloFillRectangularCluster = PublicToolFactory( Cccc.CaloFillRectangularCluster,
          name = "trigMuon_CaloFillRectangularCluster",
          eta_size = 5,
          phi_size = 7,
          cells_name = "")


if not hasattr(ServiceMgr,"TrackingVolumesSvc"):
    from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingVolumesSvc
    ServiceMgr += Trk__TrackingVolumesSvc("TrackingVolumesSvc")

def TMEF_MaterialEffectsUpdator(name='TMEF_MaterialEffectsUpdator',**kwargs):
    return CfgMgr.Trk__MaterialEffectsUpdator(name,**kwargs)


def TMEF_Propagator(name='TMEF_Propagator',**kwargs):
    return ToolSvc.AtlasRungeKuttaPropagator
    return CfgMgr.Trk__IntersectorWrapper(name,**kwargs)


def TMEF_Navigator(name='TMEF_Navigator',**kwargs):
    kwargs.setdefault("TrackingGeometrySvc",AtlasTrackingGeometrySvc)
    return CfgMgr.Trk__Navigator(name,**kwargs)


def TMEF_Extrapolator(name='TMEF_Extrapolator',**kwargs):
    return ToolSvc.AtlasExtrapolator
    kwargs.setdefault("Propagators", [ "TMEF_Propagator" ])
    kwargs.setdefault("MaterialEffectsUpdators",[ "TMEF_MaterialEffectsUpdator" ])
    kwargs.setdefault("Navigator","TMEF_Navigator")
    return CfgMgr.Trk__Extrapolator(name,**kwargs)


def TMEF_CaloEnergyTool(name='TMEF_CaloEnergyTool', **kwargs):
    kwargs.setdefault("EnergyLossMeasurement",False)
    kwargs.setdefault("MopParametrization",True)
    return CfgMgr.Rec__MuidCaloEnergyTool(name,**kwargs)


def TMEF_CaloTrackStateOnSurface(name='TMEF_CaloTrackStateOnSurface',**kwargs):
    kwargs.setdefault("CaloEnergyDeposit","TMEF_CaloEnergyTool")
    kwargs.setdefault("CaloEnergyParam"  ,"TMEF_CaloEnergyTool")
    kwargs.setdefault("Propagator", "TMEF_Propagator")
    kwargs.setdefault("MinRemainingEnergy", 0.2*GeV)
    kwargs.setdefault("ParamPtCut", 3.0*GeV)
    return CfgMgr.Rec__MuidCaloTrackStateOnSurface(name,**kwargs)


def TMEF_TrackSummaryTool(name='TMEF_TrackSummaryTool',**kwargs):
    # always setup muon tool
    kwargs.setdefault("MuonSummaryHelperTool", "MuonTrackSummaryHelperTool")
    kwargs.setdefault("doSharedHits", False)
    # only add ID tool if ID is on
    if DetFlags.detdescr.ID_on():
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool
        kwargs.setdefault("InDetSummaryHelperTool", InDetTrigTrackSummaryHelperTool)
        kwargs.setdefault("doHolesInDet",True)

    return CfgMgr.Trk__TrackSummaryTool(name,**kwargs)


def TMEF_CombinedMuonTag(name='TMEF_CombinedMuonTag',**kwargs):
    kwargs.setdefault("ExtrapolatorTool","TMEF_Extrapolator")
    kwargs.setdefault("Chi2Cut", 50000.) # Muid: 50000. old value: 2500.
    return CfgMgr.MuonCombined__MuonTrackTagTestTool(name,**kwargs)


def TMEF_MatchQuality(name='TMEF_MatchQuality',**kwargs):
    kwargs.setdefault("TagTool","TMEF_CombinedMuonTag")
    kwargs.setdefault("TrackQuery","TMEF_MuonTrackQuery")
    return CfgMgr.Rec__MuonMatchQuality(name,**kwargs)

def TMEF_MaterialAllocator(name='TMEF_MaterialAllocator',**kwargs):
    kwargs.setdefault("AggregateMaterial", True)
    kwargs.setdefault("Extrapolator","TMEF_Extrapolator")
    kwargs.setdefault("TrackingGeometrySvc", AtlasTrackingGeometrySvc)
    return CfgMgr.Trk__MaterialAllocator(name,**kwargs)



def TMEF_iPatFitter(name='TMEF_iPatFitter',**kwargs):
    kwargs.setdefault("AggregateMaterial", True)
    kwargs.setdefault("FullCombinedFit", True)
    if not TriggerFlags.run2Config == '2016':
        kwargs.setdefault("MaxIterations", 15)
    kwargs.setdefault("MaterialAllocator", "TMEF_MaterialAllocator")
    from InDetRecExample import TrackingCommon
    kwargs.setdefault("SolenoidalIntersector",TrackingCommon.getSolenoidalIntersector())
    kwargs.setdefault("TrackSummaryTool", 'TMEF_TrackSummaryTool')
    return CfgMgr.Trk__iPatFitter(name,**kwargs)


def TMEF_iPatSLFitter(name='TMEF_iPatSLFitter',**kwargs):
    kwargs.setdefault("LineFit", True)
    #kwargs.setdefault("LineMomentum", muonStandaloneFlags.straightLineFitMomentum())
    # call the other factory function
    return TMEF_iPatFitter(name,**kwargs)


def TMEF_TrackCleaner(name = 'TMEF_TrackCleaner',**kwargs):
    if TriggerFlags.run2Config=='2016':
        kwargs.setdefault('PullCut',    3.0)
        kwargs.setdefault('PullCutPhi', 3.0)
    else:
        kwargs.setdefault('PullCut',    4.0)
        kwargs.setdefault('PullCutPhi', 4.0)
    kwargs.setdefault("Fitter", "TMEF_iPatFitter")
    kwargs.setdefault("SLFitter", "TMEF_iPatSLFitter")
    if not TriggerFlags.run2Config == '2016':
        kwargs.setdefault("Iterate", False)
        kwargs.setdefault("RecoverOutliers", False)
    if muonRecFlags.doSegmentT0Fit():
        kwargs.setdefault("RecoverOutliers", False)
    return CfgMgr.Muon__MuonTrackCleaner(name,**kwargs)


def TMEF_TrkMaterialProviderTool(name='TMEF_TrkMaterialProviderTool',**kwargs):
    from TrkMaterialProvider.TrkMaterialProviderConf import Trk__TrkMaterialProviderTool
    kwargs.setdefault("UseCaloEnergyMeasurement", False)
    return Trk__TrkMaterialProviderTool(name,**kwargs)


def TMEF_CombinedMuonTrackBuilder(name='TMEF_CombinedMuonTrackBuilder',**kwargs):
    kwargs.setdefault("CaloEnergyParam", "TMEF_CaloEnergyTool")
    kwargs.setdefault("CaloTSOS", "TMEF_CaloTrackStateOnSurface")
    kwargs.setdefault("CscRotCreator", "") # enabled with special version in Muid offline
    kwargs.setdefault("Fitter", "TMEF_iPatFitter")
    kwargs.setdefault("SLFitter", "TMEF_iPatSLFitter")
    kwargs.setdefault("MaterialAllocator", "TMEF_MaterialAllocator")
    kwargs.setdefault("MdtRotCreator", "MdtDriftCircleOnTrackCreator")
    kwargs.setdefault("MuonHoleRecovery", "")
    kwargs.setdefault("Propagator", "TMEF_Propagator")
    kwargs.setdefault("SLPropagator", "TMEF_Propagator")
    kwargs.setdefault("CleanCombined", True)
    kwargs.setdefault("CleanStandalone", True)
    kwargs.setdefault("BadFitChi2", 2.5)
    kwargs.setdefault("LargeMomentumError", 0.5)
    #kwargs.setdefault("LineMomentum", muonStandaloneFlags.straightLineFitMomentum())
    kwargs.setdefault("LowMomentum", 10.*GeV)
    kwargs.setdefault("MinEnergy", 0.3*GeV)
    kwargs.setdefault("PerigeeAtSpectrometerEntrance", False)
    kwargs.setdefault("ReallocateMaterial", False)
    kwargs.setdefault("Vertex2DSigmaRPhi", 100.*mm)
    kwargs.setdefault("Vertex3DSigmaRPhi", 6.*mm)
    kwargs.setdefault("Vertex3DSigmaZ", 60.*mm)
    if not TriggerFlags.run2Config == '2016':
        kwargs.setdefault("MuonErrorOptimizer", '')

    # note - the TrackSummaryTool is done as follows offline:
    # import MuonCombinedRecExample.CombinedMuonTrackSummary
    # combinedMuonTrackBuilder.TrackSummaryTool = ToolSvc.CombinedMuonTrackSummary
    # should check our config is really ok here
    kwargs.setdefault("TrackSummaryTool", 'TMEF_TrackSummaryTool')

    # extra w.r.t. Muid Offline
    kwargs.setdefault("Cleaner", "TMEF_TrackCleaner")
    if muonRecFlags.doSegmentT0Fit():
        kwargs.setdefault("MdtRotCreator", "")

    kwargs.setdefault("UseCaloTG", True)
    kwargs.setdefault("CaloMaterialProvider", "TMEF_TrkMaterialProviderTool")
    kwargs.setdefault("TrackQuery", "TMEF_MuonTrackQuery")
    if muonRecFlags.enableErrorTuning():
        kwargs.setdefault("MuonErrorOptimizer", CfgGetter.getPublicToolClone("TMEF_MuidErrorOptimisationTool",
                                                                             "MuonErrorOptimisationTool",
                                                                             PrepareForFit              = False,
                                                                             RecreateStartingParameters = False,
                                                                             RefitTool = CfgGetter.getPublicToolClone("TMEF_MuidRefitTool",
                                                                                                                      "MuonRefitTool",
                                                                                                                      AlignmentErrors = False,
                                                                                                                      Fitter = CfgGetter.getPublicTool("iPatFitter"))
                                                                             ))

    return CfgMgr.Rec__CombinedMuonTrackBuilder(name,**kwargs)


def TMEF_MuonTrackQuery(name='TMEF_MuonTrackQuery',**kwargs):
    kwargs.setdefault("MdtRotCreator","MdtDriftCircleOnTrackCreator")
    kwargs.setdefault("Fitter", 'TMEF_iPatFitter')
    return CfgMgr.Rec__MuonTrackQuery(name,**kwargs)


def TMEF_MatchMaker(name='TMEF_MatchMaker',**kwargs):
    kwargs.setdefault("AmbiguityProcessor", "MuonAmbiProcessor")
    kwargs.setdefault("MatchQuality", "TMEF_MatchQuality")
    kwargs.setdefault("CaloTSOS", "TMEF_CaloTrackStateOnSurface") # not in Muid?
    kwargs.setdefault("TrackBuilder", "TMEF_CombinedMuonTrackBuilder")
    kwargs.setdefault("TrackQuery", "TMEF_MuonTrackQuery")
    kwargs.setdefault("Propagator", "TMEF_Propagator")
    kwargs.setdefault("IndetPullCut", 6.0)
    kwargs.setdefault("MomentumBalanceCut", 10.0) # old value: 20.0
    return CfgMgr.Rec__MuidMatchMaker(name,**kwargs)


def TMEF_TrackBuilderTool(name='TMEF_TrackBuilderTool',extraFlags=None,**kwargs):
    if extraFlags is None:
        from MuonRecExample.MuonRecUtils import ExtraFlags
        extraFlags = ExtraFlags()

    extraFlags.setFlagDefault("namePrefix","TMEF_")
    #        extraFlags.setFlagDefault("optimiseMomentumResolutionUsingChi2", False)  # offline default: False
    #        extraFlags.setFlagDefault("materialSource", "TGMat") # offline default: "TGMat"
    from MuonRecExample.MuonStandalone import MuonTrackSteering
    return MuonTrackSteering(name, extraFlags=extraFlags, **kwargs)

# new tools added by MO
def TMEF_TrackSummaryToolNoHole(name='TMEF_TrackSummaryToolNoHole',**kwargs):
    sumtool = TMEF_TrackSummaryTool(name, **kwargs)
    sumtool.doHolesInDet = False
    # @TODO to switch off hole search should use a prticular InDet...Helper
    return sumtool

def TMEF_TrkToTrackParticleConvTool(name="TMEF_TrkToTrackParticleConvTool",**kwargs):
    #import MuonCombinedRecExample.CombinedMuonTrackSummary
    kwargs.setdefault("Extrapolator", "AtlasExtrapolator" )
    kwargs.setdefault("TrackSummaryTool", 'TMEF_TrackSummaryToolNoHole')#ToolSvc.CombinedMuonTrackSummary ) #getPublicTool("CombinedMuonTrackSummary") )
    kwargs.setdefault("KeepAllPerigee",False )
    return CfgMgr.Trk__TrackParticleCreatorTool(name,**kwargs)

def TMEF_MuonCombinedTool(name="TMEF_MuonCombinedTool",**kwargs):
    #protectedInclude ("MuidExample/MuonIdentification_jobOptions.py")
    kwargs.setdefault("MuonCombinedTagTools", [ "TMEF_MuonCombinedFitTagTool" ] )
   # kwargs.setdefault("PtBalancePreSelection", 0.75)
    return CfgMgr.MuonCombined__MuonCombinedTool(name,**kwargs)

def TMEF_MuidMuonRecovery(name='TMEF_MuidMuonRecovery',**kwargs):
    kwargs.setdefault('Extrapolator', "AtlasExtrapolator")
    kwargs.setdefault('TrackBuilder',"TMEF_CombinedMuonTrackBuilder")
    return CfgMgr.Rec__MuidMuonRecovery(name,**kwargs)

def MuonHolesOnTrack(name='MuonHolesOnTrack',**kwargs):
    kwargs.setdefault("ExtrapolatorName"      , "MuonExtrapolator" )
    kwargs.setdefault("RIO_OnTrackCreator"    , "MuonRotCreator"   )
    kwargs.setdefault("DoHolesIdentification" , True )
    kwargs.setdefault("DoParameterUpdate", True )
    kwargs.setdefault("LowerTrackMomentumCut", 2000.0 )
    AtlasTrackingGeometrySvc =  CfgGetter.getService("AtlasTrackingGeometrySvc")
    kwargs.setdefault("TrackingGeometryName", AtlasTrackingGeometrySvc.TrackingGeometryName )

    from MuonTGRecTools.MuonTGRecToolsConf import Muon__MuonHolesOnTrackTool
    return Muon__MuonHolesOnTrackTool(name,**kwargs)
# end of factory function MuonHolesOnTrackTool

# need to do something about cosmics here - in offline this tool is None if not collisions running
# if jobproperties.Beam.beamType()=='collisions':
def TMEF_CombMuonMEOTPParam(name='TMEF_CombMuonMEOTPParam',**kwargs):
    kwargs.setdefault('TSOSTool','TMEF_CaloTrackStateOnSurface')
    from MuidCaloScatteringTools.MuidCaloScatteringToolsConf import Rec__MuidMaterialEffectsOnTrackProvider
    return Rec__MuidMaterialEffectsOnTrackProvider(name, **kwargs)

def TMEF_MuonCombinedPropagator(name='TMEF_MuonCombinedPropagator',**kwargs):
    kwargs.setdefault('AccuracyParameter', 0.000001)
    kwargs.setdefault('IncludeBgradients', True)
    kwargs.setdefault('MaxStraightLineStep', 0.001)
    kwargs.setdefault('MaxHelixStep', 0.001)
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator
    return Trk__RungeKuttaPropagator(name, **kwargs)

def TMEF_MuonCombinedTrackFitter(name='TMEF_MuonCombinedTrackFitter',**kwargs):
    kwargs.setdefault('ExtrapolationTool',     'AtlasExtrapolator')#gpt
    kwargs.setdefault('NavigatorTool',         ToolSvc.MuonNavigator)
    kwargs.setdefault('PropagatorTool',        'TMEF_MuonCombinedPropagator')#gpt
    kwargs.setdefault('RotCreatorTool',        'MuonRotCreator')#gpt
    kwargs.setdefault('MeasurementUpdateTool', 'MuonMeasUpdator')#gpt
    kwargs.setdefault('TrackingGeometrySvc',   ServiceMgr.AtlasTrackingGeometrySvc)
    kwargs.setdefault('ExtrapolatorMaterial',  True)
    kwargs.setdefault('MuidTool',              None)
    kwargs.setdefault('MuidToolParam',         'TMEF_CombMuonMEOTPParam')#gpt
    kwargs.setdefault('MuidMat',               False)
    kwargs.setdefault('StraightLine',          False) #FIX? StraightLine          = not jobproperties.BField.solenoidOn() and not jobproperties.BField.allToroidOn(),
    kwargs.setdefault('MaxIterations',         50)
    kwargs.setdefault('GetMaterialFromTrack',  True) #FIX? jobproperties.BField.solenoidOn() and jobproperties.BField.allToroidOn(),
    kwargs.setdefault('RecalculateDerivatives',False)
    kwargs.setdefault("UseCaloTG",             True)
    kwargs.setdefault("CaloMaterialProvider",  "TMEF_TrkMaterialProviderTool")
    from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
    return  Trk__GlobalChi2Fitter(name, **kwargs)

def TMEF_MuonCombinedFitTagTool(name="TMEF_MuonCombinedFitTagTool",**kwargs):
    kwargs.setdefault("TrackBuilder",         'TMEF_CombinedMuonTrackBuilder' )
    if not TriggerFlags.run2Config == '2016':
        kwargs.setdefault("MuonRecovery",         '' )
    else:
        kwargs.setdefault("MuonRecovery",         'TMEF_MuidMuonRecovery' )
    kwargs.setdefault("TrackQuery",           'TMEF_MuonTrackQuery' )
    kwargs.setdefault("MatchQuality",         'TMEF_MatchQuality' )
    return CfgMgr.MuonCombined__MuonCombinedFitTagTool(name,**kwargs)

def TMEF_MuonCandidateTool(name="TMEF_MuonCandidateTool",**kwargs):
    kwargs.setdefault("TrackBuilder","TMEF_CombinedMuonTrackBuilder")
    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("TrackExtrapolationTool", CfgGetter.getPublicTool("ExtrapolateMuonToIPTool"))
    return CfgMgr.MuonCombined__MuonCandidateTool(name,**kwargs)

def TrigMuonAmbiProcessor(name="TrigMuonAmbiProcessor",**kwargs) :
    # definition mostly copied from MuonRecExample/python/MooreTools.py
    import InDetRecExample.TrackingCommon as TrackingCommon
    kwargs.setdefault("AssociationTool",TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels())
    kwargs.setdefault("DropDouble", False)
    kwargs.setdefault("ScoringTool", "MuonTrackScoringTool")
    kwargs.setdefault("SelectionTool", "MuonAmbiSelectionTool" )
    return CfgMgr.Trk__TrackSelectionProcessorTool(name,**kwargs)

def TMEF_MuonCreatorTool(name="TMEF_MuonCreatorTool",**kwargs):
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = AtlasExtrapolator())
    kwargs.setdefault("ParticleCaloExtensionTool", pcExtensionTool)
    kwargs.setdefault('TrackParticleCreator','TMEF_TrkToTrackParticleConvTool')
    kwargs.setdefault("AmbiguityProcessor", CfgGetter.getPublicTool('TrigMuonAmbiProcessor'))
    kwargs.setdefault('MakeTrackAtMSLink',True)
    kwargs.setdefault("CaloMaterialProvider", "TMEF_TrkMaterialProviderTool")
    kwargs.setdefault("FillTimingInformation",False)
    kwargs.setdefault("MuonSelectionTool", "")
    kwargs.setdefault("TrackQuery", "TMEF_MuonTrackQuery")
    kwargs.setdefault("TrackSummaryTool", "TMEF_TrackSummaryTool")
    return CfgMgr.MuonCombined__MuonCreatorTool(name,**kwargs)

def TMEF_MuonCandidateTrackBuilderTool(name="TMEF_MuonCandidateTrackBuilderTool",**kwargs):
    kwargs.setdefault('MuonTrackBuilder', 'TMEF_CombinedMuonTrackBuilder')
    kwargs.setdefault('MuonSegmentTrackBuilder', CfgGetter.getPublicTool("MooMuonTrackBuilder"))
    return CfgMgr.Muon__MuonCandidateTrackBuilderTool(name,**kwargs)

def TMEF_MuonPRDSelectionTool(name="TMEF_MuonPRDSelectionTool",**kwargs):
    kwargs.setdefault('MuonRecoValidationTool','')
    return CfgMgr.Muon__MuonPRDSelectionTool(name,**kwargs)

def TMEF_MuonClusterSegmentFinder(name="TMEF_MuonClusterSegmentFinder", **kwargs):
    kwargs.setdefault('MuonPRDSelectionTool', 'TMEF_MuonPRDSelectionTool')
    return CfgMgr.Muon__MuonClusterSegmentFinder(name,**kwargs)

def TMEF_MuonClusterSegmentFinderTool(name="TMEF_MuonClusterSegmentFinderTool", extraFlags=None,**kwargs):
    kwargs.setdefault("TrackSummaryTool", "TMEF_TrackSummaryTool")
    return CfgMgr.Muon__MuonClusterSegmentFinderTool(name,**kwargs)

def TMEF_MuonLayerSegmentFinderTool(name="TMEF_MuonLayerSegmentFinderTool",**kwargs):
    kwargs.setdefault('MuonRecoValidationTool','')
    kwargs.setdefault('MuonPRDSelectionTool','TMEF_MuonPRDSelectionTool')
    kwargs.setdefault('MuonClusterSegmentFinder','TMEF_MuonClusterSegmentFinder')
    if not MuonGeometryFlags.hasCSC():
        kwargs.setdefault('Csc2DSegmentMaker', '')
        kwargs.setdefault('Csc4DSegmentMaker', '')
    if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()): kwargs.setdefault('NSWMuonClusterSegmentFinderTool','TMEF_MuonClusterSegmentFinderTool')
    return CfgMgr.Muon__MuonLayerSegmentFinderTool(name,**kwargs)

def TMEF_MuonInsideOutRecoTool(name="TMEF_MuonInsideOutRecoTool",**kwargs):
    kwargs.setdefault('MuonTrackBuilder','TMEF_CombinedMuonTrackBuilder')
    kwargs.setdefault('MuonCandidateTrackBuilderTool','TMEF_MuonCandidateTrackBuilderTool')
    kwargs.setdefault('MuonRecoValidationTool','')
    kwargs.setdefault('MuonLayerSegmentFinderTool', 'TMEF_MuonLayerSegmentFinderTool')
    return CfgMgr.MuonCombined__MuonInsideOutRecoTool(name,**kwargs )

### Stau late trigger configs

def TMEF_MuonStauRecoTool( name='TMEF_MuonStauRecoTool', **kwargs ):
   # kwargs.setdefault('DoSummary', True)
   kwargs.setdefault('DoSummary', muonCombinedRecFlags.printSummary() )
   kwargs.setdefault('ConsideredPDGs', [13,-13,1000015,-1000015])
   kwargs.setdefault('DoTruth', rec.doTruth() )
   kwargs.setdefault('MuonSegmentMaker', CfgGetter.getPublicTool('DCMathStauSegmentMaker') )
   kwargs.setdefault('MuonInsideOutRecoTool', CfgGetter.getPublicTool('TMEF_MuonStauInsideOutRecoTool') )
   return CfgMgr.MuonCombined__MuonStauRecoTool(name,**kwargs )

def TMEF_MuonStauInsideOutRecoTool( name='TMEF_MuonStauInsideOutRecoTool', **kwargs ):
   kwargs.setdefault('MuonTrackBuilder','TMEF_CombinedMuonTrackBuilder')
   kwargs.setdefault('MuonCandidateTrackBuilderTool', CfgGetter.getPublicTool('TMEF_MuonStauCandidateTrackBuilderTool') )
   return CfgMgr.MuonCombined__MuonInsideOutRecoTool(name,**kwargs )

def TMEF_MuonStauCandidateTrackBuilderTool( name='TMEF_MuonStauCandidateTrackBuilderTool',**kwargs):
   kwargs.setdefault('MuonTrackBuilder',  CfgGetter.getPublicTool('TMEF_CombinedStauTrackBuilder') )
   return CfgMgr.Muon__MuonCandidateTrackBuilderTool(name,**kwargs)

def TMEF_CombinedStauTrackBuilder( name='TMEF_CombinedStauTrackBuilder', **kwargs ):
   kwargs.setdefault('MdtRotCreator'                 , CfgGetter.getPublicTool('MdtDriftCircleOnTrackCreatorStau') )
   kwargs.setdefault('MuonHoleRecovery'              , CfgGetter.getPublicTool('TMEF_MuonStauSegmentRegionRecoveryTool') )
   return TMEF_CombinedMuonTrackBuilder(name,**kwargs )

def TMEF_MuonStauSegmentRegionRecoveryTool(name='TMEF_MuonStauSegmentRegionRecoveryTool',**kwargs ):
    kwargs.setdefault('SeededSegmentFinder', CfgGetter.getPublicTool('MuonStauSeededSegmentFinder') )
    kwargs.setdefault('ChamberHoleRecoveryTool', CfgGetter.getPublicTool('MuonStauChamberHoleRecoveryTool') )
    kwargs.setdefault('Fitter',  CfgGetter.getPublicTool('TMEF_CombinedStauTrackBuilderFit') )
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.isOnline:
        kwargs.setdefault('MdtCondKey', "")
    return CfgMgr.Muon__MuonSegmentRegionRecoveryTool(name,**kwargs)

def TMEF_CombinedStauTrackBuilderFit( name='TMEF_CombinedStauTrackBuilderFit', **kwargs ):
   kwargs.setdefault('MdtRotCreator'                 , CfgGetter.getPublicTool('MdtDriftCircleOnTrackCreatorStau') )
   return TMEF_CombinedMuonTrackBuilder(name,**kwargs )

def TMEF_MdtRawDataProviderTool(name="TMEF_MdtRawDataProviderTool",**kwargs):
    kwargs.setdefault("Decoder", "MdtROD_Decoder")
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if DetFlags.overlay.MDT_on() and overlayFlags.isDataOverlay():
      kwargs.setdefault("RdoLocation",overlayFlags.dataStore()+"+MDTCSM")
    return CfgMgr.Muon__MDT_RawDataProviderTool(name,**kwargs)

def TMEF_RpcRawDataProviderTool(name = "TMEF_RpcRawDataProviderTool",**kwargs):
    kwargs.setdefault("Decoder", "RpcROD_Decoder")
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if DetFlags.overlay.RPC_on() and overlayFlags.isDataOverlay():
      kwargs.setdefault("RdoLocation", overlayFlags.dataStore()+"+RPCPAD")
    return CfgMgr.Muon__RPC_RawDataProviderTool(name,**kwargs)

def TMEF_TgcRawDataProviderTool(name = "TMEF_TgcRawDataProviderTool",**kwargs):
    kwargs.setdefault("Decoder", "TgcROD_Decoder")
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if DetFlags.overlay.TGC_on() and overlayFlags.isDataOverlay():
      kwargs.setdefault("RdoLocation", overlayFlags.dataStore()+"+TGCRDO")
    return CfgMgr.Muon__TGC_RawDataProviderTool(name,**kwargs)

def TMEF_CscRawDataProviderTool(name = "TMEF_CscRawDataProviderTool",**kwargs):
    kwargs.setdefault("Decoder", "CscROD_Decoder")
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if DetFlags.overlay.CSC_on() and overlayFlags.isDataOverlay():
      kwargs.setdefault("RdoLocation", overlayFlags.dataStore()+"+CSCRDO")
    return CfgMgr.Muon__CSC_RawDataProviderTool(name,**kwargs)

# TrigMuonEF classes
class TrigMuonEFTrackBuilderConfig ():
    __slots__ = ()

    def __init__( self, name="TrigMuonEFTrackBuilder" ):

        raise RuntimeError("TrigMuonEFTrackBuilder no longer supported in release 19 (or later) due to removal of MooTrackSteering from the release.")

class TrigMuonEFCombinerConfig ():
    __slots__ = ()

    def __init__( self, name="TrigMuonEFCombiner" ):
        raise RuntimeError("TrigMuonEFCombinerConfig no longer supported - please switch to combiner mode of TrigMuSuperEF")



class TrigMuonEFStandaloneTrackToolConfig (TrigMuonEFConf.TrigMuonEFStandaloneTrackTool):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFStandaloneTrackTool", **kwargs ):
        super( TrigMuonEFStandaloneTrackToolConfig, self ).__init__( name, **kwargs )

        if MuonGeometryFlags.hasCSC(): self.CscClusterProvider = CfgGetter.getPublicTool("CscThresholdClusterBuilderTool")

        muonLayerHoughTool = CfgGetter.getPublicTool("MuonLayerHoughTool")
        muonLayerHoughTool.DoTruth=False

        self.SegmentsFinderTool = CfgGetter.getPublicToolClone( "TMEF_SegmentsFinderTool","MooSegmentFinder",
                                                                HoughPatternFinder = muonLayerHoughTool,
                                                                Csc2dSegmentMaker=("Csc2dSegmentMaker/Csc2dSegmentMaker" if MuonGeometryFlags.hasCSC() else ""),
                                                                Csc4dSegmentMaker=("Csc4dSegmentMaker/Csc4dSegmentMaker" if MuonGeometryFlags.hasCSC() else ""))

        CfgGetter.getPublicTool("MuonHoughPatternFinderTool").RecordAll=False
        CfgGetter.getPublicTool("MooTrackFitter").SLFit=False

        self.MdtRawDataProvider = "TMEF_MdtRawDataProviderTool"
        self.CscRawDataProvider = "TMEF_CscRawDataProviderTool"
        self.RpcRawDataProvider = "TMEF_RpcRawDataProviderTool"
        self.TgcRawDataProvider = "TMEF_TgcRawDataProviderTool"

        #Need to run non-MT version of decoding tools in the trigger since the caching is not available in MT versions
        #Need different PRD container names to run offline and trigger in same jobs, but there are many tools that depend on these containers...
        #Since this is legacy code only used for validation comparisons against the Run 3 triggers, will do the not-particularly-nice solution of
        #creating containers with unique names only if we are running offline and trigger in the same jobs, and otherwise just use the default names.
        #This means that the trigger output when running the trigger as part of RAWtoESD is not 100% correct (the pattern finding uses the correct containers,
        #so it's a small effect overall anyway), but that's an use case not currently needed for trigger validation purposes
        from AthenaCommon.AppMgr import ToolSvc
        #MDT
        from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
        from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
        from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
        from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
        MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataTool(name = "TrigEFMdtRdoToPrepDataTool")
        CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataTool(name = "TrigEFCscRdoToPrepDataTool")
        TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataTool(name = "TrigEFTgcRdoToPrepDataTool")
        RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name = "TrigEFRpcRdoToPrepDataTool")
        if not rec.doRDOTrigger and rec.doESD:
            MdtRdoToMdtPrepDataTool.OutputCollection = "TrigMDT_DriftCircles"
            CscRdoToCscPrepDataTool.OutputCollection = "TrigCSC_Measurements"
            TgcRdoToTgcPrepDataTool.OutputCollection = "TrigTGC_Measurements"
            TgcRdoToTgcPrepDataTool.OutputCoinCollection = "TrigerT1CoinDataCollection"
            RpcRdoToRpcPrepDataTool.TriggerOutputCollection="TrigRPC_Measurements"
            #InputCollection is really the output RPC coin collection...
            RpcRdoToRpcPrepDataTool.InputCollection="TrigRPC_triggerHits"
        else:
            MdtRdoToMdtPrepDataTool.OutputCollection = "MDT_DriftCircles"
            CscRdoToCscPrepDataTool.OutputCollection = "CSC_Measurements"
            TgcRdoToTgcPrepDataTool.OutputCollection = "TGC_Measurements"
            TgcRdoToTgcPrepDataTool.OutputCoinCollection = "TrigT1CoinDataCollection"
            RpcRdoToRpcPrepDataTool.TriggerOutputCollection="RPC_Measurements"
            RpcRdoToRpcPrepDataTool.InputCollection="RPC_triggerHits"

        ToolSvc += MdtRdoToMdtPrepDataTool
        self.MdtPrepDataContainer =  MdtRdoToMdtPrepDataTool.OutputCollection
        self.MdtPrepDataProvider=MdtRdoToMdtPrepDataTool
        #CSC
        ToolSvc += CscRdoToCscPrepDataTool
        self.CscPrepDataProvider=CscRdoToCscPrepDataTool
        #We use the clusters not the PRD hits directly for CSCs
        self.CscPrepDataContainer="CSC_Clusters"
        #TGC
        ToolSvc += TgcRdoToTgcPrepDataTool
        self.TgcPrepDataProvider=TgcRdoToTgcPrepDataTool
        self.TgcPrepDataContainer=TgcRdoToTgcPrepDataTool.OutputCollection
        #RPC
        ToolSvc += RpcRdoToRpcPrepDataTool
        self.RpcPrepDataProvider=RpcRdoToRpcPrepDataTool
        self.RpcPrepDataContainer=RpcRdoToRpcPrepDataTool.TriggerOutputCollection

        self.DecodeMdtBS = DetFlags.readRDOBS.MDT_on()
        self.DecodeRpcBS = DetFlags.readRDOBS.RPC_on()
        self.DecodeTgcBS = DetFlags.readRDOBS.TGC_on()
        if MuonGeometryFlags.hasCSC(): self.DecodeCscBS = DetFlags.readRDOBS.CSC_on()

        # use seeded decoding
        if (TriggerFlags.MuonSlice.doEFRoIDrivenAccess()):
            self.useMdtSeededDecoding = True
            self.useRpcSeededDecoding = True
            self.useTgcSeededDecoding = True
            if MuonGeometryFlags.hasCSC(): self.useCscSeededDecoding = True

            # use ROB based seeded decoding instead of PRD based
            self.useTgcRobDecoding = False # neither available nor needed
            self.useCscRobDecoding = False # neither available nor needed


        self.useRpcData=muonRecFlags.doRPCs()
        self.useTgcData=muonRecFlags.doTGCs()
        self.useCscData=muonRecFlags.doCSCs()
        # to select barrel(useMdtData=2), endcap(useMdtData=3)
        if muonRecFlags.doMDTs():
            self.useMdtData=1
        else:
            self.useMdtData=0


        self.useTGCInPriorNextBC = False

        self.doTimeOutChecks = False
        self.doTimeOutGuard  = False
        self.maxTgcHits      = 0
        self.maxCscHits      = 0
        self.maxRpcHits      = 0
        self.maxMdtHits      = 0
        self.doCache = True
        self.IgnoreMisalginedCSCs = False

        self.TrackBuilderTool  = "TMEF_TrackBuilderTool"
        self.TrkSummaryTool = "TMEF_TrackSummaryTool"
        self.MuonCandidateTool = "TMEF_MuonCandidateTool"

        self.TrackToTrackParticleConvTool = "MuonParticleCreatorTool"

        import MuonCondAlg.MdtCondDbAlgConfig # noqa: F401 (MDT conditions, needed for the MuonStationIntersectSvc)

        #from MuonRecExample.MuonRecTools import MuonSTEP_Propagator
        #MuonSTEP_Propagator.OutputLevel=5
        #CfgGetter.getPublicTool("MuonStraightLinePropagator").OutputLevel=5

class TrigMuonEFCombinerConfigTRTOnly ():
    __slots__ = ()

    def __init__( self, name="TrigMuonEFCombinerTRTOnly" ):
        raise RuntimeError("TrigMuonEFCombinerTRTOnly no longer supported - please switch to combiner mode of TrigMuSuperEF")


class TrigMuonEFCaloIsolationConfig (TrigMuonEFConf.TrigMuonEFCaloIsolation):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFCaloIsolationConfig" ):
        super( TrigMuonEFCaloIsolationConfig, self ).__init__( name )

        self.RequireCombinedMuon = True
        self.applyPileupCorrection = False

        CaloTopoIsolationTool = PublicToolFactory(xAOD__CaloIsolationTool,name = "CaloTopoClusterIsolationTool",
                                            doEnergyDensityCorrection = self.applyPileupCorrection,
                                            saveOnlyRequestedCorrections = True,
                                            IsoLeakCorrectionTool          = IsoCorrectionTool,
                                            ClustersInConeTool              = CaloClustersInConeTool,
                                            CaloFillRectangularClusterTool = TrigCaloFillRectangularCluster,
                                            UseEMScale = True)

        self.CaloTopoClusterIsolationTool = CaloTopoIsolationTool()

        # histograms
        self.HistoPathBase = ""
        validation_caloiso = TrigMuonEFMonitoring.TrigMuonEFCaloIsolationValidationMonitoring()
        #online_caloiso     = TrigMuonEFMonitoring.TrigMuonEFCaloIsolationOnlineMonitoring()
        #monitoring_caloiso = TrigMuonEFMonitoring.TrigMuonEFCaloIsolationMonitoring()

        self.AthenaMonTools = [validation_caloiso]

def TMEF_TrackIsolationTool(name='TMEF_isolationTool',**kwargs):
    kwargs.setdefault('deltaZCut', 2.0*mm)
    kwargs.setdefault('removeSelf',True)
    kwargs.setdefault('useAnnulus',False)
    kwargs.setdefault('useVarIso',True)
    # Get the track selection tool
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    trkseltool = InDet__InDetTrackSelectionTool()
    if 'LooseTSel' in name:
        trkseltool.CutLevel='Loose'
    elif 'TightTSel' in name:
        trkseltool.CutLevel='TightPrimary'
    log.debug('TMEF_TrackIsolationTool added trackselection tool:\n%s', trkseltool)
    kwargs.setdefault('TrackSelectionTool',trkseltool)
    return TrigMuonEFConf.TrigMuonEFTrackIsolationTool(name, **kwargs)


class TrigMuonEFTrackIsolationConfig (TrigMuonEFConf.TrigMuonEFTrackIsolation):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFTrackIsolationConfig" ):
        super( TrigMuonEFTrackIsolationConfig, self ).__init__( name )

        # configure the isolation tool
        TMEF_IsolationTool = TMEF_TrackIsolationTool('TMEF_IsolationTool',useVarIso=False)

        # Isolation tool
        self.IsolationTool = TMEF_IsolationTool

        # ID tracks
        self.IdTrackParticles = "InDetTrigTrackingxAODCnv_Muon_IDTrig"

        # Only run algo on combined muons
        self.requireCombinedMuon = True

        # Use offline isolation variables
        self.useVarIso = False

        # histograms
        self.histoPathBase = ""
        validation_trkiso = TrigMuonEFMonitoring.TrigMuonEFTrackIsolationValidationMonitoring()
        online_trkiso     = TrigMuonEFMonitoring.TrigMuonEFTrackIsolationOnlineMonitoring()

        self.AthenaMonTools = [ validation_trkiso, online_trkiso ]


class TrigMuonEFMSTrackIsolationConfig (TrigMuonEFConf.TrigMuonEFTrackIsolation):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFMSTrackIsolationConfig" ):
        super( TrigMuonEFMSTrackIsolationConfig, self ).__init__( name )

        # configure the isolation tool
        TMEF_IsolationTool = TMEF_TrackIsolationTool('TMEF_IsolationTool',useVarIso=False)

        # Isolation tool
        self.IsolationTool = TMEF_IsolationTool

        # ID tracks
        self.IdTrackParticles = "InDetTrigTrackingxAODCnv_Muon_IDTrig"

        # Only run algo on combined muons
        self.requireCombinedMuon = False

        # Use offline isolation variables
        self.useVarIso = False

        # histograms
        self.histoPathBase = ""
        validation_trkiso = TrigMuonEFMonitoring.TrigMuonEFTrackIsolationValidationMonitoring()
        online_trkiso     = TrigMuonEFMonitoring.TrigMuonEFTrackIsolationOnlineMonitoring()

        self.AthenaMonTools = [ validation_trkiso, online_trkiso ]


class TrigMuonEFTrackIsolationVarConfig (TrigMuonEFConf.TrigMuonEFTrackIsolation):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFTrackIsolationVarConfig" ):
        super( TrigMuonEFTrackIsolationVarConfig, self ).__init__( name )

        # configure the isolation tool
        trkseltoolname = 'TMEF_VarIsolationTool'
        if 'LooseTSel' in name:
            trkseltoolname = trkseltoolname + 'LooseTSel'
        elif 'TightTSel' in name:
            trkseltoolname = trkseltoolname + 'TightTSel'

        TMEF_VarIsolationTool = TMEF_TrackIsolationTool(trkseltoolname,useVarIso=True)

        # Isolation tool
        self.IsolationTool = TMEF_VarIsolationTool


        # ID tracks
        self.IdTrackParticles = "InDetTrigTrackingxAODCnv_Muon_IDTrig"

        # Only run algo on combined muons
        self.requireCombinedMuon = True

        # Use offline isolation variables
        self.useVarIso = True

        # histograms
        self.histoPathBase = ""
        validation_trkiso = TrigMuonEFMonitoring.TrigMuonEFTrackIsolationValidationMonitoring()
        online_trkiso     = TrigMuonEFMonitoring.TrigMuonEFTrackIsolationOnlineMonitoring()

        # timing
        self.doMyTiming = True
        timetool = TrigTimeHistToolConfig("Time")
        timetool.NumberOfHistBins=100
        timetool.TimerHistLimits=[0,1000]

        self.AthenaMonTools = [ validation_trkiso, online_trkiso, timetool ]


def InDetTrkRoiMaker_Muon(name="InDetTrkRoiMaker_Muon",**kwargs):
    kwargs.setdefault("FullScanMode", "HybridScan")
    kwargs.setdefault("SeedsIDalgo", "FTF")
    kwargs.setdefault("SeedsEtaMax", 2.5)
    kwargs.setdefault("SeedsPtMin", 2.0) # GeV
    kwargs.setdefault("FullEtaRange", 3.0) # MS goes up to 2.7
    kwargs.setdefault("AthenaMonTools", [ TrigMuonEFMonitoring.InDetTrkRoiMakerMonitoring("Monitoring"),
                                          TrigMuonEFMonitoring.InDetTrkRoiMakerValidationMonitoring("MonitoringVal"),
                                          TrigTimeHistToolConfig("Time")] )

    return CfgMgr.InDetTrkRoiMaker(name,**kwargs)


class TrigMuonEFRoiAggregatorConfig (TrigMuonEFConf.TrigMuonEFRoiAggregator):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFRoiAggregator", **kwargs):
        super( TrigMuonEFRoiAggregatorConfig, self ).__init__( name )

        kwargs.setdefault("CopyTracks", False)
        self.CopyTracks = True


class TrigMuonEFFSRoiMakerConfig(TrigMuonEFConf.TrigMuonEFFSRoiMaker):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFFSRoiMaker", **kwargs):
        super( TrigMuonEFFSRoiMakerConfig, self ).__init__( name )

        self.MuonPtCut = 0.0
        self.OutputContName = "MuonEFInfo"
        self.CreateFSRoI = False

        montool     = TrigMuonEFMonitoring.TrigMuonEFFSRoiMakerMonitoring()

        self.AthenaMonTools = [ montool ]

class TrigMuonEFFSRoiMakerCaloTagConfig(TrigMuonEFConf.TrigMuonEFFSRoiMaker):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFFSRoiMakerCaloTag", **kwargs):
        super( TrigMuonEFFSRoiMakerCaloTagConfig, self ).__init__( name )

        self.MuonPtCut = 0.0
        self.OutputContName = "MuonEFInfo"
        self.InvertRoI =  True
        self.UseFSRoI =  False

        kwargs.setdefault("RoISizeEta", 3.0)
        kwargs.setdefault("RoISizePhi", 1.5)
        kwargs.setdefault("RoILabel", "forID")

        self.RoISizeEta = kwargs["RoISizeEta"]
        self.RoISizePhi = kwargs["RoISizePhi"]

        self.RoILabel   = kwargs["RoILabel"]

        montool     = TrigMuonEFMonitoring.TrigMuonEFFSRoiMakerMonitoring()

        self.AthenaMonTools = [ montool ]

class TrigMuonEFFSRoiMakerUnseededConfig(TrigMuonEFConf.TrigMuonEFFSRoiMaker):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFFSRoiMakerUnseeded", **kwargs):
        super( TrigMuonEFFSRoiMakerUnseededConfig, self ).__init__( name )

        self.MuonPtCut = 0.0
        self.OutputContName = "MuonEFInfo"
        self.CreateCrackRoI = True

        kwargs.setdefault("RoISizeEta", 0.1)
        kwargs.setdefault("RoISizePhi", 3.14159)
        kwargs.setdefault("RoILabel", "forID")

        self.RoISizeEta = kwargs["RoISizeEta"]
        self.RoISizePhi = kwargs["RoISizePhi"]
        self.RoILabel   = kwargs["RoILabel"]

        montool     = TrigMuonEFMonitoring.TrigMuonEFFSRoiMakerMonitoring()

        self.AthenaMonTools = [ montool ]

# Python config class for the TrigMuonEDIDTrackRoiMaker c++ algorithm
class TrigMuonEFIDTrackRoiMakerConfig(TrigMuonEFConf.TrigMuonEFIDTrackRoiMaker):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFIDTrackRoiMaker", **kwargs):
        super( TrigMuonEFIDTrackRoiMakerConfig, self ).__init__( name )

        # use 12mm z-width for all chains
        self.Z0Width = 12.0*mm

        montool = TrigMuonEFMonitoring.TrigMuonEFIDTrackRoiMakerMonitoring()

        self.AthenaMonTools = [ montool ]


class TrigMuonEFTrackIsolationMTConfig (TrigMuonEFConf.TrigMuonEFTrackIsolationAlgMT):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFTrackIsolationMTConfig" ):
        super( TrigMuonEFTrackIsolationMTConfig, self ).__init__( name )

        # configure the isolation tool
        TMEF_IsolationTool = TMEF_TrackIsolationTool('TMEF_IsolationTool',useVarIso=True)

        # Isolation tool
        self.OnlineIsolationTool = TMEF_IsolationTool

        # ID tracks
        self.IdTrackParticles = "InDetTrigTrackingxAODCnv_Muon_IDTrig"

        # Only run algo on combined muons
        self.requireCombinedMuon = True

        # Use offline isolation variables
        self.useVarIso = True
        self.MuonContName = "Muons"
        self.ptcone02Name = "Muons.ptcone02"
        self.ptcone03Name = "Muons.ptcone03"
