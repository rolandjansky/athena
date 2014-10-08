# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# TrigMuonEF configurables
#
from TrigMuonEF.TrigMuonEFConf import *
from TriggerJobOpts.TriggerFlags  import TriggerFlags
from TrigMuonEF.TrigMuonEFMonitoring import *

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr 
from AthenaCommon import CfgGetter
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.SystemOfUnits import GeV,mm

from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig


from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

#Offline calorimeter isolation tool
from TrackInCaloTools import TrackInCaloTools
TMEFTrackInCaloTools = TrackInCaloTools.TrackInCaloTools(name='MuonIsoTrackInCaloTools')
ToolSvc += TMEFTrackInCaloTools

from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
TMEFCaloIsolationTool = xAOD__CaloIsolationTool(name='MuonIsoCaloIsolationTool',
                                                ExtrapolTrackToCaloTool = TMEFTrackInCaloTools)
ToolSvc += TMEFCaloIsolationTool


if not hasattr(ServiceMgr,"TrackingVolumesSvc"):
    from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingVolumesSvc
    ServiceMgr += Trk__TrackingVolumesSvc("TrackingVolumesSvc",BuildVolumesFromTagInfo = False)

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


def TMEF_CaloEnergyMeas(name='TMEF_CaloEnergyMeas',**kwargs):
    kwargs.setdefault("CaloNoiseTool","")
    kwargs.setdefault("UseCaloNoiseTool",False)
    return CfgMgr.Rec__MuidCaloEnergyMeas(name,**kwargs)


def TMEF_CaloEnergyTool(name='TMEF_CaloEnergyTool', **kwargs):
    kwargs.setdefault("CaloMeasTool","TMEF_CaloEnergyMeas")
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
    kwargs.setdefault("MuonSummaryHelperTool", "MuonTrackSummaryHelper")
    kwargs.setdefault("doSharedHits", False)
    # only add ID tool if ID is on
    if DetFlags.detdescr.ID_on():
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool, InDetTrigHoleSearchTool
        kwargs.setdefault("InDetSummaryHelperTool", InDetTrigTrackSummaryHelperTool)
        kwargs.setdefault("InDetHoleSearchTool", InDetTrigHoleSearchTool)

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
    kwargs.setdefault("SpectrometerExtrapolator", "TMEF_Extrapolator")
    kwargs.setdefault("TrackingGeometrySvc", AtlasTrackingGeometrySvc)    
    return CfgMgr.Trk__MaterialAllocator(name,**kwargs)



def TMEF_iPatFitter(name='TMEF_iPatFitter',**kwargs):
    kwargs.setdefault("AggregateMaterial", True)
    kwargs.setdefault("FullCombinedFit", True)
    kwargs.setdefault("MaterialAllocator", "TMEF_MaterialAllocator")
    return CfgMgr.Trk__iPatFitter(name,**kwargs)


def TMEF_iPatSLFitter(name='TMEF_iPatSLFitter',**kwargs):
    from MuonRecExample.MuonRecFlags import mooreFlags
    kwargs.setdefault("LineFit", True)
    kwargs.setdefault("LineMomentum", mooreFlags.straightLineFitMomentum())
    # call the other factory function
    return TMEF_iPatFitter(name,**kwargs)


def TMEF_TrackCleaner(name = 'TMEF_TrackCleaner',**kwargs):
    kwargs.setdefault("PullCut", 3.0)
    kwargs.setdefault("PullCutPhi", 3.0)
    kwargs.setdefault("Fitter", "TMEF_iPatFitter")
    kwargs.setdefault("SLFitter", "TMEF_iPatSLFitter")
    from MuonRecExample.MuonRecFlags import muonRecFlags
    if muonRecFlags.doSegmentT0Fit():
        kwargs.setdefault("RecoverOutliers", False)
    return CfgMgr.Muon__MuonTrackCleaner(name,**kwargs)
        

