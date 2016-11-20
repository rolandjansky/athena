# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###############################################################
#
#  MuonIdentification jobOptions file.
#  Control selection and tool configuration for:
#    MuidSA, MuidCB, MuidVX and MuGirl
#
#   In progress: conversion to 3rd chain only
#
#==============================================================


from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from MuonRecExample.MuonRecFlags import muonRecFlags,muonStandaloneFlags
from MuonRecExample.MuonStandaloneFlags import muonStandaloneFlags
from AthenaCommon.AppMgr import ToolSvc
from MuonRecExample.MuonRecUtils import ExtraFlags

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,getService,getServiceClone
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam
from AthenaCommon.DetFlags import DetFlags 
from AthenaCommon.SystemOfUnits import meter

from IOVDbSvc.CondDB import conddb
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec

GeV = 1000
mm = 1

def MuidMaterialAllocator( name='MuidMaterialAllocator', **kwargs): 
    kwargs.setdefault("AggregateMaterial",True)
    kwargs.setdefault("AllowReordering",False)
    kwargs.setdefault("Extrapolator", getPublicTool('AtlasExtrapolator') )
    kwargs.setdefault("SpectrometerExtrapolator", getPublicTool('AtlasExtrapolator'))
    kwargs.setdefault("TrackingGeometrySvc", getService("AtlasTrackingGeometrySvc") )
    return CfgMgr.Trk__MaterialAllocator(name,**kwargs)

# and the fitter
def iPatFitter( name='iPatFitter', **kwargs): 
    kwargs.setdefault("AggregateMaterial",True)
    kwargs.setdefault("FullCombinedFit", True )
    kwargs.setdefault("MaterialAllocator",getPublicTool("MuidMaterialAllocator"))
    return CfgMgr.Trk__iPatFitter(name,**kwargs)

def iPatSLFitter( name='iPatSLFitter', **kwargs): 
    kwargs.setdefault("AggregateMaterial",True)
    kwargs.setdefault("LineFit",True)
    kwargs.setdefault("FullCombinedFit", True )
    kwargs.setdefault("MaterialAllocator",getPublicTool("MuidMaterialAllocator"))
    kwargs.setdefault("LineMomentum", muonStandaloneFlags.straightLineFitMomentum() )
    return CfgMgr.Trk__iPatFitter(name,**kwargs)


# track cleaner configured to use the same fitter
def MuidTrackCleaner( name='MuidTrackCleaner', **kwargs ):
    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("PullCut"     , 5.0)
        kwargs.setdefault("PullCutPhi"  , 10.0)
    else:
        kwargs.setdefault("PullCut"     , 4.0)
        kwargs.setdefault("PullCutPhi"  , 4.0)

    kwargs.setdefault("Fitter"      , getPublicTool("iPatFitter") )
    kwargs.setdefault("SLFitter"    , getPublicTool("iPatSLFitter") )
    return CfgMgr.Muon__MuonTrackCleaner(name,**kwargs)

def OutwardsTrackCleaner( name='OutwardsTrackCleaner', **kwargs ):
    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("PullCut"     , 5.0)
        kwargs.setdefault("PullCutPhi"  , 10.0)
    else:
        kwargs.setdefault("PullCut"     , 4.0)
        kwargs.setdefault("PullCutPhi"  , 4.0)

    kwargs.setdefault("Fitter"      , getPublicTool("MuonCombinedTrackFitter") )
    kwargs.setdefault("SLFitter"    , getPublicTool("iPatSLFitter") )
    return CfgMgr.Muon__MuonTrackCleaner(name,**kwargs)

    
def MuidCaloEnergyParam( name='MuidCaloEnergyParam', **kwargs ):
    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("Cosmics", True )

    return CfgMgr.Rec__MuidCaloEnergyParam(name,**kwargs)


def MuidCaloEnergyToolParam( name='MuidCaloEnergyToolParam', **kwargs ):
    kwargs.setdefault("CaloParamTool", getPublicTool("MuidCaloEnergyParam") )
    kwargs.setdefault("EnergyLossMeasurement",False)
    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("Cosmics", True )

    return CfgMgr.Rec__MuidCaloEnergyTool(name,**kwargs)


