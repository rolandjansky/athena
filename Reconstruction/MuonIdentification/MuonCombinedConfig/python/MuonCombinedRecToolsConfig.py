# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Defines the shared tools used in muon identification
# Based on :
# https://gitlab.cern.ch/atlas/athena/blob/release/22.0.8/Reconstruction/MuonIdentification/MuonCombinedRecExample/python/MuonCombinedTools.py
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
from MuonConfig.MuonRecToolsConfig import MuonEDMPrinterToolCfg
from MuonConfig.MuonTrackBuildingConfig import MuonSegmentRegionRecoveryToolCfg

from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasEnergyLossUpdatorCfg
# FIXME
GeV = 1000
mm = 1

def MuonTrackToVertexCfg(flags, name='MuonTrackToVertexTool', **kwargs):
    acc = ComponentAccumulator()
    if 'Extrapolator' not in kwargs:
        accExtrapolator = AtlasExtrapolatorCfg(flags, 'AtlasExtrapolator')
        atlasExtrapolator = accExtrapolator.popPrivateTools()
        acc.merge(accExtrapolator)
        kwargs.setdefault('Extrapolator', atlasExtrapolator)
    acc.setPrivateTools(CompFactory.Reco.TrackToVertex(name, **kwargs))
    return acc


def MuonCombinedInDetDetailedTrackSelectorToolCfg(flags, name="MuonCombinedInDetDetailedTrackSelectorTool", **kwargs):
    if flags.Beam.Type is BeamType.Collisions:
        kwargs.setdefault("pTMin", 2000)
        kwargs.setdefault("nHitBLayer", 0)
        kwargs.setdefault("nHitBLayerPlusPix", 0)
        kwargs.setdefault("nHitTrt", 0)
        kwargs.setdefault("useTrackQualityInfo", False)
        if flags.Muon.SAMuonTrigger:
            kwargs.setdefault("IPd0Max", 19999.0)
            kwargs.setdefault("IPz0Max", 19999.0)
            kwargs.setdefault("z0Max", 19999.0)
            kwargs.setdefault("useTrackSummaryInfo", False)
            kwargs.setdefault("nHitPix", 0)
            kwargs.setdefault("nHitSct", 0)
            kwargs.setdefault("nHitSi", 0)
        else:
            kwargs.setdefault("IPd0Max", 50.0)
            kwargs.setdefault("IPz0Max", 9999.0)
            kwargs.setdefault("z0Max", 9999.0)
            kwargs.setdefault("useTrackSummaryInfo", True)
            kwargs.setdefault("nHitPix", 1)
            kwargs.setdefault("nHitSct", 3)
            kwargs.setdefault("nHitSi", 4)
    else:
        kwargs.setdefault("pTMin", 500)
        kwargs.setdefault("IPd0Max", 19999.0)
        kwargs.setdefault("IPz0Max", 19999.0)
        kwargs.setdefault("z0Max", 19999.0)
        kwargs.setdefault("useTrackSummaryInfo", False)
        kwargs.setdefault("useTrackQualityInfo", False)

    result = AtlasExtrapolatorCfg(flags)
    extrapolator = result.getPrimary()
    kwargs.setdefault("Extrapolator", extrapolator)

    kwargs.setdefault("TrackSummaryTool", "")

    # Has two CondKeys
    # SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

    # FIXME - let's put this someplace central?
    result.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Beampos",
                 "/Indet/Beampos", className='AthenaAttributeList'))
    result.addCondAlgo(CompFactory.BeamSpotCondAlg("BeamSpotCondAlg"))

    # SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
    # FIXME - handle this ^

    tool = CompFactory.InDet.InDetDetailedTrackSelectorTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonCombinedInDetDetailedTrackSelectorTool_LRTCfg(flags, name='MuonCombinedInDetDetailedTrackSelectorTool_LRT', **kwargs):
    kwargs.setdefault("pTMin", 2000)
    kwargs.setdefault("IPd0Max", 1.e4)
    kwargs.setdefault("IPz0Max",  1.e4)
    kwargs.setdefault("z0Max",  1.e4)
    kwargs.setdefault("useTrackSummaryInfo", True)
    kwargs.setdefault("nHitBLayer", 0)
    kwargs.setdefault("nHitPix", 0)
    kwargs.setdefault("nHitBLayerPlusPix", 0)
    kwargs.setdefault("nHitSct", 4)
    kwargs.setdefault("nHitSi", 4)
    kwargs.setdefault("nHitTrt", 0)
    kwargs.setdefault("useTrackQualityInfo", False)
    return MuonCombinedInDetDetailedTrackSelectorToolCfg(flags, name, **kwargs)


def InDetCandidateToolCfg(flags, name="InDetCandidateTool", **kwargs):
    result = MuonCombinedInDetDetailedTrackSelectorToolCfg(flags)
    kwargs.setdefault("TrackSelector", result.getPrimary())
    tool = CompFactory.MuonCombined.InDetCandidateTool(name, **kwargs)
    result.addPublicTool(tool)
    result.setPrivateTools(tool)
    return result


def MuonInDetForwardCandidateToolCfg(flags,  name='MuonInDetForwardCandidateTool', **kwargs):
    result = MuonCombinedInDetDetailedTrackSelectorToolCfg(
        flags, "MuonCombinedInDetDetailedForwardTrackSelectorTool", nHitSct=0)

    acc = InDetCandidateToolCfg(flags, name="InDetForwardCandidateTool",
                                TrackSelector=result.getPrimary(),
                                FlagCandidatesAsSiAssociated=True)
    tool = acc.getPrimary()
    result.merge(acc)
    result.addPublicTool(tool)
    result.setPrivateTools(tool)
    return result  # FIXME - is this and the above, actually used?


def MuonCaloEnergyToolCfg(flags,  name="MuonCaloEnergyTool", **kwargs):
    from TrackToCalo.TrackToCaloConfig import ParticleCaloCellAssociationToolCfg, ParticleCaloExtensionToolCfg


    result = ParticleCaloExtensionToolCfg(flags, name='MuonParticleCaloExtensionTool')
    particle_calo_extension_tool = result.getPrimary()

    particle_calo_cell_association_tool = result.popToolsAndMerge( 
        ParticleCaloCellAssociationToolCfg(flags, name='MuonCaloCellAssociationTool', ParticleCaloExtensionTool=particle_calo_extension_tool) )

    from TrkConfig.TrkParticleCreatorConfig import MuonCaloParticleCreatorCfg
    track_particle_creator = result.popToolsAndMerge(
        MuonCaloParticleCreatorCfg(flags))

    muonCaloEnergyTool = CompFactory.Rec.MuonCaloEnergyTool(name, ParticleCaloExtensionTool=particle_calo_extension_tool,
                                                            ParticleCaloCellAssociationTool=particle_calo_cell_association_tool,
                                                            TrackParticleCreator=track_particle_creator)
    result.setPrivateTools(muonCaloEnergyTool)
    return result