def TMEF_CombinedMuonTrackBuilder(name='TMEF_CombinedMuonTrackBuilder',**kwargs):
    from MuonRecExample.MuonRecFlags import mooreFlags
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
    kwargs.setdefault("LineMomentum", mooreFlags.straightLineFitMomentum())
    kwargs.setdefault("LowMomentum", 10.*GeV)
    kwargs.setdefault("MinEnergy", 0.3*GeV)
    kwargs.setdefault("PerigeeAtSpectrometerEntrance", True)
    kwargs.setdefault("ReallocateMaterial", False)
    kwargs.setdefault("Vertex2DSigmaRPhi", 100.*mm)
    kwargs.setdefault("Vertex3DSigmaRPhi", 6.*mm)
    kwargs.setdefault("Vertex3DSigmaZ", 60.*mm)

    # note - the TrackSummaryTool is done as follows offline:
    # import MuonCombinedRecExample.CombinedMuonTrackSummary
    # combinedMuonTrackBuilder.TrackSummaryTool = ToolSvc.CombinedMuonTrackSummary
    # should check our config is really ok here
    kwargs.setdefault("TrackSummaryTool", 'TMEF_TrackSummaryTool')

    # extra w.r.t. Muid Offline
    kwargs.setdefault("Cleaner", "TMEF_TrackCleaner")
    from MuonRecExample.MuonRecFlags import muonRecFlags
    if muonRecFlags.doSegmentT0Fit():
        kwargs.setdefault("MdtRotCreator", "")

    return CfgMgr.Rec__CombinedMuonTrackBuilder(name,**kwargs)


def TMEF_MuonTrackQuery(name='TMEF_MuonTrackQuery',**kwargs):
    kwargs.setdefault("MdtRotCreator","MdtDriftCircleOnTrackCreator")
    kwargs.setdefault("Fitter", 'TMEF_CombinedMuonTrackBuilder')
    return CfgMgr.Rec__MuonTrackQuery(name,**kwargs)


def TMEF_MatchMaker(name='TMEF_MatchMaker',**kwargs):
    kwargs.setdefault("AmbiguityProcessor", "MuonAmbiProcessor")
    kwargs.setdefault("MatchQuality", "TMEF_MatchQuality")
    kwargs.setdefault("CaloTSOS", "TMEF_CaloTrackStateOnSurface") # not in Muid?
#    kwargs.setdefault("OutwardsTrackBuilder", "TMEF_OutwardsCombinedMuonTrackBuilder") # extra in Muid (not yet configured here)
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
    sumtool.InDetHoleSearchTool = None
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

def TMEF_OutwardsMuonTrackCleaner(name='TMEF_OutwardsMuonTrackCleaner',**kwargs):
    kwargs.setdefault('PullCut',    3.0)
    kwargs.setdefault('PullCutPhi', 3.0)
    kwargs.setdefault('Fitter',     'TMEF_MuonCombinedTrackFitter')
    kwargs.setdefault('SLFitter',   'TMEF_iPatSLFitter')
    return CfgMgr.Muon__MuonTrackCleaner(name, **kwargs)

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
    from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
    return  Trk__GlobalChi2Fitter(name, **kwargs)

def TMEF_OutwardsCombinedMuonTrackBuilder(name='TMEF_OutwardsCombinedMuonTrackBuilder',**kwargs):
    kwargs.setdefault('Cleaner', 'TMEF_OutwardsMuonTrackCleaner')
    kwargs.setdefault('Fitter',  'TMEF_MuonCombinedTrackFitter')
    kwargs.setdefault('TrackSummaryTool', 'TMEF_TrackSummaryTool')#gpt
    kwargs.setdefault('MuonHoleRecovery', 'MuonSegmentRegionRecoveryTool')#gpt
    kwargs.setdefault('AllowCleanerVeto', False)
    from MuidTrackBuilder.MuidTrackBuilderConf import Rec__OutwardsCombinedMuonTrackBuilder
    return Rec__OutwardsCombinedMuonTrackBuilder(name, **kwargs)

def TMEF_MuonCombinedFitTagTool(name="TMEF_MuonCombinedFitTagTool",**kwargs):
    kwargs.setdefault("TrackBuilder",         'TMEF_CombinedMuonTrackBuilder' )
    kwargs.setdefault("OutwardsTrackBuilder", 'TMEF_OutwardsCombinedMuonTrackBuilder')
    kwargs.setdefault("TrackQuery",           'TMEF_MuonTrackQuery' )
    kwargs.setdefault("MuonRecovery",         'TMEF_MuidMuonRecovery' )
    kwargs.setdefault("MatchQuality",         'TMEF_MatchQuality' )
    return CfgMgr.MuonCombined__MuonCombinedFitTagTool(name,**kwargs)

