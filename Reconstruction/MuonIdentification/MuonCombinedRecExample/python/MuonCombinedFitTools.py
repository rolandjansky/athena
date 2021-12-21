# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

###############################################################
#
#  MuonIdentification jobOptions file.
#  Control selection and tool configuration for:
#    MuidSA, MuidCB, MuidVX and MuGirl
#
#   In progress: conversion to 3rd chain only
#
#==============================================================


from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonRecExample.MuonStandaloneFlags import muonStandaloneFlags
from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPrivateTool, getPrivateToolClone, getPublicTool, getPublicToolClone, getService
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam
from AthenaCommon.DetFlags import DetFlags 
from AthenaCommon.SystemOfUnits import meter
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

from IOVDbSvc.CondDB import conddb
from AthenaCommon.GlobalFlags import globalflags

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from InDetRecExample import TrackingCommon

GeV = 1000
mm = 1

def MuidMaterialAllocator( name='MuidMaterialAllocator', **kwargs): 
    kwargs.setdefault("AggregateMaterial",True)
    kwargs.setdefault("AllowReordering",False)
    kwargs.setdefault("Extrapolator", getPublicTool('AtlasExtrapolator') )
    kwargs.setdefault("TrackingGeometrySvc", getService("AtlasTrackingGeometrySvc") )

    cond_alg = TrackingCommon.createAndAddCondAlg(TrackingCommon.getTrackingGeometryCondAlg, "AtlasTrackingGeometryCondAlg", name="AtlasTrackingGeometryCondAlg")
    kwargs.setdefault("TrackingGeometryReadKey",cond_alg.TrackingGeometryWriteKey)

    return CfgMgr.Trk__MaterialAllocator(name,**kwargs)

# and the fitter
def iPatFitter( name='iPatFitter', **kwargs): 
    kwargs.setdefault("AggregateMaterial",True)
    kwargs.setdefault("FullCombinedFit", True )
    kwargs.setdefault("MaterialAllocator",getPublicTool("MuidMaterialAllocator"))
    from InDetRecExample import TrackingCommon
    kwargs.setdefault("SolenoidalIntersector",TrackingCommon.getSolenoidalIntersector())
    if ConfigFlags.Muon.MuonTrigger:
        kwargs.setdefault("MaxIterations", 15)
    else:
        import MuonCombinedRecExample.CombinedMuonTrackSummary  # noqa: F401 (import side-effects)
        from AthenaCommon.AppMgr import ToolSvc
        kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary)

    return CfgMgr.Trk__iPatFitter(name,**kwargs)

def iPatSLFitter( name='iPatSLFitter', **kwargs): 
    kwargs.setdefault("AggregateMaterial",True)
    kwargs.setdefault("LineFit",True)
    kwargs.setdefault("FullCombinedFit", True )
    kwargs.setdefault("MaterialAllocator",getPublicTool("MuidMaterialAllocator"))
    kwargs.setdefault("LineMomentum", muonStandaloneFlags.straightLineFitMomentum() )
    if ConfigFlags.Muon.MuonTrigger:
        kwargs.setdefault("MaxIterations", 15)
    else:
        import MuonCombinedRecExample.CombinedMuonTrackSummary  # noqa: F401 (import side-effects)
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

    if ConfigFlags.Muon.MuonTrigger:
        kwargs.setdefault("Iterate", False)
        kwargs.setdefault("RecoverOutliers", False)

    kwargs.setdefault("Fitter"      , getPrivateTool("iPatFitter") )
    kwargs.setdefault("SLFitter"    , getPrivateTool("iPatSLFitter") )
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
    if DetFlags.haveRIO.Calo_on() and not ConfigFlags.Muon.MuonTrigger:
        kwargs.setdefault("EnergyLossMeasurement", True )
        kwargs.setdefault("MinFinalEnergy", 1.0*GeV )
        kwargs.setdefault("MinMuonPt", 10.0*GeV )
        kwargs.setdefault("MopParametrization", True )
    else:
        kwargs.setdefault("EnergyLossMeasurement", False )

    if DetFlags.haveRIO.ID_on() and not ConfigFlags.Muon.MuonTrigger:
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
    cond_alg = TrackingCommon.createAndAddCondAlg(TrackingCommon.getTrackingGeometryCondAlg, "AtlasTrackingGeometryCondAlg", name="AtlasTrackingGeometryCondAlg")
    kwargs.setdefault("TrackingGeometryReadKey",cond_alg.TrackingGeometryWriteKey)
    kwargs.setdefault("MdtRotCreator",   getPublicTool("MdtDriftCircleOnTrackCreator") )
    kwargs.setdefault("Fitter", getPublicTool("iPatFitter"))
    return CfgMgr.Rec__MuonTrackQuery(name,**kwargs)