def MuonMaterialProviderToolCfg(flags,  name="MuonTrkMaterialProviderTool", **kwargs):
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasMultipleScatteringUpdatorCfg

    result = AtlasExtrapolatorCfg(flags)
    atlas_extrapolator = result.popPrivateTools()
    kwargs.setdefault("Extrapolator", atlas_extrapolator)
    result.addPublicTool(atlas_extrapolator)
    kwargs.setdefault("MuonCaloEnergyTool", result.popToolsAndMerge(MuonCaloEnergyToolCfg(flags, name = "MuonCaloEnergyTool")))
    
    # MuonCaloEnergyTool is actually a private tool
    calo_meas_tool = result.popToolsAndMerge(MuidCaloEnergyMeasCfg(flags))
    kwargs.setdefault("CaloMeasTool", calo_meas_tool)
    result.addPublicTool(calo_meas_tool)

    calo_param_tool = MuidCaloEnergyParam(flags)
    kwargs.setdefault("CaloParamTool", calo_param_tool)
    result.addPublicTool(calo_param_tool)

    multiple_scattering_tool = result.popToolsAndMerge(AtlasMultipleScatteringUpdatorCfg(flags))
    kwargs.setdefault("MultipleScatteringTool", multiple_scattering_tool)
    result.addPublicTool(multiple_scattering_tool)

    useCaloEnergyMeas = True
    if flags.Muon.MuonTrigger:
        useCaloEnergyMeas = False
    kwargs.setdefault("UseCaloEnergyMeasurement", useCaloEnergyMeas)
    acc = TrackingGeometryCondAlgCfg(flags)
    kwargs.setdefault("TrackingGeometryReadKey", acc.getPrimary().TrackingGeometryWriteKey)
    result.merge(acc)

    energy_loss_updator =  result.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags) )
    kwargs.setdefault("EnergyLossUpdator", energy_loss_updator) #PublicToolHandle
    result.addPublicTool( energy_loss_updator )

    track_isolation_tool = result.popToolsAndMerge(MuidTrackIsolationCfg(flags) )
    kwargs.setdefault("TrackIsolationTool", track_isolation_tool )
    result.addPublicTool(track_isolation_tool)

    tool = CompFactory.Trk.TrkMaterialProviderTool(name=name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonSegmentHitSummaryToolCfg(flags, name="MuonSegmentHitSummaryTool", **kwargs):
    from MuonConfig.MuonGeometryConfig import MuonDetectorCondAlgCfg

    result = MuonEDMPrinterToolCfg(flags)
    kwargs.setdefault("Printer", result.getPrimary())
    result.merge(MuonDetectorCondAlgCfg(flags))
    kwargs.setdefault("DetectorManagerKey", "MuonDetectorManager")
    tool = CompFactory.Muon.MuonSegmentHitSummaryTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonSegmentConverterToolCfg(flags, name="MuonSegmentConverterTool", **kwargs):
    result = MuonSegmentHitSummaryToolCfg(flags)
    kwargs.setdefault("MuonSegmentHitSummaryTool", result.popPrivateTools())
    # HitTimingTool does not need specific configuration
    tool = CompFactory.Muon.MuonSegmentConverterTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonDressingToolCfg(flags, name="MuonDressingTool", **kwargs):
    from MuonConfig.MuonRecToolsConfig import MuonHitSummaryToolCfg
    result = MuonHitSummaryToolCfg(flags)
    kwargs.setdefault("MuonHitSummaryTool", result.popPrivateTools())
    # HitTimingTool does not need specific configuration
    tool = CompFactory.MuonCombined.MuonDressingTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonCreatorToolCfg(flags, name="MuonCreatorTool", **kwargs):
    from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg
    result = ComponentAccumulator()
    # Not explicitly setting up MuonIdHelperSvc, nor MuonEDMHelperSvc (configured in top level reco Cfg)
    muon_edm_printer = result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags))
    kwargs.setdefault("Printer", muon_edm_printer)

    kwargs.setdefault("MuonPrinter", CompFactory.Rec.MuonPrintingTool(
        MuonStationPrinter=muon_edm_printer))

    acc = ParticleCaloExtensionToolCfg(flags, StartFromPerigee=True)
    kwargs.setdefault("ParticleCaloExtensionTool", acc.popPrivateTools())
    result.merge(acc)

    from TrkConfig.TrkParticleCreatorConfig import MuonCombinedParticleCreatorCfg
    kwargs.setdefault("TrackParticleCreator", result.popToolsAndMerge(MuonCombinedParticleCreatorCfg(flags)))

    from MuonConfig.MuonRecToolsConfig import MuonAmbiProcessorCfg
    kwargs.setdefault("AmbiguityProcessor", result.popToolsAndMerge(MuonAmbiProcessorCfg(flags)))
  
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
    kwargs.setdefault("Propagator", result.popToolsAndMerge(
        RungeKuttaPropagatorCfg(flags)))
    kwargs.setdefault("MuonDressingTool", result.popToolsAndMerge(
        MuonDressingToolCfg(flags)))
    # Not explicitly setting up MomentumBalanceTool nor ScatteringAngleTool   
    # Not explicitly setting up MeanMDTdADCTool (but probably should FIXME)

    kwargs.setdefault("CaloMaterialProvider", result.popToolsAndMerge(
        MuonMaterialProviderToolCfg(flags)))

    kwargs.setdefault("TrackQuery",   result.popToolsAndMerge(
        MuonTrackQueryCfg(flags)))
    # runCommissioningChain
    if flags.Muon.SAMuonTrigger:
        from TrkConfig.TrkTrackSummaryToolConfig import MuonTrackSummaryToolCfg
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(
            MuonTrackSummaryToolCfg(flags)))
    else:
        from TrkConfig.TrkTrackSummaryToolConfig import MuonCombinedTrackSummaryToolCfg
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(
            MuonCombinedTrackSummaryToolCfg(flags)))

    # kwargs.setdefault("CopyFloatSummaryKeys",[])
    kwargs.setdefault("CopyUInt8SummaryKeys",[]),


    if flags.Muon.MuonTrigger:
        kwargs.setdefault("MuonSelectionTool", "")
        kwargs.setdefault("UseCaloCells", False)       
    else:
        from MuonSelectorTools.MuonSelectorToolsConfig import MuonSelectionToolCfg
        kwargs.setdefault("MuonSelectionTool", result.popToolsAndMerge(MuonSelectionToolCfg(flags)))
    # This tool needs MuonScatteringAngleSignificanceTool... which in turn needs TrackingVolumeSvc.
    # FIXME - probably this should be someplace central.
    trackingVolSvc = CompFactory.Trk.TrackingVolumesSvc(
        name="TrackingVolumesSvc")
    result.addService(trackingVolSvc)

    tool = CompFactory.MuonCombined.MuonCreatorTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def ExtrapolateMuonToIPToolCfg(flags, name="ExtrapolateMuonToIPTool", **kwargs):
    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", result.popPrivateTools())
    from TrkConfig.AtlasExtrapolatorConfig import MuonExtrapolatorCfg
    kwargs.setdefault("MuonExtrapolator", result.popToolsAndMerge(
        MuonExtrapolatorCfg(flags)))

    if flags.Muon.MuonTrigger:
        from TrkConfig.TrkTrackSummaryToolConfig import MuonTrackSummaryToolCfg
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(
            MuonTrackSummaryToolCfg(flags)))
    else:
        from TrkConfig.TrkTrackSummaryToolConfig import MuonCombinedTrackSummaryToolCfg
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(
            MuonCombinedTrackSummaryToolCfg(flags)))
    kwargs.setdefault("Printer", result.getPrimaryAndMerge(
        MuonEDMPrinterToolCfg(flags)))
    result.setPrivateTools(CompFactory.ExtrapolateMuonToIPTool(name, **kwargs))
    return result


