# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = "New configuration for the ISF_FatrasSimTool"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Algorithms.CollectionMergerConfig import CollectionMergerCfg
from ISF_Tools.ISF_ToolsConfigNew import ParticleHelperCfg
from ISF_Services.ISF_ServicesConfigNew import (
    AFIIParticleBrokerSvcCfg, TruthServiceCfg
)
from ISF_Geant4Tools.ISF_Geant4ToolsConfigNew import G4RunManagerHelperCfg
from RngComps.RandomServices import dSFMT

################################################################################
# HIT CREATION SECTION
################################################################################


#   Fatras Hadronic Interaction Processor
#   hadronic interaction creator
def fatrasHitCreatorPixelCfg(flags, name="ISF_FatrasHitCreatorPixel", **kwargs):
    """Return ISF_FatrasHitCreatorPixel configured with ComponentAccumulator"""
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    bare_collection_name = "PixelHits"
    mergeable_collection_suffix = "_Fatras"
    merger_input_property = "PixelHits"
    region = "ID"

    result, hits_collection_name = CollectionMergerCfg(flags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)

    seed = 'FatrasRnd OFFSET 123 81234740 23474923'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)
    kwargs.setdefault("IdHelperName", 'PixelID')
    kwargs.setdefault("CollectionName", hits_collection_name)
    kwargs.setdefault("UseConditionsTool", False)

    iFatras__HitCreatorSilicon = CompFactory.iFatras.HitCreatorSilicon
    result.setPrivateTools(iFatras__HitCreatorSilicon(name=name, **kwargs))
    return result


def fatrasHitCreatorSCTCfg(flags, name="ISF_FatrasHitCreatorSCT", **kwargs):
    """Return ISF_FatrasHitCreatorSCT configured with ComponentAccumulator"""
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    bare_collection_name = "SCT_Hits"
    mergeable_collection_suffix = "_Fatras"
    merger_input_property = "SCTHits"
    region = "ID"

    result, hits_collection_name = CollectionMergerCfg(flags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)

    seed = 'FatrasRnd OFFSET 123 81234740 23474923'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)
    kwargs.setdefault("IdHelperName", 'SCT_ID')
    kwargs.setdefault("CollectionName", hits_collection_name)
    kwargs.setdefault("UseConditionsTool", False)

    iFatras__HitCreatorSilicon = CompFactory.iFatras.HitCreatorSilicon
    result.setPrivateTools(iFatras__HitCreatorSilicon(name=name, **kwargs))
    return result


def fatrasHitCreatorTRTCfg(flags, name="ISF_FatrasHitCreatorTRT", **kwargs):
    """Return ISF_FatrasHitCreatorTRT configured with ComponentAccumulator"""
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    bare_collection_name = "TRTUncompressedHits"
    mergeable_collection_suffix = "_Fatras"
    merger_input_property = "TRTUncompressedHits"
    region = "ID"

    result, hits_collection_name = CollectionMergerCfg(flags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)

    seed = 'FatrasRnd OFFSET 123 81234740 23474923'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)
    kwargs.setdefault("CollectionName", hits_collection_name)

    iFatras__HitCreatorTRT = CompFactory.iFatras.HitCreatorTRT
    result.setPrivateTools(iFatras__HitCreatorTRT(name=name, **kwargs))
    return result


def fatrasPileupHitCreatorPixelCfg(flags, name="ISF_FatrasPileupHitCreatorPixel", **kwargs):
    """Return ISF_FatrasHitCreatorPixel configured for pileup with ComponentAccumulator"""
    kwargs.setdefault("CollectionName", "PileupPixelHits")
    return fatrasHitCreatorPixelCfg(flags, name, **kwargs)


def fatrasPileupHitCreatorSCTCfg(flags, name="ISF_FatrasPileupHitCreatorSCT", **kwargs):
    """Return ISF_FatrasHitCreatorSCT configured for pileup with ComponentAccumulator"""
    kwargs.setdefault("CollectionName", "PileupSCT_Hits")
    return fatrasHitCreatorSCTCfg(flags, name, **kwargs)


def fatrasPileupHitCreatorTRTCfg(flags, name="ISF_FatrasPileupHitCreatorTRT", **kwargs):
    """Return ISF_FatrasHitCreatorTRT configured with ComponentAccumulator"""
    kwargs.setdefault("CollectionName", "PileupTRTUncompressedHits")
    return fatrasHitCreatorTRTCfg(flags, name, **kwargs)