def MuidSegmentRegionRecoveryTool( name ='MuidSegmentRegionRecoveryTool', **kwargs ):
    kwargs.setdefault("Builder",  getPublicTool("CombinedMuonTrackBuilderFit") )
    import MuonCombinedRecExample.CombinedMuonTrackSummary  # noqa: F401 (import side-effects)
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary)

    if(athenaCommonFlags.isOnline):
        kwargs.setdefault("MdtCondKey","")

    from RegionSelector.RegSelToolConfig import makeRegSelTool_MDT, makeRegSelTool_RPC, makeRegSelTool_TGC
    kwargs.setdefault("MDTRegionSelector", makeRegSelTool_MDT())
    kwargs.setdefault("RPCRegionSelector", makeRegSelTool_RPC())
    kwargs.setdefault("TGCRegionSelector", makeRegSelTool_TGC())
 
    reco_cscs = MuonGeometryFlags.hasCSC() and muonRecFlags.doCSCs()
    reco_stgcs = muonRecFlags.dosTGCs() and MuonGeometryFlags.hasSTGC()
    reco_mm =  muonRecFlags.doMicromegas() and MuonGeometryFlags.hasMM()  
    
    if reco_cscs:
        from RegionSelector.RegSelToolConfig import makeRegSelTool_CSC
        kwargs.setdefault("CSCRegionSelector", makeRegSelTool_CSC())
    else:
        kwargs.setdefault("CSCRegionSelector", "")
    if reco_stgcs:
        from RegionSelector.RegSelToolConfig import makeRegSelTool_sTGC
        kwargs.setdefault("STGCRegionSelector", makeRegSelTool_sTGC())
    else:
        kwargs.setdefault("STGCRegionSelector", "")
    if reco_mm:
        from RegionSelector.RegSelToolConfig import makeRegSelTool_MM
        kwargs.setdefault("MMRegionSelector", makeRegSelTool_MM())
    else:
        kwargs.setdefault("MMRegionSelector", "")

    kwargs.setdefault( "ChamberHoleRecoveryTool", getPublicTool("MuonChamberHoleRecoveryTool"))
    return CfgMgr.Muon__MuonSegmentRegionRecoveryTool(name,**kwargs)


####
def MuonSegmentRegionRecoveryTool_EMEO(name = "MuonSegmentRegionRecoveryTool_EMEO"):
    return MuidSegmentRegionRecoveryTool(name = name , 
                                        ChamberHoleRecoveryTool = getPublicTool("MuonChamberRecovery_EMEO"),
                                        Builder = getPublicTool("CombinedTrackBuilderFit_EMEO"),
                                        STGCRegionSelector = "",
                                        MMRegionSelector = "",
                                        RecoverMM = False,
                                        RecoverSTGC = False)


def MuonCaloEnergyTool(name = "MuonCaloEnergyTool", **kwargs):
    from TrackToCalo.TrackToCaloConf import Rec__MuonCaloEnergyTool
    kwargs.setdefault("ParticleCaloExtensionTool" , getPublicTool("MuonParticleCaloExtensionTool"))
    kwargs.setdefault("ParticleCaloCellAssociationTool", getPublicTool("MuonCaloCellAssociationTool"))
    kwargs.setdefault("TrackParticleCreator", getPublicTool("MuonCaloParticleCreator"))
    return Rec__MuonCaloEnergyTool(name, **kwargs)

def ParticleCaloCellAssociationTool(name="MuonCaloCellAssociationTool", **kwargs):
    from TrackToCalo.TrackToCaloConf import  Rec__ParticleCaloCellAssociationTool
    kwargs.setdefault("ParticleCaloExtensionTool", getPublicTool("MuonParticleCaloExtensionTool"))
    return Rec__ParticleCaloCellAssociationTool("MuonCaloCellAssociationTool", **kwargs)

def MuonMaterialProviderTool( name = "MuonTrkMaterialProviderTool"):
    materialProviderTool = TrackingCommon.getTrkMaterialProviderTool( name = name, 
                                                                    MuonCaloEnergyTool = getPublicTool("MuonCaloEnergyTool"))
    if ConfigFlags.Muon.MuonTrigger:
        materialProviderTool.UseCaloEnergyMeasurement = False
    return materialProviderTool

def MuonAlignmentUncertToolTheta(name ="MuonAlignmentUncertToolTheta", **kwargs):
    kwargs.setdefault("HistoName", "ThetaScattering")
    kwargs.setdefault("InFile", "MuonCombinedBaseTools/AlignmentUncertainties/201029_initial/ID_MS_Uncertainties.root")
    return CfgMgr.Muon__MuonAlignmentUncertTool(name,**kwargs)