def MuonCandidateToolCfg(flags, name="MuonCandidateTool", **kwargs):
    from MuonConfig.MuonRecToolsConfig import MuonAmbiProcessorCfg
    result = ComponentAccumulator()
    muon_edm_printer = result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags))
    kwargs.setdefault("Printer", muon_edm_printer)
    if "TrackBuilder" not in kwargs:
        kwargs.setdefault("TrackBuilder", result.popToolsAndMerge(
            CombinedMuonTrackBuilderCfg(flags, name="CombinedMuonTrackBuilder")))
    #   Why was this dependent on cosmics? will now always create this
    #   if flags.Beam.Type is BeamType.Cosmics:
    if flags.Muon.MuonTrigger and flags.Beam.Type is not BeamType.Cosmics:
        # trigger definitely only uses the ExtrapolateToIPtool in cosmics mode
        kwargs.setdefault("TrackExtrapolationTool", "")
    else:
        kwargs.setdefault("TrackExtrapolationTool", result.popToolsAndMerge(ExtrapolateMuonToIPToolCfg(flags)))
        kwargs.setdefault("SegmentContainer", "TrackMuonSegments")
    kwargs.setdefault("AmbiguityProcessor", result.popToolsAndMerge(
        MuonAmbiProcessorCfg(flags)))

    from TrkConfig.TrkTrackSummaryToolConfig import MuonTrackSummaryToolCfg
    kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonTrackSummaryToolCfg(flags)))

    # MuonIDHelperSvc already configured

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("ExtrapolationStrategy", 1)

    track_segment_association_tool = CompFactory.MuonCombined.TrackSegmentAssociationTool(
        MuonEDMPrinterTool=muon_edm_printer)
    kwargs.setdefault("TrackSegmentAssociationTool",
                      track_segment_association_tool)
    result.addPublicTool(track_segment_association_tool)

    tool = CompFactory.MuonCombined.MuonCandidateTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonCombinedDebuggerToolCfg(flags, name="MuonCombinedDebuggerTool", **kwargs):
    result = ComponentAccumulator()
    match_quality = CompFactory.Rec.MuonMatchQuality(
        TrackQuery=result.popToolsAndMerge(MuonTrackQueryCfg(flags)))
    kwargs.setdefault("MuonMatchQuality", match_quality)
    tool = CompFactory.MuonCombined.MuonCombinedDebuggerTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonCombinedToolCfg(flags, name="MuonCombinedTool", **kwargs):
    tools = []
    result = ComponentAccumulator()
    kwargs.setdefault("Printer", result.getPrimaryAndMerge(
        MuonEDMPrinterToolCfg(flags)))

    if flags.MuonCombined.doCombinedFit:
        tool = result.popToolsAndMerge(MuonCombinedFitTagToolCfg(flags)) 
        tools.append(tool)
    if flags.MuonCombined.doStatisticalCombination and flags.Beam.Type is not BeamType.Cosmics:
        tool = result.popToolsAndMerge(MuonCombinedStacoTagToolCfg(flags))
        tools.append(tool)
       
    kwargs.setdefault("MuonCombinedTagTools", tools)
    kwargs.setdefault("MuonCombinedDebuggerTool", result.popToolsAndMerge(
        MuonCombinedDebuggerToolCfg(flags)))

    acc = MuonAlignmentUncertToolThetaCfg(flags)
    result.merge(acc)
    kwargs.setdefault("AlignmentUncertTool", result.getPublicTool(
        'MuonAlignmentUncertToolTheta'))

    kwargs.setdefault("DeltaEtaPreSelection", 0.2)
    kwargs.setdefault("DeltaPhiPreSelection", 0.2)
    tool = CompFactory.MuonCombined.MuonCombinedTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonCombinedFitTagToolCfg(flags, name="MuonCombinedFitTagTool", **kwargs):
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("VertexContainer", "")

    result = ComponentAccumulator() 
    kwargs.setdefault("TrackBuilder",  result.popToolsAndMerge(CombinedMuonTrackBuilderCfg(flags)))

    kwargs.setdefault("Printer", result.getPrimaryAndMerge(
        MuonEDMPrinterToolCfg(flags)))

    kwargs.setdefault("TrackQuery",   result.popToolsAndMerge(MuonTrackQueryCfg(flags)))
    kwargs.setdefault("MatchQuality",  result.popToolsAndMerge(MuonMatchQualityCfg(flags)))
   
    from MuonConfig.MuonRecToolsConfig import MuonTrackScoringToolCfg
    kwargs.setdefault("TrackScoringTool", result.popToolsAndMerge(MuonTrackScoringToolCfg(flags)))
    
    tool = CompFactory.MuonCombined.MuonCombinedFitTagTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result

def EMEO_MuonCombinedFitTagToolCfg(flags, name = "MuonCombinedFitTagTool_EMEO", **kwargs):
    result = ComponentAccumulator()
    track_builder = result.popToolsAndMerge(EMEO_CombinedMuonTrackBuilderCfg(flags))
    fit_tag_tool = result.popToolsAndMerge(MuonCombinedFitTagToolCfg(flags, name = name, 
                                            TrackBuilder = track_builder,
                                            **kwargs))
    result.setPrivateTools(fit_tag_tool)
    return result

def EMEO_MuonCombinedToolCfg(flags, name="MuonCombinedTool_EMEO", **kwargs):
    tools = []
    result = ComponentAccumulator()
    kwargs.setdefault("Printer", result.getPrimaryAndMerge(
        MuonEDMPrinterToolCfg(flags)))

    if flags.MuonCombined.doCombinedFit:
        tool = result.popToolsAndMerge(EMEO_MuonCombinedFitTagToolCfg(flags)) 
        tools.append(tool)
    if flags.MuonCombined.doStatisticalCombination:
        tool = result.popToolsAndMerge(MuonCombinedStacoTagToolCfg(flags))
        tools.append(tool)
       
    kwargs.setdefault("MuonCombinedTagTools", tools)
    kwargs.setdefault("MuonCombinedDebuggerTool", result.popToolsAndMerge(
        MuonCombinedDebuggerToolCfg(flags)))

    acc = MuonAlignmentUncertToolThetaCfg(flags)
    result.merge(acc)
    kwargs.setdefault("AlignmentUncertTool", result.getPublicTool(
        'MuonAlignmentUncertToolTheta'))

    kwargs.setdefault("DeltaEtaPreSelection", 0.2)
    kwargs.setdefault("DeltaPhiPreSelection", 0.2)
    tool = CompFactory.MuonCombined.MuonCombinedTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result

def MuonCombinedStacoTagToolCfg(flags, name="MuonCombinedStacoTagTool", **kwargs):

    result = ComponentAccumulator()
    kwargs.setdefault("Printer", result.getPrimaryAndMerge(
        MuonEDMPrinterToolCfg(flags)))
    kwargs.setdefault("TagTool", result.popToolsAndMerge(
        CombinedMuonTagTestToolCfg(flags)))
    kwargs.setdefault("Extrapolator", result.popToolsAndMerge(
        AtlasExtrapolatorCfg(flags)))

    tool = CompFactory.MuonCombined.MuonCombinedStacoTagTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result

# From Combined fit tools.py


def MuidMaterialAllocatorCfg(flags, name='MuidMaterialAllocator', **kwargs):
    from TrkConfig.TrkExSTEP_PropagatorConfig import AtlasSTEP_PropagatorCfg
    kwargs.setdefault("AggregateMaterial", True)
    kwargs.setdefault("AllowReordering", False)

    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", result.popPrivateTools())
    # Intersector (a RungeKuttaIntersector) does not require explicit configuration
    kwargs.setdefault("STEP_Propagator", result.popToolsAndMerge(
            AtlasSTEP_PropagatorCfg(flags, name="MuonPropagator")))
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    result.merge(TrackingGeometryCondAlgCfg(flags))
    kwargs.setdefault("TrackingGeometryReadKey", "AtlasTrackingGeometry")

    tool = CompFactory.Trk.MaterialAllocator(name, **kwargs)
    result.setPrivateTools(tool)
    return result

# and the fitter


