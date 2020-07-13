# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
from TriggerJobOpts.TriggerFlags import TriggerFlags

GeV = 1000
mm = 1

def MuidMaterialAllocator( name='MuidMaterialAllocator', **kwargs): 
    kwargs.setdefault("AggregateMaterial",True)
    kwargs.setdefault("AllowReordering",False)
    kwargs.setdefault("Extrapolator", getPublicTool('AtlasExtrapolator') )
    kwargs.setdefault("TrackingGeometrySvc", getService("AtlasTrackingGeometrySvc") )
    return CfgMgr.Trk__MaterialAllocator(name,**kwargs)

# and the fitter
def iPatFitter( name='iPatFitter', **kwargs): 
    kwargs.setdefault("AggregateMaterial",True)
    kwargs.setdefault("FullCombinedFit", True )
    kwargs.setdefault("MaterialAllocator",getPublicTool("MuidMaterialAllocator"))
    from InDetRecExample import TrackingCommon
    kwargs.setdefault("SolenoidalIntersector",TrackingCommon.getSolenoidalIntersector())
    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("MaxIterations", 15)
    else:
        import MuonCombinedRecExample.CombinedMuonTrackSummary
        from AthenaCommon.AppMgr import ToolSvc
        kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary)

    return CfgMgr.Trk__iPatFitter(name,**kwargs)

def iPatSLFitter( name='iPatSLFitter', **kwargs): 
    kwargs.setdefault("AggregateMaterial",True)
    kwargs.setdefault("LineFit",True)
    kwargs.setdefault("FullCombinedFit", True )
    kwargs.setdefault("MaterialAllocator",getPublicTool("MuidMaterialAllocator"))
    kwargs.setdefault("LineMomentum", muonStandaloneFlags.straightLineFitMomentum() )
    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("MaxIterations", 15)
    else:
        import MuonCombinedRecExample.CombinedMuonTrackSummary
        from AthenaCommon.AppMgr import ToolSvc
        kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary)

    return CfgMgr.Trk__iPatFitter(name,**kwargs)


# track cleaner configured to use the same fitter
def MuidTrackCleaner( name='MuidTrackCleaner', **kwargs ):
    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("PullCut"     , 5.0)
        kwargs.setdefault("PullCutPhi"  , 10.0)
    else:
        kwargs.setdefault("PullCut"     , 4.0)
        kwargs.setdefault("PullCutPhi"  , 4.0)

    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("Iterate", False)
        kwargs.setdefault("RecoverOutliers", False)

    kwargs.setdefault("Fitter"      , getPublicTool("iPatFitter") )
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
        kwargs.setdefault("CellContainerLocation" , "AllCalo" )
        kwargs.setdefault("NoiseThresInSigmas"    , 4. )
    return CfgMgr.Rec__MuidCaloEnergyMeas(name,**kwargs)
           
def MuidCaloEnergyTool( name='MuidCaloEnergyTool', **kwargs ):
    kwargs.setdefault("CaloMeasTool", getPublicTool("MuidCaloEnergyMeas") )
    kwargs.setdefault("CaloParamTool", getPublicTool("MuidCaloEnergyParam") )
    if DetFlags.haveRIO.Calo_on() and not TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("EnergyLossMeasurement", True )
        kwargs.setdefault("MinFinalEnergy", 1.0*GeV )
        kwargs.setdefault("MinMuonPt", 10.0*GeV )
        kwargs.setdefault("MopParametrization", True )
    else:
        kwargs.setdefault("EnergyLossMeasurement", False )

    if DetFlags.haveRIO.ID_on() and not TriggerFlags.MuonSlice.doTrigMuonConfig:
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
    kwargs.setdefault("MaxHelixStep",        .001 )
    kwargs.setdefault("MaxStraightLineStep", .001 )
    return CfgMgr.Trk__RungeKuttaPropagator(name,**kwargs)


def MuonTrackQuery( name="MuonTrackQuery", **kwargs ):
     kwargs.setdefault("MdtRotCreator",   getPublicTool("MdtDriftCircleOnTrackCreator") )
     kwargs.setdefault("Fitter", getPublicTool("iPatFitter"))
     return CfgMgr.Rec__MuonTrackQuery(name,**kwargs)