def MuonAlignmentUncertToolPhi(name ="MuonAlignmentUncertToolPhi", **kwargs):
    kwargs.setdefault("HistoName", "PhiScattering")
    kwargs.setdefault("InFile", "MuonCombinedBaseTools/AlignmentUncertainties/201029_initial/ID_MS_Uncertainties.root")
    return CfgMgr.Muon__MuonAlignmentUncertTool(name,**kwargs)
        
def CombinedMuonTrackBuilderFit( name='CombinedMuonTrackBuilderFit', **kwargs ):
    # N.B. This is a duplication of CombinedMuonTrackBuilder but I tried to remove it and got into circular dependency hell
    # Leave to new configuration to fix. EJWM. 
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("CaloEnergyParam"               , getPublicTool("MuidCaloEnergyToolParam") )
    kwargs.setdefault("CaloTSOS"                      , getPublicTool("MuidCaloTrackStateOnSurface") )
    kwargs.setdefault("MaterialAllocator"             , getPublicTool("MuidMaterialAllocator") )
    kwargs.setdefault("MdtRotCreator"                 , getPublicTool("MdtDriftCircleOnTrackCreator") )
    kwargs.setdefault("AlignmentUncertToolPhi"        , getPublicTool("MuonAlignmentUncertToolPhi") )
    kwargs.setdefault("AlignmentUncertToolTheta"      , getPublicTool("MuonAlignmentUncertToolTheta") )
    kwargs.setdefault("CaloMaterialProvider"          , getPublicTool("MuonTrkMaterialProviderTool"))
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
    kwargs.setdefault("TrackQuery"                    , getPrivateTool("MuonTrackQuery") )


    reco_cscs = MuonGeometryFlags.hasCSC() and muonRecFlags.doCSCs()
    reco_mircomegas = muonRecFlags.doMicromegas() and MuonGeometryFlags.hasMM()

    if ConfigFlags.Muon.MuonTrigger:
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
        kwargs.setdefault("Cleaner"                       , getPrivateToolClone("TrigMuidTrackCleaner_"+suffix,"MuidTrackCleaner", Fitter=getPublicToolClone("TrigiPatFitterClean_"+suffix, "iPatFitter", TrackSummaryTool=trackSummary, MaxIterations=4)))

    else:
        import MuonCombinedRecExample.CombinedMuonTrackSummary  # noqa: F401 (import side-effects)
        kwargs.setdefault("TrackSummaryTool"              , ToolSvc.CombinedMuonTrackSummary )
        kwargs.setdefault("Propagator"                    , getPrivateTool("MuonCombinedPropagator") )
        kwargs.setdefault("SLPropagator"                  , getPrivateTool("MuonCombinedPropagator") )
        kwargs.setdefault("Fitter"                        , getPrivateTool("iPatFitter") )
        kwargs.setdefault("SLFitter"                      , getPrivateTool("iPatSLFitter") )
        kwargs.setdefault("CscRotCreator"                 , getPrivateTool("CscClusterOnTrackCreator") if reco_cscs else "")
        kwargs.setdefault("MMRotCreator"                  , getPrivateTool("MMClusterOnTrackCreator") if reco_mircomegas else "" )
        kwargs.setdefault("Cleaner"                       , getPrivateTool("MuidTrackCleaner") )
        kwargs.setdefault("MuonErrorOptimizer"            , getPrivateTool('MuidErrorOptimisationTool'))
        kwargs.setdefault("MuonHoleRecovery"              , getPrivateTool("MuonChamberHoleRecoveryTool") ) 


    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("MdtRotCreator" ,  "" )
        kwargs.setdefault("LowMomentum"   ,  1.5*GeV )
        kwargs.setdefault("ReallocateMaterial", False )
        kwargs.setdefault("Vertex2DSigmaRPhi" , 100.*mm )
        kwargs.setdefault("Vertex3DSigmaRPhi" , 100.*mm )
        kwargs.setdefault("Vertex3DSigmaZ"    ,  1.*meter )

    if muonRecFlags.doSegmentT0Fit():
        kwargs.setdefault("MdtRotCreator"                 , "" )

    cond_alg = TrackingCommon.createAndAddCondAlg(TrackingCommon.getTrackingGeometryCondAlg, "AtlasTrackingGeometryCondAlg", name="AtlasTrackingGeometryCondAlg")
    kwargs.setdefault('TrackingGeometryReadKey', cond_alg.TrackingGeometryWriteKey)
    
    return CfgMgr.Rec__CombinedMuonTrackBuilder(name,**kwargs)