################################################################################
# TRACK CREATION SECTION
################################################################################
def fatrasSimHitCreatorIDCfg(flags, name="ISF_FatrasSimHitCreatorID", **kwargs):
    """Return ISF_FatrasSimHitCreatorID configured with ComponentAccumulator"""

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    pixel_hit_cfg = result.popToolsAndMerge(fatrasHitCreatorPixelCfg(flags))
    result.addPublicTool(pixel_hit_cfg)
    kwargs.setdefault("PixelHitCreator", result.getPublicTool(pixel_hit_cfg.name))

    sct_hit_cfg = result.popToolsAndMerge(fatrasHitCreatorSCTCfg(flags))
    result.addPublicTool(sct_hit_cfg)
    kwargs.setdefault("SctHitCreator", result.getPublicTool(sct_hit_cfg.name))

    trt_hit_cfg = result.popToolsAndMerge(fatrasHitCreatorTRTCfg(flags))
    result.addPublicTool(trt_hit_cfg)
    kwargs.setdefault("TrtHitCreator", result.getPublicTool(trt_hit_cfg.name))

    kwargs.setdefault("OutputLevel", flags.Exec.OutputLevel)

    iFatras__SimHitCreatorID = CompFactory.iFatras.SimHitCreatorID
    result.setPrivateTools(iFatras__SimHitCreatorID(name=name, **kwargs))
    return result


def fatrasPileupSimHitCreatorIDCfg(flags, name="ISF_FatrasPileupSimHitCreatorID", **kwargs):
    """Return ISF_FatrasSimHitCreatorID configured for pileup with ComponentAccumulator"""

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    pixel_hit_cfg = result.popToolsAndMerge(fatrasPileupHitCreatorPixelCfg(flags))
    result.addPublicTool(pixel_hit_cfg)
    kwargs.setdefault("PixelHitCreator", result.getPublicTool(pixel_hit_cfg.name))

    sct_hit_cfg = result.popToolsAndMerge(fatrasPileupHitCreatorSCTCfg(flags))
    result.addPublicTool(sct_hit_cfg)
    kwargs.setdefault("SctHitCreator", result.getPublicTool(sct_hit_cfg.name))

    trt_hit_cfg = result.popToolsAndMerge(fatrasPileupHitCreatorTRTCfg(flags))
    result.addPublicTool(trt_hit_cfg)
    kwargs.setdefault("TrtHitCreator", result.getPublicTool(trt_hit_cfg.name))

    return fatrasSimHitCreatorIDCfg(flags, name, **kwargs)


def fatrasSimHitCreatorMSCfg(flags, name="ISF_FatrasSimHitCreatorMS", **kwargs):
    """Return ISF_FatrasSimHitCreatorMS configured with ComponentAccumulator"""

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')


    result = ComponentAccumulator()
    mergeable_collection_suffix = "_Fatras"
    region = "MUON"

    mdt_bare_collection_name="MDT_Hits"
    mdt_merger_input_property="MDTHits"
    mdt_result, mdt_hits_collection_name = CollectionMergerCfg(flags,
                                                               mdt_bare_collection_name,
                                                               mergeable_collection_suffix,
                                                               mdt_merger_input_property,
                                                               region)
    result.merge(mdt_result)

    rpc_bare_collection_name="RPC_Hits"
    rpc_merger_input_property="RPCHits"
    rpc_result, rpc_hits_collection_name = CollectionMergerCfg(flags,
                                                               rpc_bare_collection_name,
                                                               mergeable_collection_suffix,
                                                               rpc_merger_input_property,
                                                               region)
    result.merge(rpc_result)

    tgc_bare_collection_name="TGC_Hits"
    tgc_merger_input_property="TGCHits"
    tgc_result, tgc_hits_collection_name = CollectionMergerCfg(flags,
                                                               tgc_bare_collection_name,
                                                               mergeable_collection_suffix,
                                                               tgc_merger_input_property,
                                                               region)
    result.merge(tgc_result)

    csc_bare_collection_name="CSC_Hits"
    csc_merger_input_property="CSCHits"
    csc_result, csc_hits_collection_name = CollectionMergerCfg(flags,
                                                               csc_bare_collection_name,
                                                               mergeable_collection_suffix,
                                                               csc_merger_input_property,
                                                               region)
    result.merge(csc_result)

    seed = 'FatrasRnd OFFSET 123 81234740 23474923'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)

    fatras_ext_cfg = result.popToolsAndMerge(fatrasExtrapolatorCfg(flags))
    result.addPublicTool(fatras_ext_cfg)
    kwargs.setdefault("Extrapolator" , result.getPublicTool('ISF_FatrasExtrapolator'))

    kwargs.setdefault("MDTCollectionName", mdt_hits_collection_name)
    kwargs.setdefault("RPCCollectionName", rpc_hits_collection_name)
    kwargs.setdefault("TGCCollectionName", tgc_hits_collection_name)
    kwargs.setdefault("CSCCollectionName", csc_hits_collection_name)

    Muon__MuonTGMeasurementTool = CompFactory.Muon.MuonTGMeasurementTool
    muon_tgmeasurement_tool = Muon__MuonTGMeasurementTool(name='MuonTGMeasurementTool',
                                                          UseDSManager=True)
    kwargs.setdefault("MeasurementTool", muon_tgmeasurement_tool)

    iFatras__SimHitCreatorMS = CompFactory.iFatras.SimHitCreatorMS
    result.setPrivateTools(iFatras__SimHitCreatorMS(name=name, **kwargs))
    return result