def MuidCaloEnergyMeas( name='MuidCaloEnergyMeas', **kwargs ):
    kwargs.setdefault("CaloParamTool", getPublicTool("MuidCaloEnergyParam") )

    if DetFlags.haveRIO.Calo_on():
        from AthenaCommon.AppMgr    import ToolSvc
        import CaloTools.CaloNoiseToolDefault as cntd
        ToolSvc += cntd.CaloNoiseToolDefault()
        kwargs.setdefault("CaloNoiseTool", ToolSvc.CaloNoiseToolDefault )
        kwargs.setdefault("CellContainerLocation" , "AllCalo" )
        kwargs.setdefault("NoiseThresInSigmas"    , 4. )
    else:
        kwargs.setdefault("CaloNoiseTool", '' )
        kwargs.setdefault("UseCaloNoiseTool", False )
    return CfgMgr.Rec__MuidCaloEnergyMeas(name,**kwargs)
           
def MuidCaloEnergyTool( name='MuidCaloEnergyTool', **kwargs ):
    kwargs.setdefault("CaloMeasTool", getPublicTool("MuidCaloEnergyMeas") )
    kwargs.setdefault("CaloParamTool", getPublicTool("MuidCaloEnergyParam") )
    if DetFlags.haveRIO.Calo_on():
        kwargs.setdefault("EnergyLossMeasurement", True )
        kwargs.setdefault("MinFinalEnergy", 1.0*GeV )
        kwargs.setdefault("MinMuonPt", 10.0*GeV )
        kwargs.setdefault("MopParametrization", True )
    else:
        kwargs.setdefault("EnergyLossMeasurement", False )

    if DetFlags.haveRIO.ID_on():
        kwargs.setdefault("TrackIsolation", True )
    else:
        kwargs.setdefault("TrackIsolation", False )

    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("Cosmics", True )

    return CfgMgr.Rec__MuidCaloEnergyTool(name,**kwargs)


def MuidCaloTrackStateOnSurface( name='MuidCaloTrackStateOnSurface', **kwargs ):
    kwargs.setdefault("Propagator", getPublicTool("AtlasRungeKuttaPropagator"))
    kwargs.setdefault("MinRemainingEnergy" , 0.2*GeV )
    kwargs.setdefault("ParamPtCut"         , 3.0*GeV )
    kwargs.setdefault("CaloEnergyDeposit"  , getPublicTool("MuidCaloEnergyTool") )
    kwargs.setdefault("CaloEnergyParam"  , getPublicTool("MuidCaloEnergyToolParam") )
    return CfgMgr.Rec__MuidCaloTrackStateOnSurface(name,**kwargs)

def MuidCaloTrackStateOnSurfaceParam( name='MuidCaloTrackStateOnSurfaceParam', **kwargs ):
    kwargs.setdefault("Propagator", getPublicTool("AtlasRungeKuttaPropagator"))
    kwargs.setdefault("MinRemainingEnergy" , 0.2*GeV )
    kwargs.setdefault("ParamPtCut"         , 3.0*GeV )
    kwargs.setdefault("CaloEnergyDeposit"  , getPublicTool("MuidCaloEnergyToolParam") )
    kwargs.setdefault("CaloEnergyParam"  , getPublicTool("MuidCaloEnergyToolParam") )
    return CfgMgr.Rec__MuidCaloTrackStateOnSurface(name,**kwargs)

def MuidMaterialEffectsOnTrackProvider( name='MuidMaterialEffectsOnTrackProvider', **kwargs ):
    kwargs.setdefault("TSOSTool",      getPublicTool("MuidCaloTrackStateOnSurface") )
    kwargs.setdefault("TSOSToolParam", getPublicTool("MuidCaloTrackStateOnSurfaceParam") )
    if jobproperties.Beam.beamType()=='cosmics':
        kwargs.setdefault("Cosmics",True)
    return CfgMgr.Rec__MuidMaterialEffectsOnTrackProvider(name,**kwargs)

def MuidMaterialEffectsOnTrackProviderParam( name='MuidMaterialEffectsOnTrackProviderParam', **kwargs ):
    kwargs.setdefault("TSOSTool",      getPublicTool("MuidCaloTrackStateOnSurfaceParam") )
    kwargs.setdefault("TSOSToolParam", getPublicTool("MuidCaloTrackStateOnSurfaceParam") )
    if jobproperties.Beam.beamType()=='cosmics':
        kwargs.setdefault("Cosmics",True)
    return CfgMgr.Rec__MuidMaterialEffectsOnTrackProvider(name,**kwargs)