def TMEF_MuonCandidateTool(name="TMEF_MuonCandidateTool",**kwargs):
    kwargs.setdefault("TrackBuilder","TMEF_CombinedMuonTrackBuilder")
    return CfgMgr.MuonCombined__MuonCandidateTool(name,**kwargs)

def TMEF_MuonCreatorTool(name="TMEF_MuonCreatorTool",**kwargs):
    kwargs.setdefault('TrackParticleCreator','TMEF_TrkToTrackParticleConvTool')
    kwargs.setdefault('TrackIsolationTool',None)
    kwargs.setdefault('CaloIsolationTool',None)
    kwargs.setdefault('MakeTrackAtMSLink',True)
    return CfgMgr.MuonCombined__MuonCreatorTool(name,**kwargs)

# TrigMuonEF classes
class TrigMuonEFTrackBuilderConfig ():
    __slots__ = ()

    def __init__( self, name="TrigMuonEFTrackBuilder" ):

        raise RuntimeError("TrigMuonEFTrackBuilder no longer supported in release 19 (or later) due to removal of MooTrackSteering from the release.")
        
class TrigMuonEFCombinerConfig ():
    __slots__ = ()

    def __init__( self, name="TrigMuonEFCombiner" ):
        raise RuntimeError("TrigMuonEFCombinerConfig no longer supported - please switch to combiner mode of TrigMuSuperEF")


  
class TrigMuonEFStandaloneTrackToolConfig (TrigMuonEFStandaloneTrackTool):
    __slots__ = ()
    
    def __init__( self, name="TrigMuonEFStandaloneTrackTool", **kwargs ):
        super( TrigMuonEFStandaloneTrackToolConfig, self ).__init__( name, **kwargs )
        
        self.CscClusterProvider = CfgGetter.getPublicTool("CscThresholdClusterBuilderTool")

        self.SegmentsFinderTool = CfgGetter.getPrivateToolClone( "TMEF_SegmentsFinderTool","MooSegmentFinder",
                                                                     WriteIntermediateResults = False)
        self.SegmentsFinderTool.HoughPatternFinder = "Muon::MuonHoughPatternFinderTool/MuonHoughPatternFinderTool"
        self.SegmentsFinderTool.HoughPatternFinder.RecordAll = False
        self.SegmentsFinderTool.HoughPatternFinder.muonCombinePatternTool ="MuonCombinePatternTool" 

        if (TriggerFlags.MuonSlice.doEFRoIDrivenAccess()):
            self.useRoIDrivenDataAccess  = True

        # ROB based decoding - off for now, should be enabled once tested
        self.useRobBasedRoiDecoding = False

        from MuonRecExample.MuonRecFlags import muonRecFlags
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
        self.doCache = False

        self.TrackBuilderTool  = "TMEF_TrackBuilderTool"
        self.TrkSummaryTool = "TMEF_TrackSummaryTool"                
        self.MuonCandidateTool = "TMEF_MuonCandidateTool"

        self.TrackToTrackParticleConvTool = "MuonParticleCreatorTool"

class TrigMuonEFCombinerConfigTRTOnly ():
    __slots__ = ()

    def __init__( self, name="TrigMuonEFCombinerTRTOnly" ):
        raise RuntimeError("TrigMuonEFCombinerTRTOnly no longer supported - please switch to combiner mode of TrigMuSuperEF")


class TrigMuonEFCaloIsolationConfig (TrigMuonEFCaloIsolation):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFCaloIsolationConfig" ):
        super( TrigMuonEFCaloIsolationConfig, self ).__init__( name )

        self.RequireCombinedMuon = True

        # Isolation tools
        self.TrackInCaloTool   = TMEFTrackInCaloTools
        self.CaloIsolationTool = TMEFCaloIsolationTool

        #Better to clone?
        #self.CaloIsolationTool = CfgGetter.getPublicToolClone("xAOD__CaloIsolationTriggerTool",
        #                                                      "xAOD__CaloIsolationTool",
        #                                                      ExtrapolTrackToCaloTool = TMEFTrackInCaloTools)



        # histograms
        self.HistoPathBase = ""
        #validation_caloiso = TrigMuonEFCaloIsolationValidationMonitoring()
        #online_caloiso     = TrigMuonEFCaloIsolationOnlineMonitoring()
        monitoringCaloIso = TrigMuonEFCaloIsolationMonitoring()

        self.AthenaMonTools = [ monitoringCaloIso ]