def CombinedTrackBuilderFit_EMEO(name = "CombinedTrackBuilderFit_EMEO", **kwargs):
    return CombinedMuonTrackBuilderFit(name = name,
                                       MuonHoleRecovery = getPublicTool("MuonChamberRecovery_EMEO") if not ConfigFlags.Muon.MuonTrigger else "",
                                       MMRotCreator = "")
    

def CombinedMuonTrackBuilder( name='CombinedMuonTrackBuilder', **kwargs ):
    from AthenaCommon.AppMgr import ToolSvc
    reco_cscs = MuonGeometryFlags.hasCSC() and muonRecFlags.doCSCs()
    reco_mircomegas = muonRecFlags.doMicromegas() and MuonGeometryFlags.hasMM()

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
    kwargs.setdefault("CaloMaterialProvider"          , getPublicTool("MuonTrkMaterialProviderTool"))
    kwargs.setdefault("TrackQuery"                    , getPrivateTool("MuonTrackQuery") )

    if ConfigFlags.Muon.MuonTrigger:
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
        kwargs.setdefault("MMRotCreator"                  , "" )
        kwargs.setdefault("Cleaner"                       , getPrivateToolClone("TrigMuidTrackCleaner_"+suffix, "MuidTrackCleaner", Fitter=getPublicToolClone("TrigiPatFitterClean_"+suffix, "iPatFitter", TrackSummaryTool=trackSummary, MaxIterations=4)))
    else:
        import MuonCombinedRecExample.CombinedMuonTrackSummary  # noqa: F401 (import side-effects)
        kwargs.setdefault("MuonHoleRecovery"              , getPublicTool("MuidSegmentRegionRecoveryTool") )
        kwargs.setdefault("TrackSummaryTool"              , ToolSvc.CombinedMuonTrackSummary )
        kwargs.setdefault("Propagator"                    , getPublicTool("MuonCombinedPropagator") )
        kwargs.setdefault("SLPropagator"                  , getPublicTool("MuonCombinedPropagator") )
        kwargs.setdefault("Fitter"                        , getPublicTool("iPatFitter") )
        kwargs.setdefault("SLFitter"                      , getPublicTool("iPatSLFitter") )
        kwargs.setdefault("CscRotCreator"                 , getPublicTool("CscClusterOnTrackCreator") if reco_cscs else "")
        kwargs.setdefault("MMRotCreator"                  , getPublicTool("MMClusterOnTrackCreator") if reco_mircomegas else "" )
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
       kwargs.setdefault("MuonErrorOptimizer", getPublicTool('MuidErrorOptimisationTool'))

    if muonRecFlags.doSegmentT0Fit():
        kwargs.setdefault("MdtRotCreator"                 , "" )
  
    cond_alg = TrackingCommon.createAndAddCondAlg(TrackingCommon.getTrackingGeometryCondAlg, "AtlasTrackingGeometryCondAlg", name="AtlasTrackingGeometryCondAlg")
    kwargs.setdefault('TrackingGeometryReadKey', cond_alg.TrackingGeometryWriteKey)
       
    return CfgMgr.Rec__CombinedMuonTrackBuilder(name,**kwargs)

def CombinedMuonTrackBuilder_EMEO(name = "MuonCombinedTrackBuilder_EMEO"):
    return CombinedMuonTrackBuilder(name = name,
                                    MuonHoleRecovery = getPublicTool("MuonSegmentRegionRecoveryTool_EMEO"),
                                    MMRotCreator = "")


def MuidErrorOptimisationTool(name='MuidErrorOptimisationTool', **kwargs):
    useAlignErrs = True
    if conddb.dbdata == 'COMP200' or conddb.dbmc == 'COMP200' or 'HLT' in globalflags.ConditionsTag() or conddb.isOnline or ConfigFlags.Muon.MuonTrigger:
        useAlignErrs = False
    kwargs.setdefault( 'RefitTool', getPublicToolClone("MuidRefitTool", "MuonRefitTool", AlignmentErrors = useAlignErrs, Fitter = getPublicTool("iPatFitter") ) )
    kwargs.setdefault( 'PrepareForFit', False )
    kwargs.setdefault( 'RecreateStartingParameters', False )
    kwargs.setdefault("RefitTool", getPublicTool("MuonRefitTool"))
    return CfgMgr.Muon__MuonErrorOptimisationTool(name, **kwargs)

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
    cond_alg = TrackingCommon.createAndAddCondAlg(TrackingCommon.getTrackingGeometryCondAlg, "AtlasTrackingGeometryCondAlg", name="AtlasTrackingGeometryCondAlg")
    kwargs.setdefault("TrackingGeometryReadKey",cond_alg.TrackingGeometryWriteKey)
    return CfgMgr.MuonCombined__MuonTrackTagTestTool(name,**kwargs)