def MuonCombinedPropagator( name='MuonCombinedPropagator', **kwargs ):
    kwargs.setdefault("AccuracyParameter",   .000001 )
    kwargs.setdefault("IncludeBgradients",   True )
    kwargs.setdefault("MaxStraightLineStep", .001 )
    kwargs.setdefault("MaxHelixStep",        .001 )
    return CfgMgr.Trk__RungeKuttaPropagator(name,**kwargs)


def MuonTrackQuery( name="MuonTrackQuery", **kwargs ):
     kwargs.setdefault("MdtRotCreator",   getPublicTool("MdtDriftCircleOnTrackCreator") )
     kwargs.setdefault("Fitter",          getPublicTool("CombinedMuonTrackBuilder") )
     return CfgMgr.Rec__MuonTrackQuery(name,**kwargs)

def MuidSegmentRegionRecoveryTool( name ='MuidSegmentRegionRecoveryTool', **kwargs ):
    kwargs.setdefault("Fitter",  getPublicTool("CombinedMuonTrackBuilderFit") )
    return CfgMgr.Muon__MuonSegmentRegionRecoveryTool(name,**kwargs)

        
def CombinedMuonTrackBuilderFit( name='CombinedMuonTrackBuilderFit', **kwargs ):
    import MuonCombinedRecExample.CombinedMuonTrackSummary
    from AthenaCommon.AppMgr    import ToolSvc
    kwargs.setdefault("CaloEnergyParam"               , getPublicTool("MuidCaloEnergyToolParam") )
    kwargs.setdefault("CaloTSOS"                      , getPublicTool("MuidCaloTrackStateOnSurface") )
    kwargs.setdefault("CscRotCreator"                 , getPublicTool("CscClusterOnTrackCreator") )
    kwargs.setdefault("Fitter"                        , getPublicTool("iPatFitter") )
    kwargs.setdefault("SLFitter"                      , getPublicTool("iPatSLFitter") )
    kwargs.setdefault("MaterialAllocator"             , getPublicTool("MuidMaterialAllocator") )
    kwargs.setdefault("MdtRotCreator"                 , getPublicTool("MdtDriftCircleOnTrackCreator") )
    kwargs.setdefault("Propagator"                    , getPublicTool("MuonCombinedPropagator") )
    kwargs.setdefault("SLPropagator"                  , getPublicTool("MuonCombinedPropagator") )
    kwargs.setdefault("CleanCombined"                 , True )
    kwargs.setdefault("CleanStandalone"               , True )
    kwargs.setdefault("BadFitChi2"                    , 2.5 )
    kwargs.setdefault("LargeMomentumError"            , 0.5 )
    kwargs.setdefault("LineMomentum"                  , muonStandaloneFlags.straightLineFitMomentum() )
    kwargs.setdefault("LowMomentum"                   , 10.*GeV )
    kwargs.setdefault("MinEnergy"                     , 0.3*GeV )
    kwargs.setdefault("PerigeeAtSpectrometerEntrance" , True )
    kwargs.setdefault("ReallocateMaterial"            , False )
    kwargs.setdefault("Vertex2DSigmaRPhi"             , 100.*mm )
    kwargs.setdefault("Vertex3DSigmaRPhi"             , 6.*mm )
    kwargs.setdefault("Vertex3DSigmaZ"                , 60.*mm)
    kwargs.setdefault("TrackSummaryTool"              , ToolSvc.CombinedMuonTrackSummary )
    kwargs.setdefault("UseCaloTG"                     , False )

    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("MdtRotCreator" ,  "" )
        kwargs.setdefault("LowMomentum"   ,  1.5*GeV )
        kwargs.setdefault("ReallocateMaterial", False )
        kwargs.setdefault("Vertex2DSigmaRPhi" , 100.*mm )
        kwargs.setdefault("Vertex3DSigmaRPhi" , 100.*mm )
        kwargs.setdefault("Vertex3DSigmaZ"    ,  1.*meter )

    if muonRecFlags.doSegmentT0Fit():
        kwargs.setdefault("MdtRotCreator"                 , "" )

    getPublicTool("MuonCaloParticleCreator")
    return CfgMgr.Rec__CombinedMuonTrackBuilder(name,**kwargs)