def iPatFitterCfg(flags, name='iPatFitter', **kwargs):
    from TrkConfig.SolenoidalIntersectorConfig import SolenoidalIntersectorCfg
    from TrkConfig.TrkExSTEP_PropagatorConfig import AtlasSTEP_PropagatorCfg

    kwargs.setdefault("AggregateMaterial", True)
    kwargs.setdefault("FullCombinedFit", True)
    result = MuidMaterialAllocatorCfg(flags)
    kwargs.setdefault("MaterialAllocator", result.popPrivateTools())
    # RungeKuttaIntersector needs a AtlasFieldCacheCondObj, but it's impossible to get here without that being configured already so let's be lazy
    # It does not otherwise require explicit configuration
    kwargs.setdefault('SolenoidalIntersector', result.popToolsAndMerge(SolenoidalIntersectorCfg(flags)))
    kwargs.setdefault('Propagator', result.popToolsAndMerge(AtlasSTEP_PropagatorCfg(flags)))
    # StraightLineIntersector does not need explicit configuration
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("MaxIterations", 15)
    if flags.Muon.SAMuonTrigger:
        from TrkConfig.TrkTrackSummaryToolConfig import MuonTrackSummaryToolCfg
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonTrackSummaryToolCfg(flags)))
    else:
        from TrkConfig.TrkTrackSummaryToolConfig import MuonCombinedTrackSummaryToolCfg
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonCombinedTrackSummaryToolCfg(flags)))

    tool = CompFactory.Trk.iPatFitter(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def iPatSLFitterCfg(flags, name='iPatSLFitter', **kwargs):
    kwargs.setdefault("LineFit", True)
    kwargs.setdefault("LineMomentum", flags.Muon.straightLineFitMomentum)
    return iPatFitterCfg(flags, name, **kwargs)

# track cleaner configured to use the same fitter


def MuidTrackCleanerCfg(flags, name='MuidTrackCleaner', **kwargs):
    from MuonConfig.MuonRecToolsConfig import MuonTrackCleanerCfg
    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("PullCut", 5.0)
        kwargs.setdefault("PullCutPhi", 10.0)
    else:
        kwargs.setdefault("PullCut", 4.0)
        kwargs.setdefault("PullCutPhi", 4.0)
    result = ComponentAccumulator()
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("Iterate", False)
        kwargs.setdefault("RecoverOutliers", False)
        kwargs.setdefault("Fitter", result.popToolsAndMerge(
            iPatFitterCfg(flags, 'iPatFitterClean', MaxIterations=4)))
    else:
        kwargs.setdefault(
            "Fitter", result.popToolsAndMerge(iPatFitterCfg(flags)))
        kwargs.setdefault(
            "SLFitter", result.popToolsAndMerge(iPatSLFitterCfg(flags)))
    kwargs.setdefault("MaxAvePullSumPerChamber", 3.5) # Set back to default, because overridden in MuonTrackCleaner.
    return MuonTrackCleanerCfg(flags, name, **kwargs)


def MuidCaloEnergyParam(flags, name='MuidCaloEnergyParam', **kwargs):
    kwargs.setdefault("Cosmics", flags.Beam.Type is BeamType.Cosmics)
    return CompFactory.Rec.MuidCaloEnergyParam(name, **kwargs)


def MuidCaloEnergyMeasCfg(flags, name='MuidCaloEnergyMeas', **kwargs):
    result = ComponentAccumulator()
    muidcaloenergyparam = MuidCaloEnergyParam(flags)
    kwargs.setdefault("CaloParamTool",  muidcaloenergyparam)
    # FIXME! Need to setup the folders for CaloNoiseKey (which is why this needs a CA)
    # Not sure how to do : if flags.haveRIO.Calo_on() but TBH, if the cells aren't there it will abort anyway
    kwargs.setdefault("CellContainerLocation", "AllCalo")
    kwargs.setdefault("NoiseThresInSigmas", 4.)
    tool = CompFactory.Rec.MuidCaloEnergyMeas(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuidCaloEnergyToolParamCfg(flags, name='MuidCaloEnergyToolParam', **kwargs):
    # Some duplication with MuidCaloEnergyToolCfg but probably safer like this, since
    # we don't want to set e.g. MinFinalEnergy here
    result = MuidCaloEnergyMeasCfg(flags)
    kwargs.setdefault("CaloMeasTool", result.popPrivateTools())
    kwargs.setdefault("EnergyLossMeasurement", False)

    kwargs.setdefault("CaloParamTool", MuidCaloEnergyParam(flags) )
    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("Cosmics", True )
    kwargs.setdefault("TrackIsolationTool", result.popToolsAndMerge(MuidTrackIsolationCfg(flags) ) )

    result.setPrivateTools(CompFactory.Rec.MuidCaloEnergyTool(name, **kwargs))
    return result


def MuidTrackIsolationCfg(flags, name='MuidTrackIsolation', **kwargs):
     from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
     kwargs.setdefault("InDetTracksLocation", "CombinedInDetTracks")
     # RungeKuttaIntersector requires the magnetic field conditions
     result = AtlasFieldCacheCondAlgCfg(flags)
     tool = CompFactory.Rec.MuidTrackIsolation(name, **kwargs)
     result.setPrivateTools(tool)
     return result


def MuidCaloEnergyToolCfg(flags, name='MuidCaloEnergyTool', **kwargs):
    result = MuidCaloEnergyMeasCfg(flags)
    kwargs.setdefault("CaloMeasTool", result.popPrivateTools())
    kwargs.setdefault("CaloParamTool", MuidCaloEnergyParam(flags) )
    kwargs.setdefault("MinFinalEnergy", 1.0*GeV)
    kwargs.setdefault("MinMuonPt", 10.0*GeV)
    kwargs.setdefault("MopParametrization", True)
    if flags.Muon.MuonTrigger:
        # both properties also previously false if DetFlags.haveRIO.Calo_on()
        kwargs.setdefault("EnergyLossMeasurement", False)
        kwargs.setdefault("TrackIsolation", False)
    else:
        kwargs.setdefault("EnergyLossMeasurement", True)
        kwargs.setdefault("TrackIsolation", True)

    kwargs.setdefault("TrackIsolationTool", result.popToolsAndMerge(MuidTrackIsolationCfg(flags) ) )
    kwargs.setdefault("Cosmics", flags.Beam.Type is BeamType.Cosmics)
    result.setPrivateTools(CompFactory.Rec.MuidCaloEnergyTool(name, **kwargs))
    return result


def MuidCaloTrackStateOnSurfaceCfg(flags, name='MuidCaloTrackStateOnSurface', **kwargs):
    result = ComponentAccumulator()
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
    kwargs.setdefault("Propagator", result.popToolsAndMerge(
        RungeKuttaPropagatorCfg(flags)))
    kwargs.setdefault("MinRemainingEnergy", 0.2*GeV)
    kwargs.setdefault("ParamPtCut", 3.0*GeV)
    kwargs.setdefault("CaloEnergyDeposit", result.popToolsAndMerge(MuidCaloEnergyToolCfg(flags)))
    kwargs.setdefault("CaloEnergyParam", result.popToolsAndMerge(MuidCaloEnergyToolParamCfg(flags)))
    # I don't think CaloMaterialParam i.e. MuidCaloMaterialParam needs explicit configuration
    # Ditto for IntersectorWrapper, since it just uses RKIntersector which doesn't
    tool = CompFactory.Rec.MuidCaloTrackStateOnSurface(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuidCaloTrackStateOnSurfaceParamCfg(flags, name='MuidCaloTrackStateOnSurfaceParam', **kwargs):
    result = ComponentAccumulator()
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
    kwargs.setdefault("Propagator", result.popToolsAndMerge(
        RungeKuttaPropagatorCfg(flags)))
    kwargs.setdefault("MinRemainingEnergy", 0.2*GeV)
    kwargs.setdefault("ParamPtCut", 3.0*GeV)
    kwargs.setdefault("CaloEnergyDeposit", MuidCaloEnergyParam(flags))
    kwargs.setdefault("CaloEnergyParam",   MuidCaloEnergyParam(flags))
    tool = CompFactory.Rec.MuidCaloTrackStateOnSurface(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuidMaterialEffectsOnTrackProviderCfg(flags, name='MuidMaterialEffectsOnTrackProvider', **kwargs):
    result = MuidCaloTrackStateOnSurfaceCfg(flags)
    kwargs.setdefault("TSOSTool",      result.popPrivateTools())
    acc = MuidCaloTrackStateOnSurfaceParamCfg(flags)
    kwargs.setdefault("TSOSToolParam", acc.popPrivateTools())
    result.merge(acc)
    kwargs.setdefault("Cosmics", flags.Beam.Type is BeamType.Cosmics)
    tool = CompFactory.Rec.MuidMaterialEffectsOnTrackProvider(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuidMaterialEffectsOnTrackProviderParamCfg(flags, name='MuidMaterialEffectsOnTrackProviderParam', **kwargs):
    result = MuidCaloTrackStateOnSurfaceParamCfg(flags)
    muidtsosparam = result.popPrivateTools()
    kwargs.setdefault("TSOSTool",      muidtsosparam)
    kwargs.setdefault("TSOSToolParam", muidtsosparam)
    kwargs.setdefault("Cosmics", flags.Beam.Type is BeamType.Cosmics)
    tool = CompFactory.Rec.MuidMaterialEffectsOnTrackProvider(name, **kwargs)
    result.setPrivateTools(tool)
    return result

def MuonTrackQueryCfg(flags, name="MuonTrackQuery", **kwargs ):
    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import MdtDriftCircleOnTrackCreatorCfg
    result = MdtDriftCircleOnTrackCreatorCfg(flags)
    kwargs.setdefault("MdtRotCreator",   result.popPrivateTools())
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    result.merge(TrackingGeometryCondAlgCfg(flags))
    kwargs.setdefault("TrackingGeometryReadKey", "AtlasTrackingGeometry")

    acc = iPatFitterCfg(flags)
    kwargs.setdefault("Fitter", acc.popPrivateTools())
    result.merge(acc)

    tool = CompFactory.Rec.MuonTrackQuery(name, **kwargs)
    result.setPrivateTools(tool)
    return result




def EMEO_MuonSegmentRegionRecoveryToolCfg(flags, name="MuonSegmentRegionRecoveryTool_EMEO"):
    result = ComponentAccumulator()
    from MuonConfig.MuonTrackBuildingConfig import EMEO_MuonChamberHoleRecoveryToolCfg
    chamber_recovery = result.popToolsAndMerge(
        EMEO_MuonChamberHoleRecoveryToolCfg(flags))
    trk_builder = result.popToolsAndMerge(
        EMEO_CombinedTrackBuilderFitCfg(flags))
    from TrkConfig.TrkTrackSummaryToolConfig import MuonCombinedTrackSummaryToolCfg
    muon_combined_track_summary = result.popToolsAndMerge(
        MuonCombinedTrackSummaryToolCfg(flags))
    tool = result.popToolsAndMerge(MuonSegmentRegionRecoveryToolCfg(flags,
                                                                    name=name,
                                                                    ChamberHoleRecoveryTool=chamber_recovery,
                                                                    Builder=trk_builder,
                                                                    TrackSummaryTool = muon_combined_track_summary,
                                                                    STGCRegionSelector="",
                                                                    MMRegionSelector="",
                                                                    RecoverMM=False,
                                                                    RecoverSTGC=False))
    result.setPrivateTools(tool)
    return result


def EMEO_CombinedMuonTrackBuilderCfg(flags, name="MuonCombinedTrackBuilder_EMEO"):
    result = ComponentAccumulator()
    recovery_tool = result.popToolsAndMerge(
        EMEO_MuonSegmentRegionRecoveryToolCfg(flags))
    acc = CombinedMuonTrackBuilderCfg(flags, name,
                                      MMRotCreator="",
                                      MuonHoleRecovery=recovery_tool)
    # Need to reset this to be the primary tool
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result


def MuidErrorOptimisationToolCfg(flags, name='MuidErrorOptimisationTool', **kwargs):
    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryHelperToolCfg, MuonRefitToolCfg
    result=ComponentAccumulator()
    kwargs.setdefault("TrackSummaryTool",  result.popToolsAndMerge(
        MuonTrackSummaryHelperToolCfg(flags)))
    useAlignErrs = True
    if flags.IOVDb.DatabaseInstance == 'COMP200' or \
                'HLT' in flags.IOVDb.GlobalTag or flags.Common.isOnline or flags.Muon.MuonTrigger:
        useAlignErrs = False
    kwargs.setdefault("RefitTool", result.popToolsAndMerge(MuonRefitToolCfg(
            flags, name="MuidRefitTool", AlignmentErrors=useAlignErrs, 
            Fitter=result.popToolsAndMerge(iPatFitterCfg(flags)))))
    tool = CompFactory.Muon.MuonErrorOptimisationTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonAlignmentUncertToolThetaCfg(flags, name="MuonAlignmentUncertToolTheta", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("HistoName", "ThetaScattering")
    kwargs.setdefault(
        "InFile", "MuonCombinedBaseTools/AlignmentUncertainties/201029_initial/ID_MS_Uncertainties.root")
    tool = CompFactory.Muon.MuonAlignmentUncertTool(name, **kwargs)
    result.addPublicTool(tool)
    return result


def MuonAlignmentUncertToolPhiCfg(flags, name="MuonAlignmentUncertToolPhi", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("HistoName", "PhiScattering")
    kwargs.setdefault(
        "InFile", "MuonCombinedBaseTools/AlignmentUncertainties/201029_initial/ID_MS_Uncertainties.root")
    tool = CompFactory.Muon.MuonAlignmentUncertTool(name, **kwargs)
    result.addPublicTool(tool)
    return result


def CombinedMuonTrackBuilderCfg(flags, name='CombinedMuonTrackBuilder', **kwargs):
    from AthenaCommon.SystemOfUnits import meter
    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import CscClusterOnTrackCreatorCfg, MdtDriftCircleOnTrackCreatorCfg
    from TrkConfig.TrkTrackSummaryToolConfig import MuonCombinedTrackSummaryToolCfg

    result = ComponentAccumulator()
    kwargs.setdefault("CaloEnergyParam", result.popToolsAndMerge(MuidCaloEnergyToolParamCfg(flags)))
    kwargs.setdefault("CaloTSOS", result.popToolsAndMerge(MuidCaloTrackStateOnSurfaceCfg(flags)))
    kwargs.setdefault("Cleaner", result.popToolsAndMerge(MuidTrackCleanerCfg(flags)))
    acc = MuonAlignmentUncertToolPhiCfg(flags)
    result.merge(acc)
    kwargs.setdefault("AlignmentUncertToolPhi",
                      result.getPublicTool('MuonAlignmentUncertToolPhi'))
    acc = MuonAlignmentUncertToolThetaCfg(flags)
    result.merge(acc)
    kwargs.setdefault("AlignmentUncertToolTheta",
                      result.getPublicTool('MuonAlignmentUncertToolTheta'))

    if flags.Detector.GeometryCSC and not flags.Muon.MuonTrigger:
        acc = CscClusterOnTrackCreatorCfg(flags)
        kwargs.setdefault("CscRotCreator", acc.popPrivateTools())
        result.merge(acc)
    else:
        kwargs.setdefault("CscRotCreator", "")

    kwargs.setdefault("Extrapolator", result.popToolsAndMerge(
        AtlasExtrapolatorCfg(flags)))

    acc = iPatFitterCfg(flags)
    ipatFitter = acc.popPrivateTools()  # possibly used again below
    kwargs.setdefault("Fitter", ipatFitter)
    result.merge(acc)

    acc = iPatSLFitterCfg(flags)
    kwargs.setdefault("SLFitter", acc.popPrivateTools())
    result.merge(acc)

    acc = MuidMaterialAllocatorCfg(flags)
    kwargs.setdefault("MaterialAllocator", acc.popPrivateTools())
    result.merge(acc)

    acc = MdtDriftCircleOnTrackCreatorCfg(flags)
    kwargs.setdefault("MdtRotCreator", acc.popPrivateTools())
    result.merge(acc)

    # Tracking Geometry
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
        TrackingGeometryCondAlgCfg)
    acc = TrackingGeometryCondAlgCfg(flags)
    result.merge(TrackingGeometryCondAlgCfg(flags))
    geom_cond_key = acc.getPrimary().TrackingGeometryWriteKey
    kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)

    kwargs.setdefault("CleanCombined", True)
    kwargs.setdefault("CleanStandalone", True)
    kwargs.setdefault("BadFitChi2", 2.5)
    kwargs.setdefault("LargeMomentumError", 0.5)
    kwargs.setdefault("LineMomentum", flags.Muon.straightLineFitMomentum)
    kwargs.setdefault("LowMomentum", 10.*GeV)
    kwargs.setdefault("MinEnergy", 0.3*GeV)
    kwargs.setdefault("PerigeeAtSpectrometerEntrance", False)
    kwargs.setdefault("ReallocateMaterial", False)
    kwargs.setdefault("Vertex2DSigmaRPhi", 100.*mm)
    kwargs.setdefault("Vertex3DSigmaRPhi", 6.*mm)
    kwargs.setdefault("Vertex3DSigmaZ", 60.*mm)
    kwargs.setdefault("UseCaloTG", True)

    acc = MuonMaterialProviderToolCfg(flags)
    kwargs.setdefault("CaloMaterialProvider", acc.popPrivateTools())
    result.merge(acc)

    kwargs.setdefault("TrackQuery",   result.popToolsAndMerge(
        MuonTrackQueryCfg(flags)))

    if flags.Muon.SAMuonTrigger:
        from TrkConfig.TrkTrackSummaryToolConfig import MuonTrackSummaryToolCfg
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonTrackSummaryToolCfg(flags)))
    else:
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonCombinedTrackSummaryToolCfg(flags)))

    from TrkConfig.TrkExRungeKuttaPropagatorConfig import MuonCombinedPropagatorCfg
    acc = MuonCombinedPropagatorCfg(flags)
    propagator = acc.popPrivateTools()
    kwargs.setdefault("Propagator", propagator)
    kwargs.setdefault("SLPropagator", propagator)

    kwargs.setdefault("Printer", acc.getPrimaryAndMerge(
        MuonEDMPrinterToolCfg(flags)))

    result.merge(acc)

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("MdtRotCreator",  "")
        kwargs.setdefault("LowMomentum",  1.5*GeV)
        kwargs.setdefault("ReallocateMaterial", False)
        kwargs.setdefault("Vertex2DSigmaRPhi", 100.*mm)
        kwargs.setdefault("Vertex3DSigmaRPhi", 100.*mm)
        kwargs.setdefault("Vertex3DSigmaZ",  1.*meter)

    # configure tools for data reprocessing
    if flags.Muon.enableErrorTuning and 'MuonErrorOptimizer' not in kwargs:
        acc = MuidErrorOptimisationToolCfg(flags, name="MuidErrorOptimisationTool", PrepareForFit=False,
                                           RecreateStartingParameters=False)
        kwargs.setdefault("MuonErrorOptimizer", acc.popPrivateTools())
        result.merge(acc)
    else:
        kwargs.setdefault("MuonErrorOptimizer", "")

    if flags.Muon.MuonTrigger:
        kwargs.setdefault("MuonHoleRecovery", "")
    else:
        if "MuonHoleRecovery" not in kwargs:
            # Meeded to resolve circular dependency since MuonSegmentRegionRecoveryToolCfg calls CombinedMuonTrackBuilderCfg (i.e. this)!
            muon_combined_track_summary = result.popToolsAndMerge(MuonCombinedTrackSummaryToolCfg(flags))
            acc = MuonSegmentRegionRecoveryToolCfg(flags, name = "MuidSegmentRegionRecoveryTool", TrackSummaryTool = muon_combined_track_summary)
            kwargs.setdefault("MuonHoleRecovery", acc.popPrivateTools())
            result.merge(acc)

    if flags.Muon.doSegmentT0Fit:
        kwargs.setdefault("MdtRotCreator", "")
    tool = CompFactory.Rec.CombinedMuonTrackBuilder(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def CombinedMuonTrackBuilderFitCfg(flags, name='CombinedMuonTrackBuilderFit', **kwargs):
    # In the old configuration we had duplication between CombinedMuonTrackBuilder and CombinedMuonTrackBuilderFit
    # Here we just call the Combined
    from MuonConfig.MuonTrackBuildingConfig import MuonChamberHoleRecoveryToolCfg
    result = ComponentAccumulator()
    kwargs.setdefault("PerigeeAtSpectrometerEntrance", True)
    kwargs.setdefault("UseCaloTG", False)
    kwargs.setdefault("MuonErrorOptimizer",
                      result.popToolsAndMerge(MuidErrorOptimisationToolCfg(flags, PrepareForFit=False,
                                           RecreateStartingParameters=False)))
    kwargs.setdefault("MuonHoleRecovery", result.popToolsAndMerge(MuonChamberHoleRecoveryToolCfg(flags)) ) 

    tool = result.popToolsAndMerge(CombinedMuonTrackBuilderCfg(
        flags, name, **kwargs))  # Need to reset this to be the primary tool
    result.setPrivateTools(tool)
    return result


def EMEO_CombinedTrackBuilderFitCfg(flags, name="CombinedTrackBuilderFit_EMEO", **kwargs):
    result = ComponentAccumulator()
    from MuonConfig.MuonTrackBuildingConfig import EMEO_MuonChamberHoleRecoveryToolCfg
    if not flags.Muon.MuonTrigger:
        trk_builder = result.popToolsAndMerge(
            EMEO_MuonChamberHoleRecoveryToolCfg(flags))
        kwargs.setdefault("MuonHoleRecovery", trk_builder)
    else:
        kwargs.setdefault("MuonHoleRecovery", "")
    kwargs.setdefault("MMRotCreator", "")
    tool = result.popToolsAndMerge(CombinedMuonTrackBuilderFitCfg(
        flags, name, **kwargs))  # Need to reset this to be the primary tool
    result.setPrivateTools(tool)
    return result


def MuonMatchQualityCfg(flags, name='MuonMatchQuality', **kwargs):
    result = CombinedMuonTagTestToolCfg(flags)
    kwargs.setdefault("TagTool", result.popPrivateTools())
    kwargs.setdefault("TrackQuery", result.popToolsAndMerge(
        MuonTrackQueryCfg(flags)))

    tool = CompFactory.Rec.MuonMatchQuality(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuidMuonRecoveryCfg(flags, name='MuidMuonRecovery', **kwargs):
    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", result.popPrivateTools())
    acc = CombinedMuonTrackBuilderCfg(flags)
    kwargs.setdefault("TrackBuilder", acc.popPrivateTools())
    result.merge(acc)
    tool = CompFactory.Rec.MuidMuonRecovery(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def CombinedMuonTagTestToolCfg(flags, name='CombinedMuonTagTestTool', **kwargs):
    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("ExtrapolatorTool", result.popPrivateTools())
    kwargs.setdefault("Chi2Cut", 50000.)
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    result.merge(TrackingGeometryCondAlgCfg(flags))
    kwargs.setdefault("TrackingGeometryReadKey", "AtlasTrackingGeometry")
    tool = CompFactory.MuonCombined.MuonTrackTagTestTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result

# From MuonCaloTagTool.py


def CaloTrkMuIdAlgTrackSelectorToolCfg(flags, name='CaloTrkMuIdAlgTrackSelectorTool', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("pTMin", 5000.)
    kwargs.setdefault("IPd0Max", 7.)
    kwargs.setdefault("IPz0Max", 130)     # 130 (tuned on Z)
    kwargs.setdefault("nHitBLayer", 0)
    kwargs.setdefault("nHitPix", 1)
    kwargs.setdefault("nHitSct", 5)
    kwargs.setdefault("nHitSi", 7)
    kwargs.setdefault("nHitTrt", 0)
    from TrkConfig.TrkTrackSummaryToolConfig import MuonCombinedTrackSummaryToolCfg
    kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonCombinedTrackSummaryToolCfg(flags)))
    acc = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", acc.popPrivateTools())
    result.merge(acc)
    tool = CompFactory.InDet.InDetDetailedTrackSelectorTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def TrackDepositInCaloToolCfg(flags, name='TrackDepositInCaloTool', **kwargs):
    from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg, ParticleCaloCellAssociationToolCfg
    result = ParticleCaloExtensionToolCfg(flags)
    kwargs.setdefault("ParticleCaloExtensionTool",
                      result.popPrivateTools())
    acc = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("ExtrapolatorHandle", acc.popPrivateTools())
    result.merge(acc)
    acc = ParticleCaloCellAssociationToolCfg(flags)
    kwargs.setdefault("ParticleCaloCellAssociationTool",
                      acc.popPrivateTools())
    result.merge(acc)
    tool = CompFactory.TrackDepositInCaloTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def CaloMuonLikelihoodToolCfg(flags, name='CaloMuonLikelihoodTool', **kwargs):
    from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg
    result = ParticleCaloExtensionToolCfg(flags)
    kwargs.setdefault("ParticleCaloExtensionTool",
                      result.popPrivateTools())
    tool = CompFactory.CaloMuonLikelihoodTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def CaloMuonScoreToolCfg(flags, name='CaloMuonScoreTool', **kwargs):
    from TrackToCalo.TrackToCaloConfig import ParticleCaloCellAssociationToolCfg
    result = ParticleCaloCellAssociationToolCfg(flags)
    kwargs.setdefault("ParticleCaloCellAssociationTool",
                      result.popPrivateTools())
    tool = CompFactory.CaloMuonScoreTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonCaloTagToolCfg(flags, name='MuonCaloTagTool', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("CaloMuonTagLoose",       CompFactory.CaloMuonTag(
        name="CaloMuonTagLoose", TagMode="Loose"))
    kwargs.setdefault("CaloMuonTagTight",       CompFactory.CaloMuonTag())
    acc = CaloMuonLikelihoodToolCfg(flags)
    kwargs.setdefault("CaloMuonLikelihoodTool", acc.popPrivateTools())
    result.merge(acc)
    acc = CaloMuonScoreToolCfg(flags)
    kwargs.setdefault("CaloMuonScoreTool", acc.popPrivateTools())
    result.merge(acc)
    acc = TrackDepositInCaloToolCfg(flags)
    trackDepositInCaloTool = acc.popPrivateTools()
    kwargs.setdefault("TrackDepositInCaloTool", trackDepositInCaloTool)
    result.merge(acc)
    acc = CaloTrkMuIdAlgTrackSelectorToolCfg(flags)
    calotrkmuidalgtrackselectortool = acc.popPrivateTools()
    kwargs.setdefault("TrackSelectorTool",   calotrkmuidalgtrackselectortool)
    result.merge(acc)
    kwargs.setdefault("doCaloLR", False)
    tool = CompFactory.MuonCombined.MuonCaloTagTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result

# Misc


def MuonLayerSegmentFinderToolCfg(flags, name="MuonLayerSegmentFinderTool", **kwargs):
    from MuonConfig.MuonSegmentFindingConfig import DCMathSegmentMakerCfg, MuonClusterSegmentFinderToolCfg, MuonPRDSelectionToolCfg
    result = ComponentAccumulator()
    
    from MuonConfig.MuonSegmentFindingConfig import Csc2dSegmentMakerCfg, Csc4dSegmentMakerCfg
    kwargs.setdefault("Csc2DSegmentMaker", result.popToolsAndMerge(Csc2dSegmentMakerCfg(flags))  if flags.Detector.GeometryCSC  else "" )
    kwargs.setdefault("Csc4DSegmentMaker", result.popToolsAndMerge(Csc4dSegmentMakerCfg(flags))  if flags.Detector.GeometryCSC  else "" )

    kwargs.setdefault("MuonPRDSelectionTool", result.popToolsAndMerge(MuonPRDSelectionToolCfg(flags)))
    kwargs.setdefault("SegmentMaker",  result.popToolsAndMerge(DCMathSegmentMakerCfg(flags, name="DCMathSegmentMaker")))
    kwargs.setdefault("NSWMuonClusterSegmentFinderTool", 
                      result.popToolsAndMerge(MuonClusterSegmentFinderToolCfg(flags, name="MuonClusterSegmentFinderTool")))  
    tool = CompFactory.Muon.MuonLayerSegmentFinderTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonLayerSegmentMatchingToolCfg(flags, name="MuonLayerSegmentMatchingTool", **kwargs):
    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", result.popPrivateTools())
    MuTagTool = result.getPrimaryAndMerge(MuTagMatchingToolCfg(flags))
    kwargs.setdefault("MatchTool", MuTagTool)

    tool = CompFactory.Muon.MuonLayerSegmentMatchingTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonInsideOutRecoToolCfg(flags, name="MuonInsideOutRecoTool", **kwargs):
    from MuonConfig.MuonRecToolsConfig import MuonAmbiProcessorCfg
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("VertexContainer", "")

    result = MuonEDMPrinterToolCfg(flags)
    kwargs.setdefault("MuonEDMPrinterTool", result.popPrivateTools())

    layersegmentfindertool = result.popToolsAndMerge(
        MuonLayerSegmentFinderToolCfg(flags, name="MuonLayerSegmentFinderTool"))
    kwargs.setdefault("MuonLayerSegmentFinderTool", layersegmentfindertool)
    kwargs.setdefault("MuonLayerSegmentMatchingTool", result.popToolsAndMerge(
        MuonLayerSegmentMatchingToolCfg(flags)))
    kwargs.setdefault("MuonLayerAmbiguitySolverTool",  result.popToolsAndMerge(
        MuonLayerAmbiguitySolverToolCfg(flags)))
    kwargs.setdefault("MuonCandidateTrackBuilderTool", result.popToolsAndMerge(
        MuonCandidateTrackBuilderToolCfg(flags)))
    from TrkConfig.TrkTrackSummaryToolConfig import MuonCombinedTrackSummaryToolCfg
    kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(
        MuonCombinedTrackSummaryToolCfg(flags)))
    kwargs.setdefault("MuonTrackBuilder", result.popToolsAndMerge(
        CombinedMuonTrackBuilderCfg(flags)))
    kwargs.setdefault("TrackAmbiguityProcessor",
                      result.popToolsAndMerge(MuonAmbiProcessorCfg(flags)))

    tool = CompFactory.MuonCombined.MuonInsideOutRecoTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonCandidateTrackBuilderToolCfg(flags, name="MuonCandidateTrackBuilderTool", **kwargs):
    result = CombinedMuonTrackBuilderCfg(flags)
    kwargs.setdefault("MuonTrackBuilder", result.popPrivateTools())
    muoncandidatetrackbuilder = CompFactory.Muon.MuonCandidateTrackBuilderTool(
        name, **kwargs)
    result.setPrivateTools(muoncandidatetrackbuilder)
    return result


def MuonSegmentSelectionToolCfg(flags, name="MuonSegmentSelectionTool", **kwargs):
    if flags.Input.isMC is False:
        kwargs.setdefault("GoodADCFractionCut",  0.5)
        kwargs.setdefault("MinADCPerSegmentCut", 100)
    result = ComponentAccumulator()
    kwargs.setdefault("Printer", result.getPrimaryAndMerge(
        MuonEDMPrinterToolCfg(flags)))
    muon_segment_hit_summary_tool = result.popToolsAndMerge(
        MuonSegmentHitSummaryToolCfg(flags))
    kwargs.setdefault("MuonSegmentHitSummaryTool",
                      muon_segment_hit_summary_tool)
    result.addPublicTool(muon_segment_hit_summary_tool)

    result.setPrivateTools(
        CompFactory.Muon.MuonSegmentSelectionTool(name, **kwargs))
    return result


def MuonLayerAmbiguitySolverToolCfg(flags, name="MuonLayerAmbiguitySolverTool", **kwargs):
    from MuonConfig.MuonTrackBuildingConfig import MuonSegmentMatchingToolCfg, MooTrackBuilderCfg
    result = MuonSegmentSelectionToolCfg(flags)
    kwargs.setdefault("MuonSegmentSelectionTool", result.popPrivateTools())
    kwargs.setdefault("MuonSegmentMatchingTool", result.popToolsAndMerge(
        MuonSegmentMatchingToolCfg(flags, name='MuonSegmentMatchingToolTight', TightSegmentMatching=True)))

    kwargs.setdefault("MuonSegmentTrackBuilder",
                      result.popToolsAndMerge(MooTrackBuilderCfg(flags)))
    kwargs.setdefault("MuonEDMPrinterTool", result.getPrimaryAndMerge(
        MuonEDMPrinterToolCfg(flags)))

    result.setPrivateTools(
        CompFactory.Muon.MuonLayerAmbiguitySolverTool(name, **kwargs))
    return result

def MdtDriftCircleOnTrackCreatorStauCfg(flags, name="MdtDriftCircleOnTrackCreatorStau",**kwargs ):
    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import MdtDriftCircleOnTrackCreatorCfg, MdtCalibWindowNumber
    kwargs.setdefault("TimingMode", 3 )
    kwargs.setdefault("TimeWindowSetting", MdtCalibWindowNumber('Collision_t0fit') )
    return MdtDriftCircleOnTrackCreatorCfg(flags,name,**kwargs)

def MuonStauRecoToolCfg(flags,  name="MuonStauRecoTool", **kwargs):
    # In the old configuration this was split over several functions. But since these Stau tools are only used here,
    # trying a new approach. We can always refactorise later if necessary.

    from MuonConfig.MuonSegmentFindingConfig import DCMathSegmentMakerCfg, MuonPRDSelectionToolCfg
    from MuonConfig.MuonTrackBuildingConfig import MuonChamberHoleRecoveryToolCfg
    from MuonConfig.MuonRecToolsConfig import MuonAmbiProcessorCfg, MuonSeededSegmentFinderCfg
    from MuonConfig.MuonCalibrationConfig import MdtCalibrationDbToolCfg
    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import MdtDriftCircleOnTrackCreatorCfg
    from MuonConfig.MuonSegmentFindingConfig import MuonLayerHoughAlgCfg
    kwargs.setdefault("DoSummary", flags.Muon.printSummary)
    kwargs.setdefault("ConsideredPDGs", [13, -13, 1000015, -1000015])
    kwargs.setdefault("DoTruth", flags.Input.isMC)

    result = MuonEDMPrinterToolCfg(flags)
    result.merge(MuonLayerHoughAlgCfg(flags))
    # Not setting up MuonIdHelperSvc nor MuonEDMHelperSvc
    kwargs.setdefault("MuonEDMPrinterTool"  , result.getPrimary())
    kwargs.setdefault("MuonPRDSelectionTool", result.popToolsAndMerge(MuonPRDSelectionToolCfg(flags)))
    
    # This is going to be used in a few tools below
    rotcreator = result.popToolsAndMerge(MdtDriftCircleOnTrackCreatorStauCfg(flags))
    kwargs.setdefault("MuonPRDSelectionToolStau", 
                      result.popToolsAndMerge(MuonPRDSelectionToolCfg(flags,
                                                "MuonPRDSelectionToolStau",
                                                MdtDriftCircleOnTrackCreator = rotcreator)))

    segmentmaker = result.popToolsAndMerge(DCMathSegmentMakerCfg(
        flags, name="DCMathStauSegmentMaker", MdtCreator=rotcreator))
    # segmentmaker also used by MuonSeededSegmentFinder below
    kwargs.setdefault("MuonSegmentMaker", segmentmaker)
    kwargs.setdefault("MuonSegmentMakerT0Fit", result.popToolsAndMerge(DCMathSegmentMakerCfg(
        flags, name="DCMathStauSegmentMaker", MdtCreator=rotcreator, doSegmentT0Fit=True)))

    kwargs.setdefault("MuonLayerSegmentMatchingTool", result.popToolsAndMerge(MuonLayerSegmentMatchingToolCfg(flags)))

    # Not configuring MuonRecoValidationTool as it is off by default, but it would need configuring if used
    kwargs.setdefault("TrackAmbiguityProcessor",
                      result.popToolsAndMerge(MuonAmbiProcessorCfg(flags)))
    # I don't believe MuonHitTimingTool needs configuration.
    kwargs.setdefault("MuonPRDSelectionTool", result.popToolsAndMerge(
        MuonPRDSelectionToolCfg(flags)))
    kwargs.setdefault("MuonPRDSelectionToolStau", result.popToolsAndMerge(
        MuonPRDSelectionToolCfg(flags, MdtDriftCircleOnTrackCreator=rotcreator)))
    kwargs.setdefault("MdtDriftCircleOnTrackCreator",   result.popToolsAndMerge(MdtDriftCircleOnTrackCreatorCfg(flags)))
    # Now setup MuonInsideOutRecoTool property of MuonStauRecoTool. Long chain here! Could split for clarity. Another option would be to have a Stau flag on
    # shared tool functions.
    chamberholerecoverytool = result.popToolsAndMerge(
        MuonChamberHoleRecoveryToolCfg(flags))
    seededsegmentfinder = result.popToolsAndMerge(MuonSeededSegmentFinderCfg(flags, name="MuonStauSeededSegmentFinder", MdtRotCreator=rotcreator,
                                                                             SegmentMaker=segmentmaker, SegmentMakerNoHoles=segmentmaker))
    fitter = result.popToolsAndMerge(CombinedMuonTrackBuilderFitCfg(
        flags, name="CombinedStauTrackBuilderFit", MdtRotCreator=rotcreator))
    from TrkConfig.TrkTrackSummaryToolConfig import MuonCombinedTrackSummaryToolCfg
    muon_combined_track_summary = result.popToolsAndMerge(MuonCombinedTrackSummaryToolCfg(flags))
    muidsegmentregionrecovery = result.popToolsAndMerge(MuonSegmentRegionRecoveryToolCfg(flags, name="MuonStauSegmentRegionRecoveryTool", SeededSegmentFinder=seededsegmentfinder,
                                                                                         ChamberHoleRecoveryTool=chamberholerecoverytool, Fitter=fitter, TrackSummaryTool=muon_combined_track_summary))
    trackbuilder = result.popToolsAndMerge(CombinedMuonTrackBuilderCfg(
        flags, name="CombinedStauTrackBuilder", MdtRotCreator=rotcreator, MuonHoleRecovery=muidsegmentregionrecovery))
    muoncandidatetrackbuilder = CompFactory.Muon.MuonCandidateTrackBuilderTool(
        name="MuonStauCandidateTrackBuilderTool", MuonTrackBuilder=trackbuilder)
    kwargs.setdefault("MuonInsideOutRecoTool", result.popToolsAndMerge(
        MuonInsideOutRecoToolCfg(flags, MuonCandidateTrackBuilderTool=muoncandidatetrackbuilder)))
    # Rest

    kwargs.setdefault("MdtCalibrationDbTool", result.popToolsAndMerge(
        MdtCalibrationDbToolCfg(flags)))


    tool = CompFactory.MuonCombined.MuonStauRecoTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def MuonSystemExtensionToolCfg(flags, **kwargs):
    result = ComponentAccumulator()

    from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg
    particle_calo_extension_tool = result.popToolsAndMerge(
        ParticleCaloExtensionToolCfg(flags, name='MuonParticleCaloExtensionTool'))

    atlas_extrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags))

    muon_ext_tool = CompFactory.Muon.MuonSystemExtensionTool("MuonSystemExtensionTool",
                                                             ParticleCaloExtensionTool=particle_calo_extension_tool,
                                                             Extrapolator=atlas_extrapolator)
    result.setPrivateTools(muon_ext_tool)
    return result


def MuTagMatchingToolCfg(flags, name='MuTagMatchingTool', **kwargs):
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg

    #TODO: defaults in cxx
    kwargs.setdefault("AssumeLocalErrors", True)
    kwargs.setdefault("PhiCut", 30.)
    kwargs.setdefault("GlobalPhiCut", 1.)
    kwargs.setdefault("ThetaCut", 5.)
    kwargs.setdefault("GlobalThetaCut", 0.5)
    kwargs.setdefault("ThetaAngleCut", 5.)
    kwargs.setdefault("DoDistanceCut", True)
    kwargs.setdefault("CombinedPullCut", 3.0)

    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("IExtrapolator", result.popPrivateTools())
    kwargs.setdefault("Propagator", result.popToolsAndMerge(
        RungeKuttaPropagatorCfg(flags)))
    kwargs.setdefault("Printer", result.getPrimaryAndMerge(
        MuonEDMPrinterToolCfg(flags)))
    kwargs.setdefault("MuonSegmentHitSummary", result.popToolsAndMerge(
        MuonSegmentHitSummaryToolCfg(flags)))
    kwargs.setdefault("MuonSegmentSelection", result.popToolsAndMerge(
        MuonSegmentSelectionToolCfg(flags)))

    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
        TrackingGeometryCondAlgCfg)
    acc = TrackingGeometryCondAlgCfg(flags)
    geom_cond_key = acc.getPrimary().TrackingGeometryWriteKey
    result.merge(acc)
    kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)

    tool = CompFactory.MuTagMatchingTool(name, **kwargs)

    result.addPublicTool(tool, primary=True)
    return result