def fatrasPdgG4ParticleCfg(flags, name="ISF_FatrasPdgG4Particle", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    iFatras__PDGToG4Particle = CompFactory.iFatras.PDGToG4Particle
    result.setPrivateTools(iFatras__PDGToG4Particle(name=name, **kwargs))
    return result

######################################################################################
# validation & process sampling
######################################################################################


def fatrasPhysicsValidationToolCfg(flags, name="ISF_FatrasPhysicsValidationTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()
    kwargs.setdefault("ValidationStreamName", "ISFFatras")

    iFatras__PhysicsValidationTool = CompFactory.iFatras.PhysicsValidationTool
    result.setPrivateTools(iFatras__PhysicsValidationTool(name=name, **kwargs))
    return result


def fatrasParticleDecayHelperCfg(flags, name="ISF_FatrasParticleDecayHelper", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    seed = 'FatrasG4 OFFSET 0 23491234 23470291'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)
    kwargs.setdefault("G4RandomStreamName", flags.Sim.Fatras.G4RandomStreamName)
    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)

    if "ParticleBroker" not in kwargs:
        kwargs.setdefault("ParticleBroker", result.getPrimaryAndMerge(AFIIParticleBrokerSvcCfg(flags)).name)

    if "TruthRecordSvc" not in kwargs:
        kwargs.setdefault("ParticleTruthSvc", result.getPrimaryAndMerge(TruthServiceCfg(flags)).name)

    pdg_g4part_cfg = result.popToolsAndMerge(fatrasPdgG4ParticleCfg(flags))
    result.addPublicTool(pdg_g4part_cfg)
    kwargs.setdefault("PDGToG4ParticleConverter", result.getPublicTool(pdg_g4part_cfg.name))

    phys_val_cfg = result.popToolsAndMerge(fatrasPhysicsValidationToolCfg(flags))
    result.addPublicTool(phys_val_cfg)
    kwargs.setdefault("PhysicsValidationTool", result.getPublicTool(phys_val_cfg.name))

    tool = result.popToolsAndMerge(G4RunManagerHelperCfg(flags))
    result.addPublicTool(tool)
    kwargs.setdefault("G4RunManagerHelper", result.getPublicTool(tool.name))

    iFatras__G4ParticleDecayHelper = CompFactory.iFatras.G4ParticleDecayHelper
    result.setPrivateTools(iFatras__G4ParticleDecayHelper(name=name, **kwargs))
    return result


################################################################################
# Extrapolator
################################################################################
# the definition of an extrapolator (to be cleaned up)
def fatrasNavigatorCfg(flags, name="ISF_FatrasNavigator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()
    if not flags.Sim.ISF.UseTrackingGeometryCond:
        if 'TrackingGeometrySvc' not in kwargs:
            from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
            acc = TrackingGeometrySvcCfg(flags)
            kwargs.setdefault("TrackingGeometrySvc", acc.getPrimary())
            kwargs.setdefault("TrackingGeometryKey", '')
            result.merge(acc)
    else:
        if 'TrackingGeometryKey' not in kwargs:
            from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
            acc = TrackingGeometryCondAlgCfg(flags)
            geom_cond_key = acc.getPrimary().TrackingGeometryWriteKey
            result.merge(acc)
            kwargs.setdefault("TrackingGeometryKey", geom_cond_key)

    Trk__Navigator = CompFactory.Trk.Navigator
    navigator = Trk__Navigator(name=name, **kwargs)

    result.setPrivateTools(navigator)
    return result


def fatrasEnergyLossUpdatorCfg(flags, name="ISF_FatrasEnergyLossUpdator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    seed = 'FatrasRnd OFFSET 123 81234740 23474923'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)

    kwargs.setdefault("UsePDG_EnergyLossFormula", True)
    kwargs.setdefault("EnergyLossDistribution", 2)

    from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasEnergyLossUpdatorCfg
    kwargs.setdefault("EnergyLossUpdator", result.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags)))

    iFatras__McEnergyLossUpdator = CompFactory.iFatras.McEnergyLossUpdator
    result.setPrivateTools(iFatras__McEnergyLossUpdator(name=name, **kwargs))
    return result


def fatrasMultipleScatteringUpdatorCfg(flags, name="ISF_FatrasMultipleScatteringUpdator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    seed = 'FatrasRnd OFFSET 123 81234740 23474923'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.TrkExRandomStreamName)
    kwargs.setdefault("GaussianMixtureModel", flags.Sim.Fatras.GaussianMixtureModel)

    Trk__MultipleScatteringUpdator = CompFactory.Trk.MultipleScatteringUpdator
    result.setPrivateTools(Trk__MultipleScatteringUpdator(name=name, **kwargs))
    return result


# Combining all in the MaterialEffectsUpdator
def fatrasMaterialUpdatorCfg(flags, name="ISF_FatrasMaterialUpdator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    seed = 'FatrasRnd OFFSET 123 81234740 23474923'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)
    if "ParticleBroker" not in kwargs:
        kwargs.setdefault("ParticleBroker", result.getPrimaryAndMerge(AFIIParticleBrokerSvcCfg(flags)).name)

    if "TruthRecordSvc" not in kwargs:
        kwargs.setdefault("TruthRecordSvc", result.getPrimaryAndMerge(TruthServiceCfg(flags)).name)

    # @TODO retire once migration to TrackingGeometry conditions data is complete
    if flags.Sim.ISF.UseTrackingGeometryCond:
        if 'TrackingGeometryReadKey' not in kwargs:
            from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
            acc = TrackingGeometryCondAlgCfg(flags)
            geom_cond_key = acc.getPrimary().TrackingGeometryWriteKey
            result.merge(acc)
            kwargs.setdefault("TrackingGeometryKey", geom_cond_key)

    # hadronic interactions
    kwargs.setdefault("HadronicInteraction", True)

    g4had_proc_cfg = result.popToolsAndMerge(fatrasG4HadIntProcessorCfg(flags))
    result.addPublicTool(g4had_proc_cfg)
    kwargs.setdefault("HadronicInteractionProcessor", result.getPublicTool(g4had_proc_cfg.name))

    # energy loss
    kwargs.setdefault("EnergyLoss", True)
    eloss_updator = result.popToolsAndMerge(fatrasEnergyLossUpdatorCfg(flags))
    result.addPublicTool(eloss_updator)
    kwargs.setdefault("EnergyLossUpdator", result.getPublicTool(eloss_updator.name))

    # mutiple scattering
    kwargs.setdefault("MultipleScattering", True)
    multi_scattering_updator = result.popToolsAndMerge(fatrasMultipleScatteringUpdatorCfg(flags))
    result.addPublicTool(multi_scattering_updator)
    kwargs.setdefault("MultipleScatteringUpdator", result.getPublicTool(multi_scattering_updator.name))

    # photon conversion
    ph_conv_cfg = result.popToolsAndMerge(fatrasConversionCreatorCfg(flags))
    result.addPublicTool(ph_conv_cfg)
    kwargs.setdefault("PhotonConversionTool", result.getPublicTool(ph_conv_cfg.name))

    # the validation output
    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)
    kwargs.setdefault("BremPhotonValidation", False)
    kwargs.setdefault("EnergyDepositValidation", False)

    kwargs.setdefault("MomentumCut", flags.Sim.Fatras.MomCutOffSec)
    kwargs.setdefault("MinimumBremPhotonMomentum", flags.Sim.Fatras.MomCutOffSec)

    phys_val_cfg = result.popToolsAndMerge(fatrasPhysicsValidationToolCfg(flags))
    result.addPublicTool(phys_val_cfg)
    kwargs.setdefault("PhysicsValidationTool", result.getPublicTool(phys_val_cfg.name))

    proc_samp_cfg = result.popToolsAndMerge(fatrasProcessSamplingToolCfg(flags))
    result.addPublicTool(proc_samp_cfg)
    kwargs.setdefault("ProcessSamplingTool", result.getPublicTool(proc_samp_cfg.name))

    pdhelper_cfg = result.popToolsAndMerge(fatrasParticleDecayHelperCfg(flags))
    result.addPublicTool(pdhelper_cfg)
    kwargs.setdefault("ParticleDecayHelper", result.getPublicTool(pdhelper_cfg.name))

    # MCTruth Process Code
    kwargs.setdefault("BremProcessCode", 3)  # TODO: to be taken from central definition

    iFatras__McMaterialEffectsUpdator = CompFactory.iFatras.McMaterialEffectsUpdator
    result.setPrivateTools(iFatras__McMaterialEffectsUpdator(name=name, **kwargs))
    return result


def fatrasChargedPropagatorCfg(flags, name="ISF_FatrasChargedPropagator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()
    # the charged particle propagator
    ChargedPropagator = CompFactory.Trk.RungeKuttaPropagator
    result.setPrivateTools(ChargedPropagator(name=name, **kwargs))
    return result


def fatrasSTEP_PropagatorCfg(flags, name="ISF_FatrasSTEP_Propagator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()
    kwargs.setdefault("MomentumCutOff", flags.Sim.Fatras.MomCutOffSec)
    kwargs.setdefault("SimulationMode", True)

    material_updator = result.popToolsAndMerge(fatrasMaterialUpdatorCfg(flags))
    result.addPublicTool(material_updator)
    kwargs.setdefault("SimMatEffUpdator", result.getPublicTool(material_updator.name))

    # the step propagaor
    StepPropagator = CompFactory.Trk.STEP_Propagator
    result.setPrivateTools(StepPropagator(name=name, **kwargs))
    return result


def fatrasExtrapolatorCfg(flags, name="ISF_FatrasExtrapolator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    # Charged Transport Tool
    # assign the tools
    navigator = result.popToolsAndMerge(fatrasNavigatorCfg(flags))
    result.addPublicTool(navigator)
    kwargs.setdefault("Navigator", result.getPublicTool(navigator.name))

    material_updator = result.popToolsAndMerge(fatrasMaterialUpdatorCfg(flags))
    result.addPublicTool(material_updator)
    kwargs.setdefault("MaterialEffectsUpdators", [result.getPublicTool(material_updator.name)])

    charged_propagator = result.popToolsAndMerge(fatrasChargedPropagatorCfg(flags))
    result.addPublicTool(charged_propagator)
    kwargs.setdefault("Propagators", [result.getPublicTool(charged_propagator.name)])

    step_propagator = result.popToolsAndMerge(fatrasSTEP_PropagatorCfg(flags))
    result.addPublicTool(step_propagator)
    kwargs.setdefault("STEP_Propagator", result.getPublicTool(step_propagator.name))

    # Fatras specific: stop the trajectory
    kwargs.setdefault("StopWithNavigationBreak", True)
    kwargs.setdefault("StopWithUpdateKill", True)
    kwargs.setdefault("RobustSampling", True)
    kwargs.setdefault("ResolveMuonStation", True)
    kwargs.setdefault("UseMuonMatApproximation", True)

    TimedExtrapolator = CompFactory.Trk.TimedExtrapolator
    result.setPrivateTools(TimedExtrapolator(name=name, **kwargs))
    return result


################################################################################
# SIMULATION TOOL and SERVICE
################################################################################
def fatrasKinematicFilterCfg(flags, name="ISF_FatrasKinematicFilter", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    kwargs.setdefault("MaxEtaSymmetric", 10.)
    kwargs.setdefault("MinMomentum", flags.Sim.Fatras.MomCutOffSec)

    ISF__KinematicParticleFilter = CompFactory.ISF.KinematicParticleFilter
    result.setPrivateTools(ISF__KinematicParticleFilter(name=name, **kwargs))
    return result


def fatrasConversionCreatorCfg(flags, name="ISF_FatrasConversionCreator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    seed = 'FatrasRnd OFFSET 123 81234740 23474923'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)

    if "ParticleBroker" not in kwargs:
        kwargs.setdefault("ParticleBroker", result.getPrimaryAndMerge(AFIIParticleBrokerSvcCfg(flags)).name)

    if "TruthRecordSvc" not in kwargs:
        kwargs.setdefault("TruthRecordSvc", result.getPrimaryAndMerge(TruthServiceCfg(flags)).name)

    phys_val_cfg = result.popToolsAndMerge(fatrasPhysicsValidationToolCfg(flags))
    result.addPublicTool(phys_val_cfg)
    kwargs.setdefault("PhysicsValidationTool", result.getPublicTool(phys_val_cfg.name))

    kwargs.setdefault("PhysicsProcessCode", 14)  # TODO: to be taken from central definition
    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)

    iFatras__PhotonConversionTool = CompFactory.iFatras.PhotonConversionTool
    result.setPrivateTools(iFatras__PhotonConversionTool(name=name, **kwargs))
    return result


def fatrasG4HadIntProcessorCfg(flags, name="ISF_FatrasG4HadIntProcessor", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    seed = 'FatrasRnd OFFSET 123 81234740 23474923'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)

    if "ParticleBroker" not in kwargs:
        kwargs.setdefault("ParticleBroker", result.getPrimaryAndMerge(AFIIParticleBrokerSvcCfg(flags)).name)

    if "TruthRecordSvc" not in kwargs:
        kwargs.setdefault("TruthRecordSvc", result.getPrimaryAndMerge(TruthServiceCfg(flags)).name)

    phys_val_cfg = result.popToolsAndMerge(fatrasPhysicsValidationToolCfg(flags))
    result.addPublicTool(phys_val_cfg)
    kwargs.setdefault("PhysicsValidationTool", result.getPublicTool(phys_val_cfg.name))

    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)
    kwargs.setdefault("MomentumCut", flags.Sim.Fatras.MomCutOffSec)

    tool = result.popToolsAndMerge(G4RunManagerHelperCfg(flags))
    result.addPublicTool(tool)
    kwargs.setdefault("G4RunManagerHelper", result.getPublicTool(tool.name))

    iFatras__G4HadIntProcessor = CompFactory.iFatras.G4HadIntProcessor
    result.setPrivateTools(iFatras__G4HadIntProcessor(name=name, **kwargs))
    return result


#   Fatras Hadronic Interaction Processor
def fatrasParametricHadIntProcessorCfg(flags, name="ISF_FatrasParametricHadIntProcessor", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    seed = 'FatrasRnd OFFSET 123 81234740 23474923'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)

    if "ParticleBroker" not in kwargs:
        kwargs.setdefault("ParticleBroker", result.getPrimaryAndMerge(AFIIParticleBrokerSvcCfg(flags)).name)

    if "TruthRecordSvc" not in kwargs:
        kwargs.setdefault("TruthRecordSvc", result.getPrimaryAndMerge(TruthServiceCfg(flags)).name)

    kwargs.setdefault("HadronicInteractionScaleFactor", flags.Sim.Fatras.HadronIntProb)
    kwargs.setdefault("MinimumHadronicInitialEnergy", flags.Sim.Fatras.MomCutOffSec)
    kwargs.setdefault("MinimumHadronicOutEnergy", flags.Sim.Fatras.MomCutOffSec)
    kwargs.setdefault("HadronicInteractionValidation", False)
    kwargs.setdefault("PhysicsProcessCode", 121)  # TODO: to be taken from central definition

    phys_val_cfg = result.popToolsAndMerge(fatrasPhysicsValidationToolCfg(flags))
    result.addPublicTool(phys_val_cfg)
    kwargs.setdefault("PhysicsValidationTool", result.getPublicTool(phys_val_cfg.name))
    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)

    iFatras__HadIntProcessorParametric = CompFactory.iFatras.HadIntProcessorParametric
    result.setPrivateTools(iFatras__HadIntProcessorParametric(name, **kwargs))
    return result


def fatrasProcessSamplingToolCfg(flags, name="ISF_FatrasProcessSamplingTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    seed = 'FatrasRnd OFFSET 123 81234740 23474923'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))

    # truth record
    if "TruthRecordSvc" not in kwargs:
        kwargs.setdefault("TruthRecordSvc", result.getPrimaryAndMerge(TruthServiceCfg(flags)).name)

    # decays
    pd_helper_cfg = result.popToolsAndMerge(fatrasParticleDecayHelperCfg(flags))
    result.addPublicTool(pd_helper_cfg)
    kwargs.setdefault("ParticleDecayHelper", result.getPublicTool(pd_helper_cfg.name))

    # photon conversion
    ph_conv_cfg = result.popToolsAndMerge(fatrasConversionCreatorCfg(flags))
    result.addPublicTool(ph_conv_cfg)
    kwargs.setdefault("PhotonConversionTool", result.getPublicTool(ph_conv_cfg.name))

    # Hadronic interactions
    g4had_proc_cfg = result.popToolsAndMerge(fatrasG4HadIntProcessorCfg(flags))
    result.addPublicTool(g4had_proc_cfg)
    kwargs.setdefault("HadronicInteractionProcessor", result.getPublicTool(g4had_proc_cfg.name))
    kwargs.setdefault("HadronicInteraction", True)

    # Validation Tool
    phys_val_cfg = result.popToolsAndMerge(fatrasPhysicsValidationToolCfg(flags))
    result.addPublicTool(phys_val_cfg)
    kwargs.setdefault("PhysicsValidationTool", result.getPublicTool(phys_val_cfg.name))
    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)

    iFatras__ProcessSamplingTool = CompFactory.iFatras.ProcessSamplingTool
    result.setPrivateTools(iFatras__ProcessSamplingTool(name=name, **kwargs))
    return result


def fatrasTransportToolCfg(flags, name="ISF_FatrasSimTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    if "SimHitCreatorID" not in kwargs:
        id_cfg = result.popToolsAndMerge(fatrasSimHitCreatorIDCfg(flags))
        result.addPublicTool(id_cfg)
        kwargs.setdefault("SimHitCreatorID", result.getPublicTool(id_cfg.name))

    ms_cfg = result.popToolsAndMerge(fatrasSimHitCreatorMSCfg(flags))
    result.addPublicTool(ms_cfg)
    kwargs.setdefault("SimHitCreatorMS", result.getPublicTool(ms_cfg.name))

    pdhelper_cfg = result.popToolsAndMerge(fatrasParticleDecayHelperCfg(flags))
    result.addPublicTool(pdhelper_cfg)
    kwargs.setdefault("ParticleDecayHelper", result.getPublicTool(pdhelper_cfg.name))

    acc = ParticleHelperCfg(flags)
    part_helper_cfg = acc.getPrimary()
    kwargs.setdefault("ParticleHelper", part_helper_cfg)
    result.merge(acc)

    kin_filter_cfg = result.popToolsAndMerge(fatrasKinematicFilterCfg(flags))
    result.addPublicTool(kin_filter_cfg)
    kwargs.setdefault("TrackFilter", result.getPublicTool(kin_filter_cfg.name))
    kwargs.setdefault("NeutralFilter", result.getPublicTool(kin_filter_cfg.name))
    kwargs.setdefault("PhotonFilter", result.getPublicTool(kin_filter_cfg.name))

    extrapolator_cfg = result.popToolsAndMerge(fatrasExtrapolatorCfg(flags))
    result.addPublicTool(extrapolator_cfg)
    kwargs.setdefault("Extrapolator", result.getPublicTool(extrapolator_cfg.name))

    phys_val_cfg = result.popToolsAndMerge(fatrasPhysicsValidationToolCfg(flags))
    result.addPublicTool(phys_val_cfg)
    kwargs.setdefault("PhysicsValidationTool", result.getPublicTool(phys_val_cfg.name))

    proc_samp_cfg = result.popToolsAndMerge(fatrasProcessSamplingToolCfg(flags))
    result.addPublicTool(proc_samp_cfg)
    kwargs.setdefault("ProcessSamplingTool", result.getPublicTool(proc_samp_cfg.name))

    kwargs.setdefault("OutputLevel", flags.Exec.OutputLevel)
    kwargs.setdefault("ValidationOutput", flags.Sim.ISF.ValidationMode)

    seed = 'FatrasRnd OFFSET 123 81234740 23474923'
    result.merge(dSFMT(seed))
    kwargs.setdefault("RandomNumberService", result.getService("AtDSFMTGenSvc"))

    iFatras__TransportTool = CompFactory.iFatras.TransportTool
    result.setPrivateTools(iFatras__TransportTool(name=name, **kwargs))
    return result


def fatrasPileupSimToolCfg(flags, name="ISF_FatrasPileupSimTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    pusimhit_cfg = result.popToolsAndMerge(fatrasPileupSimHitCreatorIDCfg(flags))
    result.addPublicTool(pusimhit_cfg)
    kwargs.setdefault("SimHitCreatorID", result.getPublicTool(pusimhit_cfg.name))

    simtool_cfg = result.popToolsAndMerge(fatrasTransportToolCfg(flags, **kwargs))
    result.setPrivateTools(simtool_cfg)
    return result


# FatrasSimulatorTool
def fatrasSimulatorToolSTCfg(flags, name="ISF_FatrasSimulatorToolST", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    transportTool = result.popToolsAndMerge(fatrasTransportToolCfg(flags, **kwargs))
    result.addPublicTool(transportTool)
    kwargs.setdefault("IDSimulationTool", result.getPublicTool(transportTool.name))
    kwargs.setdefault("SimulationTool", result.getPublicTool(transportTool.name))

    ISF__FatrasSimTool = CompFactory.ISF.FatrasSimTool
    result.setPrivateTools(ISF__FatrasSimTool(name, **kwargs))
    return result


def fatrasPileupSimulatorToolSTCfg(flags, name="ISF_FatrasPileupSimulatorToolST", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    puTransportTool = result.popToolsAndMerge(fatrasPileupSimToolCfg(flags))
    result.addPublicTool(puTransportTool)
    kwargs.setdefault("IDSimulationTool", result.getPublicTool(puTransportTool.name))

    simtool_cfg = result.popToolsAndMerge(fatrasTransportToolCfg(flags))
    result.addPublicTool(simtool_cfg)
    kwargs.setdefault("SimulationTool", result.getPublicTool(simtool_cfg.name))

    simulator_tool_cfg = result.popToolsAndMerge(fatrasSimulatorToolSTCfg(flags))
    result.setPrivateTools(simulator_tool_cfg)
    return result


# FatrasSimulationSvc
def fatrasSimServiceIDCfg(flags, name="ISF_FatrasSimSvc", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    kwargs.setdefault("Identifier", "Fatras")

    simulator_tool_cfg = result.popToolsAndMerge(fatrasSimulatorToolSTCfg(flags))
    result.addPublicTool(simulator_tool_cfg)
    kwargs.setdefault("SimulatorTool", result.getPublicTool(simulator_tool_cfg.name))

    result.addService(CompFactory.ISF.LegacySimSvc(name, **kwargs))
    return result


def fatrasPileupSimServiceIDCfg(flags, name="ISF_FatrasPileupSimSvc", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    pu_sim_tool_cfg = result.popToolsAndMerge(fatrasPileupSimulatorToolSTCfg(flags))
    result.addPublicTool(pu_sim_tool_cfg)
    kwargs.setdefault("SimulatorTool", result.getPublicTool(pu_sim_tool_cfg.name))

    acc = fatrasSimServiceIDCfg(flags, name, **kwargs)
    result.merge(acc)
    return result


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    Configurable.configurableRun3Behavior = 1

    log.setLevel(DEBUG)

    ConfigFlags.Input.isMC = True
    ConfigFlags.Input.Files = defaultTestFiles.HITS
    ConfigFlags.Exec.MaxEvents = 3
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)
    acc.popToolsAndMerge(fatrasTransportToolCfg(ConfigFlags))

    print("INFO_FatrasConfig: Dumping config flags")
    ConfigFlags.dump()
    print("INFO_FatrasConfig: Print config details")
    acc.printConfig(withDetails=True, summariseProps=True)
    acc.store(open('fatrassimtool.pkl', 'wb'))

    sc = acc.run()

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