def MuidSegmentRegionRecoveryTool( name ='MuidSegmentRegionRecoveryTool', **kwargs ):
    kwargs.setdefault("Builder",  getPublicTool("CombinedMuonTrackBuilderFit") )
    import MuonCombinedRecExample.CombinedMuonTrackSummary
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary)

    from RegionSelector.RegSelToolConfig import makeRegSelTool_MDT, makeRegSelTool_RPC, makeRegSelTool_TGC
    kwargs.setdefault("MDTRegionSelector", makeRegSelTool_MDT())
    kwargs.setdefault("RPCRegionSelector", makeRegSelTool_RPC())
    kwargs.setdefault("TGCRegionSelector", makeRegSelTool_TGC())
    if MuonGeometryFlags.hasCSC():
        from RegionSelector.RegSelToolConfig import makeRegSelTool_CSC
        kwargs.setdefault("CSCRegionSelector", makeRegSelTool_CSC())
    else:
        kwargs.setdefault("CSCRegionSelector", "")
    if MuonGeometryFlags.hasSTGC():
        from RegionSelector.RegSelToolConfig import makeRegSelTool_sTGC
        kwargs.setdefault("STGCRegionSelector", makeRegSelTool_sTGC())
    else:
        kwargs.setdefault("STGCRegionSelector", "")
    if MuonGeometryFlags.hasMM():
        from RegionSelector.RegSelToolConfig import makeRegSelTool_MM
        kwargs.setdefault("MMRegionSelector", makeRegSelTool_MM())
    else:
        kwargs.setdefault("MMRegionSelector", "")

    return CfgMgr.Muon__MuonSegmentRegionRecoveryTool(name,**kwargs)


def MuonMaterialProviderTool( name = "MuonMaterialProviderTool"):
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool, Rec__MuonCaloEnergyTool, Rec__ParticleCaloCellAssociationTool
    from TrkMaterialProvider.TrkMaterialProviderConf import Trk__TrkMaterialProviderTool
    caloCellAssociationTool = Rec__ParticleCaloCellAssociationTool(ParticleCaloExtensionTool = getPublicTool("MuonParticleCaloExtensionTool"))
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += caloCellAssociationTool
  
    muonCaloEnergyTool = Rec__MuonCaloEnergyTool(ParticleCaloExtensionTool = getPublicTool("MuonParticleCaloExtensionTool"),
                                                 ParticleCaloCellAssociationTool = caloCellAssociationTool)

    ToolSvc += muonCaloEnergyTool
    materialProviderTool = Trk__TrkMaterialProviderTool(MuonCaloEnergyTool = muonCaloEnergyTool);
    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        materialProviderTool.UseCaloEnergyMeasurement = False
    return materialProviderTool