def CombinedMuonTrackBuilder( name='CombinedMuonTrackBuilder', **kwargs ):
    import MuonCombinedRecExample.CombinedMuonTrackSummary
    from AthenaCommon.AppMgr    import ToolSvc
    kwargs.setdefault("CaloEnergyParam"               , getPublicTool("MuidCaloEnergyToolParam") )
    kwargs.setdefault("CaloTSOS"                      , getPublicTool("MuidCaloTrackStateOnSurface") )
    kwargs.setdefault("CscRotCreator"                 , getPublicTool("CscClusterOnTrackCreator") )
    kwargs.setdefault("Fitter"                        , getPublicTool("iPatFitter") )
    kwargs.setdefault("SLFitter"                      , getPublicTool("iPatSLFitter") )
    kwargs.setdefault("MaterialAllocator"             , getPublicTool("MuidMaterialAllocator") )
    kwargs.setdefault("MdtRotCreator"                 , getPublicTool("MdtDriftCircleOnTrackCreator") )
    kwargs.setdefault("MuonHoleRecovery"              , getPublicTool("MuidSegmentRegionRecoveryTool") )
    kwargs.setdefault("Propagator"                    , getPublicTool("MuonCombinedPropagator") )
    kwargs.setdefault("SLPropagator"                  , getPublicTool("MuonCombinedPropagator") )
    kwargs.setdefault("CleanCombined"                 , True )
    kwargs.setdefault("CleanStandalone"               , True )
    kwargs.setdefault("BadFitChi2"                    , 2.5 )
    kwargs.setdefault("LargeMomentumError"            , 0.5 )
    kwargs.setdefault("LineMomentum"                  , muonStandaloneFlags.straightLineFitMomentum() )
    kwargs.setdefault("LowMomentum"                   , 10.*GeV )
    kwargs.setdefault("MinEnergy"                     , 0.3*GeV )
    kwargs.setdefault("PerigeeAtSpectrometerEntrance" , False )
    kwargs.setdefault("ReallocateMaterial"            , False )
    kwargs.setdefault("Vertex2DSigmaRPhi"             , 100.*mm )
    kwargs.setdefault("Vertex3DSigmaRPhi"             , 6.*mm )
    kwargs.setdefault("Vertex3DSigmaZ"                , 60.*mm)
    kwargs.setdefault("TrackSummaryTool"              , ToolSvc.CombinedMuonTrackSummary )
    kwargs.setdefault("UseCaloTG"                     , True ) #
    
    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("MdtRotCreator" ,  "" )
        kwargs.setdefault("LowMomentum"   ,  1.5*GeV )
        kwargs.setdefault("ReallocateMaterial", False )
        kwargs.setdefault("Vertex2DSigmaRPhi" , 100.*mm )
        kwargs.setdefault("Vertex3DSigmaRPhi" , 100.*mm )
        kwargs.setdefault("Vertex3DSigmaZ"    ,  1.*meter )

    # configure tools for data reprocessing 
    if muonRecFlags.enableErrorTuning():
       # use alignment effects on track for all algorithms

       useAlignErrs = True
       if conddb.dbdata == 'COMP200' or conddb.dbmc == 'COMP200' or 'HLT' in globalflags.ConditionsTag() or conddb.isOnline :
            useAlignErrs = False

       kwargs.setdefault("MuonErrorOptimizer", getPublicToolClone("MuidErrorOptimisationTool",
                                                                  "MuonErrorOptimisationTool",
                                                                  PrepareForFit              = False,
                                                                  RecreateStartingParameters = False,
                                                                  RefitTool = getPublicToolClone("MuidRefitTool",
                                                                                                 "MuonRefitTool",
                                                                  				 AlignmentErrors = useAlignErrs,
                                                                                                 Fitter = getPublicTool("iPatFitter"))))


    if muonRecFlags.doSegmentT0Fit():
        kwargs.setdefault("MdtRotCreator"                 , "" )
    getPublicTool("MuonCaloParticleCreator")
    return CfgMgr.Rec__CombinedMuonTrackBuilder(name,**kwargs)


def MuonMatchQuality(name='MuonMatchQuality', **kwargs ):
    kwargs.setdefault("TagTool", getPublicTool("CombinedMuonTagTestTool") )
    return CfgMgr.Rec__MuonMatchQuality(name,**kwargs)