class TrigMuonEFTrackIsolationConfig (TrigMuonEFTrackIsolation):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFTrackIsolationConfig" ):
        super( TrigMuonEFTrackIsolationConfig, self ).__init__( name )

        # configure the isolation tool
        TMEF_IsolationTool = TrigMuonEFTrackIsolationTool(name = 'TMEF_IsolationTool',
                                                 deltaZCut = 6.0*mm, 
                                                 removeSelf=True,
                                                 useAnnulus=False) 

        # Isolation tool
        self.IsolationTool = TMEF_IsolationTool

        # ID tracks
        #self.IdTrackParticles = "InDetTrigParticleCreation_FullScan_EFID"
        #self.IdTrackParticles = "InDetTrigParticleCreation_MuonIso_EFID"
        self.IdTrackParticles = "InDetTrigTrackingxAODCnv_MuonIso_EFID"

        # Only run algo on combined muons
        self.requireCombinedMuon = True

        # histograms
        self.histoPathBase = ""
        validation_trkiso = TrigMuonEFTrackIsolationValidationMonitoring()
        online_trkiso     = TrigMuonEFTrackIsolationOnlineMonitoring()

        self.AthenaMonTools = [ validation_trkiso, online_trkiso ]


class TrigMuonEFTrackIsolationAnnulusConfig (TrigMuonEFTrackIsolation):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFTrackIsolationAnnulusConfig" ):
        super( TrigMuonEFTrackIsolationAnnulusConfig, self ).__init__( name )

        # configure the isolation tool
        TMEF_AnnulusIsolationTool = TrigMuonEFTrackIsolationTool(name = 'TMEF_AnnulusIsolationTool',
                                                 deltaZCut = 6.0*mm, 
                                                 removeSelf=True,
                                                 useAnnulus=True,
                                                 annulusSize=0.1) 

        # Isolation tool
        self.IsolationTool = TMEF_AnnulusIsolationTool

        # ID tracks
        #self.IdTrackParticles = "InDetTrigParticleCreation_FullScan_EFID"
        self.IdTrackParticles = "InDetTrigParticleCreation_MuonIso_EFID"

        # Only run algo on combined muons
        self.requireCombinedMuon = True

        # histograms
        self.histoPathBase = ""
        validation_trkiso = TrigMuonEFTrackIsolationValidationMonitoring()
        online_trkiso     = TrigMuonEFTrackIsolationOnlineMonitoring()

        self.AthenaMonTools = [ validation_trkiso, online_trkiso ]


def InDetTrkRoiMaker_Muon(name="InDetTrkRoiMaker_Muon",**kwargs):
    kwargs.setdefault("FullScanMode", "HybridScan")
    kwargs.setdefault("SeedsIDalgo", "Stratety_F")
    kwargs.setdefault("SeedsEtaMax", 2.5)
    kwargs.setdefault("SeedsPtMin", 3.0) # GeV
    kwargs.setdefault("FullEtaRange", 3.0) # MS goes up to 2.7
    kwargs.setdefault("AthenaMonTools", [ InDetTrkRoiMakerMonitoring("Monitoring"),
                                          TrigTimeHistToolConfig("Time", TimerHistLimits = [0, 10], NumberOfHistBins = 100) ] )

    return CfgMgr.InDetTrkRoiMaker(name,**kwargs)


class TrigMuonEFRoiAggregatorConfig (TrigMuonEFRoiAggregator):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFRoiAggregator" ):
        super( TrigMuonEFRoiAggregator, self ).__init__( name )


class TrigMuonEFFSRoiMakerConfig(TrigMuonEFFSRoiMaker):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFFSRoiMaker" ):
        super( TrigMuonEFFSRoiMakerConfig, self ).__init__( name )

        self.MuonPtCut = 0.0
        self.OutputContName = "MuonEFInfo"

        montool     = TrigMuonEFFSRoiMakerMonitoring()
        
        self.AthenaMonTools = [ montool ]

        pass

                             