def CombinedMuonTrackBuilderFit( name='CombinedMuonTrackBuilderFit', **kwargs ):
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("CaloEnergyParam"               , getPublicTool("MuidCaloEnergyToolParam") )
    kwargs.setdefault("CaloTSOS"                      , getPublicTool("MuidCaloTrackStateOnSurface") )
    kwargs.setdefault("MaterialAllocator"             , getPublicTool("MuidMaterialAllocator") )
    kwargs.setdefault("MdtRotCreator"                 , getPublicTool("MdtDriftCircleOnTrackCreator") )
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
    kwargs.setdefault("UseCaloTG"                     , False )
    kwargs.setdefault("CaloMaterialProvider"          , getPublicTool("MuonMaterialProviderTool"))
    kwargs.setdefault("TrackQuery"                    , getPrivateTool("MuonTrackQuery") )

    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("MuonHoleRecovery"              , "" )
        trigTrackSummary = getPublicToolClone("TrigMuonTrackSummary", "MuonTrackSummaryTool")
        if DetFlags.detdescr.ID_on():
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool
            trigTrackSummary.InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool
            trigTrackSummary.doHolesInDet = True
        kwargs.setdefault("TrackSummaryTool"              , trigTrackSummary )

        kwargs.setdefault("Propagator"                    , ToolSvc.AtlasRungeKuttaPropagator)
        kwargs.setdefault("SLPropagator"                  , ToolSvc.AtlasRungeKuttaPropagator)

        #The trigger uses the iPatFitter in different sequences that do and do not include ID tracking
        #so using the same track summary tool as is used here to get it correct
        trackSummary = kwargs["TrackSummaryTool"]
        suffix = "MS"
        if "Trig" in trackSummary.name():
            suffix = "CB"
        kwargs.setdefault("Fitter"                        , getPublicToolClone("TrigiPatFitter_"+suffix, "iPatFitter", TrackSummaryTool=trackSummary) )
        kwargs.setdefault("SLFitter"                      , getPublicToolClone("TrigiPatSLFitter_"+suffix, "iPatSLFitter", TrackSummaryTool=trackSummary) )
        kwargs.setdefault("CscRotCreator"                 , "" )
        kwargs.setdefault("Cleaner"                       , getPrivateToolClone("TrigMuidTrackCleaner_"+suffix,"MuidTrackCleaner", Fitter=kwargs["Fitter"]) )

    else:
        import MuonCombinedRecExample.CombinedMuonTrackSummary
        kwargs.setdefault("TrackSummaryTool"              , ToolSvc.CombinedMuonTrackSummary )
        kwargs.setdefault("Propagator"                    , getPublicTool("MuonCombinedPropagator") )
        kwargs.setdefault("SLPropagator"                  , getPublicTool("MuonCombinedPropagator") )
        kwargs.setdefault("Fitter"                        , getPublicTool("iPatFitter") )
        kwargs.setdefault("SLFitter"                      , getPublicTool("iPatSLFitter") )
        kwargs.setdefault("CscRotCreator"                 , (getPublicTool("CscClusterOnTrackCreator") if MuonGeometryFlags.hasCSC() else "") )
        kwargs.setdefault("Cleaner"                       , getPrivateTool("MuidTrackCleaner") )


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
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("CaloEnergyParam"               , getPublicTool("MuidCaloEnergyToolParam") )
    kwargs.setdefault("CaloTSOS"                      , getPublicTool("MuidCaloTrackStateOnSurface") )
    kwargs.setdefault("MaterialAllocator"             , getPublicTool("MuidMaterialAllocator") )
    kwargs.setdefault("MdtRotCreator"                 , getPublicTool("MdtDriftCircleOnTrackCreator") )
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
    kwargs.setdefault("UseCaloTG"                     , True ) #
    kwargs.setdefault("CaloMaterialProvider"          , getPublicTool("MuonMaterialProviderTool"))
    kwargs.setdefault("TrackQuery"                    , getPrivateTool("MuonTrackQuery") )

    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("MuonHoleRecovery"              , "" )
        trigTrackSummary = getPublicToolClone("TrigMuonTrackSummary", "MuonTrackSummaryTool")
        if DetFlags.detdescr.ID_on():
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool
            trigTrackSummary.InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool
            trigTrackSummary.doHolesInDet = True
        kwargs.setdefault("TrackSummaryTool"              , trigTrackSummary )

        kwargs.setdefault("Propagator"                    , ToolSvc.AtlasRungeKuttaPropagator)
        kwargs.setdefault("SLPropagator"                  , ToolSvc.AtlasRungeKuttaPropagator)

        #The trigger uses the iPatFitter in different sequences that do and do not include ID tracking
        #so using the same track summary tool as is used here to get it correct
        trackSummary = kwargs["TrackSummaryTool"]
        suffix = "MS"
        if "Trig" in trackSummary.name():
            suffix = "CB"
        kwargs.setdefault("Fitter"                        , getPublicToolClone("TrigiPatFitter_"+suffix, "iPatFitter", TrackSummaryTool=trackSummary) )
        kwargs.setdefault("SLFitter"                      , getPublicToolClone("TrigiPatSLFitter_"+suffix, "iPatSLFitter", TrackSummaryTool=trackSummary) )
        kwargs.setdefault("MuonErrorOptimizer", "")
        kwargs.setdefault("CscRotCreator"                 , "" )
        kwargs.setdefault("Cleaner"                       , getPrivateToolClone("TrigMuidTrackCleaner_"+suffix, "MuidTrackCleaner", Fitter=kwargs["Fitter"]) )
    else:
        import MuonCombinedRecExample.CombinedMuonTrackSummary
        kwargs.setdefault("MuonHoleRecovery"              , getPublicTool("MuidSegmentRegionRecoveryTool") )
        kwargs.setdefault("TrackSummaryTool"              , ToolSvc.CombinedMuonTrackSummary )
        kwargs.setdefault("Propagator"                    , getPublicTool("MuonCombinedPropagator") )
        kwargs.setdefault("SLPropagator"                  , getPublicTool("MuonCombinedPropagator") )
        kwargs.setdefault("Fitter"                        , getPublicTool("iPatFitter") )
        kwargs.setdefault("SLFitter"                      , getPublicTool("iPatSLFitter") )
        kwargs.setdefault("CscRotCreator"                 , (getPublicTool("CscClusterOnTrackCreator") if MuonGeometryFlags.hasCSC() else "") )
        kwargs.setdefault("Cleaner"                       , getPrivateTool("MuidTrackCleaner") )


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
       if conddb.dbdata == 'COMP200' or conddb.dbmc == 'COMP200' or 'HLT' in globalflags.ConditionsTag() or conddb.isOnline or TriggerFlags.MuonSlice.doTrigMuonConfig:
            useAlignErrs = False

       kwargs.setdefault("MuonErrorOptimizer", getPublicToolClone("MuidErrorOptimisationTool",
                                                                  "MuonErrorOptimisationTool",
                                                                  PrepareForFit              = False,
                                                                  RecreateStartingParameters = False,
                                                                  RefitTool = getPublicToolClone("MuidRefitTool", "MuonRefitTool", AlignmentErrors = useAlignErrs, Fitter = getPublicTool("iPatFitter"))
                                                                  ))


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