def MuidMuonRecovery(name='MuidMuonRecovery',**kwargs):
    kwargs.setdefault("Extrapolator", getPublicTool("AtlasExtrapolator") )
    kwargs.setdefault("TrackBuilder", getPublicTool("CombinedMuonTrackBuilder") )
    return CfgMgr.Rec__MuidMuonRecovery(name,**kwargs)

def MuonCombinedTrackFitter( name="MuonCombinedTrackFitter", **kwargs ):
    kwargs.setdefault("ExtrapolationTool"     , getPublicTool("AtlasExtrapolator") )
    kwargs.setdefault("NavigatorTool"         , getPublicTool("MuonNavigator") )
    kwargs.setdefault("PropagatorTool"        , getPublicTool("MuonCombinedPropagator") )
    kwargs.setdefault("RotCreatorTool"        , getPublicTool("MuonRotCreator") )
    kwargs.setdefault("MeasurementUpdateTool" , getPublicTool("MuonMeasUpdator") )
    kwargs.setdefault("TrackingGeometrySvc"   , getService("AtlasTrackingGeometrySvc") )
    kwargs.setdefault("ExtrapolatorMaterial"  , True )
    kwargs.setdefault("MuidTool"              , getPublicTool("MuidMaterialEffectsOnTrackProvider") )
    kwargs.setdefault("MuidToolParam"         , None )
    if jobproperties.Beam.beamType()=='collisions':
        kwargs.setdefault("MuidToolParam"     , getPublicTool("MuidMaterialEffectsOnTrackProviderParam" ) )
    kwargs.setdefault("MuidMat"               , True )
    kwargs.setdefault("StraightLine"          , not jobproperties.BField.solenoidOn() and not jobproperties.BField.allToroidOn() )
    kwargs.setdefault("MaxIterations"         , 50 )
    kwargs.setdefault("GetMaterialFromTrack"  ,  jobproperties.BField.solenoidOn() and jobproperties.BField.allToroidOn() )
    kwargs.setdefault("RecalculateDerivatives", False)
    kwargs.setdefault("UseCaloTG"             , True) #
    return CfgMgr.Trk__GlobalChi2Fitter(name,**kwargs)

def CombinedMuonTagTestTool( name='CombinedMuonTagTestTool', **kwargs ):
    kwargs.setdefault("ExtrapolatorTool",getPublicTool("AtlasExtrapolator") )
    kwargs.setdefault("TrackingGeometrySvc",  getService("AtlasTrackingGeometrySvc") )
    kwargs.setdefault("Chi2Cut",50000.)
    return CfgMgr.MuonCombined__MuonTrackTagTestTool(name,**kwargs)


def OutwardsSegmentRegionRecoveryTool( name ='OutwardsSegmentRegionRecoveryTool', **kwargs ):
    kwargs.setdefault("Fitter",  getPublicTool("MuonCombinedTrackFitter") )
    return CfgMgr.Muon__MuonSegmentRegionRecoveryTool(name,**kwargs)

def OutwardsCombinedMuonTrackBuilder( name = 'OutwardsCombinedMuonTrackBuilder', **kwargs ):
    import MuonCombinedRecExample.CombinedMuonTrackSummary
    from AthenaCommon.AppMgr    import ToolSvc
    kwargs.setdefault("Cleaner", getPublicTool("OutwardsTrackCleaner") )
    kwargs.setdefault("Fitter",  getPublicTool("MuonCombinedTrackFitter") )
    kwargs.setdefault("TrackSummaryTool"     , ToolSvc.CombinedMuonTrackSummary )
    kwargs.setdefault("MuonHoleRecovery"     , getPublicTool("OutwardsSegmentRegionRecoveryTool") )
    kwargs.setdefault("AllowCleanerVeto"     , False)
    if muonRecFlags.enableErrorTuning():
       kwargs.setdefault("MuonErrorOptimizer", getPublicToolClone("OutwardsErrorOptimisationTool", "MuidErrorOptimisationTool",
                                                                  PrepareForFit=False,RecreateStartingParameters=False,
                                                                  RefitTool = getPublicToolClone("OutwardsRefitTool",
                                                                                                 "MuonRefitTool",
                                                                  				 AlignmentErrors = False,
                                                                                                 Fitter = getPublicTool("MuonCombinedTrackFitter"))))

    return CfgMgr.Rec__OutwardsCombinedMuonTrackBuilder(name,**kwargs)
    # tools for ID/MS match quality and recovery of incorrect spectrometer station association	