def MuTagAmbiguitySolverToolCfg(flags, name='MuTagAmbiguitySolverTool', **kwargs):
    from MuonConfig.MuonTrackBuildingConfig import MuonSegmentMatchingToolCfg
    #TODO: defaults in cxx
    kwargs.setdefault("RejectOuterEndcap", True)
    kwargs.setdefault("RejectMatchPhi", True)
    result = MuonEDMPrinterToolCfg(flags)
    kwargs.setdefault("Printer", result.popPrivateTools())
    kwargs.setdefault("MuonSegmentMatchingTool", result.popToolsAndMerge(
        MuonSegmentMatchingToolCfg(flags)))

    tool = CompFactory.MuTagAmbiguitySolverTool(name, **kwargs)
    result.addPublicTool(tool, primary=True)
    return result


def MuonSegmentTagToolCfg(flags, name="MuonSegmentTagTool", **kwargs):
    result = ComponentAccumulator()
    mu_tag_matching = result.getPrimaryAndMerge(MuTagMatchingToolCfg(flags))
    kwargs.setdefault("MuTagMatchingTool", mu_tag_matching)

    kwargs.setdefault("Printer", result.getPrimaryAndMerge(
        MuonEDMPrinterToolCfg(flags)))
    kwargs.setdefault("MuTagAmbiguitySolverTool", result.getPrimaryAndMerge(
        MuTagAmbiguitySolverToolCfg(flags)))
    kwargs.setdefault("MuonSegmentSelectionTool", result.getPrimaryAndMerge(
        MuonSegmentSelectionToolCfg(flags)))
    kwargs.setdefault("MuonSegmentHitSummaryTool", result.getPrimaryAndMerge(
        MuonSegmentHitSummaryToolCfg(flags)))

    result.setPrivateTools(
        CompFactory.MuonCombined.MuonSegmentTagTool(name, **kwargs))
    return result
