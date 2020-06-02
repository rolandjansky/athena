# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = "New configuration for the ISF_FatrasSimTool"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name

################################################################################
# HIT CREATION SECTION
################################################################################

#   Fatras Hadronic Interaction Processor
#   hadronic interaction creator


def fatrasHitCreatorPixelCfg(flags, name="ISF_FatrasHitCreatorPixel", **kwargs):
    """Return ISF_FatrasHitCreatorPixel configured with ComponentAccumulator"""
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    hits_collection_name = generate_mergeable_collection_name(bare_collection_name="PixelHits",
                                                              mergeable_collection_suffix="_Fatras",
                                                              merger_input_property="PixelHits")

    kwargs.setdefault("IdHelperName", 'PixelID')
    kwargs.setdefault("CollectionName", hits_collection_name)

    # NOTE   why it is here ?
    # FastHitConvertTool = CompFactory.FastHitConvertTool
    kwargs.setdefault("UseConditionsTool", False)

    iFatras__HitCreatorSilicon = CompFactory.iFatras.HitCreatorSilicon
    acc.addPublicTool(iFatras__HitCreatorSilicon(name=name, **kwargs))
    return acc


def fatrasHitCreatorSCTCfg(flags, name="ISF_FatrasHitCreatorSCT", **kwargs):
    """Return ISF_FatrasHitCreatorSCT configured with ComponentAccumulator"""
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    hits_collection_name = generate_mergeable_collection_name(bare_collection_name="SCT_Hits",
                                                              mergeable_collection_suffix="_Fatras",
                                                              merger_input_property="SCTHits")
    kwargs.setdefault("IdHelperName", 'SCT_ID')
    kwargs.setdefault("CollectionName", hits_collection_name)
    kwargs.setdefault("UseConditionsTool", False)

    iFatras__HitCreatorSilicon = CompFactory.iFatras.HitCreatorSilicon
    acc.addPublicTool(iFatras__HitCreatorSilicon(name=name, **kwargs))
    return acc


def fatrasHitCreatorTRTCfg(flags, name="ISF_FatrasHitCreatorTRT", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    hits_collection_name = generate_mergeable_collection_name(bare_collection_name="TRTUncompressedHits",
                                                              mergeable_collection_suffix="_Fatras",
                                                              merger_input_property="TRTUncompressedHits")
    kwargs.setdefault("CollectionName", hits_collection_name)

    iFatras__HitCreatorTRT = CompFactory.iFatras.HitCreatorTRT
    acc.addPublicTool(iFatras__HitCreatorTRT(name=name, **kwargs))
    return acc


################################################################################
# TRACK CREATION SECTION
################################################################################

def fatrasSimHitCreatorIDCfg(flags, name="ISF_FatrasSimHitCreatorID", **kwargs):
    """Return ISF_FatrasSimHitCreatorID configured with ComponentAccumulator"""

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    acc = fatrasHitCreatorPixelCfg(flags)
    pixel_hit_cfg = acc.getPublicTool('ISF_FatrasHitCreatorPixel')
    result.merge(acc)
    kwargs.setdefault("PixelHitCreator", pixel_hit_cfg)

    acc = fatrasHitCreatorSCTCfg(flags)
    sct_hit_cfg = acc.getPublicTool('ISF_FatrasHitCreatorSCT')
    result.merge(acc)
    kwargs.setdefault("SctHitCreator", sct_hit_cfg)

    acc = fatrasHitCreatorTRTCfg(flags)
    trt_hit_cfg = acc.getPublicTool("ISF_FatrasHitCreatorTRT")
    result.merge(acc)
    kwargs.setdefault("TrtHitCreator", trt_hit_cfg)

    kwargs.setdefault("OutputLevel", 3)

    iFatras__SimHitCreatorID = CompFactory.iFatras.SimHitCreatorID
    result.addPublicTool(iFatras__SimHitCreatorID(name=name, **kwargs))
    return result


def fatrasSimHitCreatorMSCfg(flags, name="ISF_FatrasSimHitCreatorMS", **kwargs):
    """Return ISF_FatrasSimHitCreatorMS configured with ComponentAccumulator"""

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    mergeable_collection_suffix = "_Fatras"

    mdt_hits_collection_name = generate_mergeable_collection_name(bare_collection_name="MDT_Hits",
                                                                  mergeable_collection_suffix=mergeable_collection_suffix,
                                                                  merger_input_property="MDTHits")

    rpc_hits_collection_name = generate_mergeable_collection_name(bare_collection_name="RPC_Hits",
                                                                  mergeable_collection_suffix=mergeable_collection_suffix,
                                                                  merger_input_property="RPCHits")

    tgc_hits_collection_name = generate_mergeable_collection_name(bare_collection_name="TGC_Hits",
                                                                  mergeable_collection_suffix=mergeable_collection_suffix,
                                                                  merger_input_property="TGCHits")

    csc_hits_collection_name = generate_mergeable_collection_name(bare_collection_name="CSC_Hits",
                                                                  mergeable_collection_suffix=mergeable_collection_suffix,
                                                                  merger_input_property="CSCHits")

    #####
    # Extrapolator from ACTS to be added
    # kwargs.setdefault("Extrapolator" , getPublicTool('ISF_FatrasExtrapolator'))
    #####
    kwargs.setdefault("MDTCollectionName", mdt_hits_collection_name)
    kwargs.setdefault("RPCCollectionName", rpc_hits_collection_name)
    kwargs.setdefault("TGCCollectionName", tgc_hits_collection_name)
    kwargs.setdefault("CSCCollectionName", csc_hits_collection_name)

    Muon__MuonTGMeasurementTool = CompFactory.Muon.MuonTGMeasurementTool
    muon_tgmeasurement_tool = Muon__MuonTGMeasurementTool(name='MuonTGMeasurementTool',
                                                          UseDSManager=True)
    result.addPublicTool(muon_tgmeasurement_tool)
    kwargs.setdefault("MeasurementTool", muon_tgmeasurement_tool)

    iFatras__SimHitCreatorMS = CompFactory.iFatras.SimHitCreatorMS
    result.addPublicTool(iFatras__SimHitCreatorMS(name=name, **kwargs))
    return result


def fatrasPdgG4ParticleCfg(flags, name="ISF_FatrasPdgG4Particle", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    iFatras__PDGToG4Particle = CompFactory.iFatras.PDGToG4Particle
    result.addPublicTool(iFatras__PDGToG4Particle(name=name, **kwargs))
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
    result.addPublicTool(iFatras__PhysicsValidationTool(name=name, **kwargs))
    return result


def G4RunManagerHelperCfg(flags, name="ISF_G4RunManagerHelper", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    iGeant4__G4RunManagerHelper = CompFactory.iGeant4.G4RunManagerHelper
    result.addPublicTool(iGeant4__G4RunManagerHelper(name=name, **kwargs))

    return result


def fatrasParticleHelperCfg(flags, name="ISF_ParticleHelper", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    ISF__ParticleHelper = CompFactory.ISF.ParticleHelper
    result.addPublicTool(ISF__ParticleHelper(name=name, **kwargs))

    return result


def fatrasParticleDecayHelperCfg(flags, name="ISF_FatrasParticleDecayHelper", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    acc = fatrasPdgG4ParticleCfg(flags)
    pdg_g4part_cfg = acc.getPublicTool('ISF_FatrasPdgG4Particle')
    result.merge(acc)
    kwargs.setdefault("PDGToG4ParticleConverter", pdg_g4part_cfg)

    acc = fatrasPhysicsValidationToolCfg(flags)
    phys_val_cfg = acc.getPublicTool('ISF_FatrasPhysicsValidationTool')
    result.merge(acc)
    kwargs.setdefault("PhysicsValidationTool", phys_val_cfg)

    acc = G4RunManagerHelperCfg(flags)
    g4run_man_cfg = acc.getPublicTool('ISF_G4RunManagerHelper')
    result.merge(acc)
    kwargs.setdefault("PDGToG4ParticleConverter", g4run_man_cfg)

    iFatras__G4ParticleDecayHelper = CompFactory.iFatras.G4ParticleDecayHelper
    result.addPublicTool(iFatras__G4ParticleDecayHelper(name=name, **kwargs))

    return result

################################################################################
# Extrapolator
################################################################################
# the definition of an extrapolator (to be cleaned up)
def fatrasNavigatorCfg(flags, name="ISF_FatrasNavigator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()    

    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc = TrackingGeometrySvcCfg(flags)
    kwargs.setdefault("TrackingGeometrySvc", acc.getPrimary())
    result.merge(acc)

    Trk__Navigator = CompFactory.Trk.Navigator
    result.addPublicTool(Trk__Navigator(name=name, **kwargs))

    return result

def fatrasEnergyLossUpdatorCfg(flags, name="ISF_FatrasEnergyLossUpdator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    kwargs.setdefault("UsePDG_EnergyLossFormula", True)
    kwargs.setdefault("EnergyLossDistribution", 2)
    
    iFatras__McEnergyLossUpdator = CompFactory.iFatras.McEnergyLossUpdator
    result.addPublicTool(iFatras__McEnergyLossUpdator(name=name, **kwargs))

    return result

def fatrasMultipleScatteringUpdatorCfg(flags, name="ISF_FatrasMultipleScatteringUpdator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()
    kwargs.setdefault("GaussianMixtureModel", True)

    Trk__MultipleScatteringUpdator = CompFactory.Trk.MultipleScatteringUpdator
    result.addPublicTool(Trk__MultipleScatteringUpdator(name=name, **kwargs))

    return result

# Combining all in the MaterialEffectsUpdator
def fatrasMaterialUpdatorCfg(flags, name="ISF_FatrasMaterialUpdator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    # Geometry Svc
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc = TrackingGeometrySvcCfg(flags)
    kwargs.setdefault("TrackingGeometrySvc", acc.getPrimary())
    result.merge(acc)
    
    # hadronic interactions
    kwargs.setdefault("HadronicInteraction", True)
    acc = fatrasG4HadIntProcessorCfg(flags)
    g4had_proc_cfg = acc.getPublicTool('ISF_FatrasG4HadIntProcessor')
    kwargs.setdefault("HadronicInteractionProcessor", g4had_proc_cfg)
    result.merge(acc)

    # energy loss
    kwargs.setdefault("EnergyLoss", True)
    acc = fatrasEnergyLossUpdatorCfg(flags)
    eloss_updator = acc.getPublicTool('ISF_FatrasEnergyLossUpdator')
    kwargs.setdefault("EnergyLossUpdator", eloss_updator)
    result.merge(acc)

    # mutiple scattering
    kwargs.setdefault("MultipleScattering", True)
    acc = fatrasMultipleScatteringUpdatorCfg(flags)
    multi_scattering_updator = acc.getPublicTool('ISF_FatrasMultipleScatteringUpdator')
    kwargs.setdefault("MultipleScatteringUpdator", multi_scattering_updator)
    result.merge(acc)

    # photon conversion
    acc = fatrasConversionCreatorCfg(flags)
    ph_conv_cfg = acc.getPublicTool('ISF_FatrasConversionCreator')
    result.merge(acc)
    kwargs.setdefault("PhotonConversionTool", ph_conv_cfg)
  
    # the validation output
    ##  NOTE to be checked
    ###  kwargs.setdefault("ValidationMode", ISF_Flags.ValidationMode())

    acc = fatrasPhysicsValidationToolCfg(flags)
    phys_val_cfg = acc.getPublicTool('ISF_FatrasPhysicsValidationTool')
    result.merge(acc)
    kwargs.setdefault("PhysicsValidationTool", phys_val_cfg)

    acc = fatrasProcessSamplingToolCfg(flags)
    proc_samp_cfg = acc.getPublicTool('ISF_FatrasProcessSamplingTool')
    result.merge(acc)
    kwargs.setdefault("ProcessSamplingTool", proc_samp_cfg)

    acc = fatrasParticleDecayHelperCfg(flags)
    pdhelper_cfg = acc.getPublicTool('ISF_FatrasParticleDecayHelper')
    result.merge(acc)
    kwargs.setdefault("ParticleDecayHelper", pdhelper_cfg)


    iFatras__McMaterialEffectsUpdator = CompFactory.iFatras.McMaterialEffectsUpdator
    result.addPublicTool(iFatras__McMaterialEffectsUpdator(name=name, **kwargs))

    return result

def fatrasChargedPropagatorCfg(flags, name="ISF_FatrasChargedPropagator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()
    # the charged particle propagator
    ChargedPropagator = CompFactory.Trk.RungeKuttaPropagator
    result.addPublicTool(ChargedPropagator(name=name, **kwargs))

    return result

def fatrasSTEP_PropagatorCfg(flags, name="ISF_FatrasSTEP_Propagator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()
    kwargs.setdefault("SimulationMode", True)

    acc = fatrasMaterialUpdatorCfg(flags)
    material_updator = acc.getPublicTool('ISF_FatrasMaterialUpdator')
    result.merge(acc)
    kwargs.setdefault("SimMatEffUpdator", material_updator)

    # the step propagaor
    StepPropagator = CompFactory.Trk.STEP_Propagator
    result.addPublicTool(StepPropagator(name=name, **kwargs))

    return result


def fatrasExtrapolatorCfg(flags, name="ISF_FatrasExtrapolator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    # Charged Transport Tool
    # assign the tools
    acc = fatrasNavigatorCfg(flags)
    navigator = acc.getPublicTool('ISF_FatrasNavigator')
    result.merge(acc)
    kwargs.setdefault("Navigator", navigator)

    acc = fatrasMaterialUpdatorCfg(flags)
    material_updator = acc.getPublicTool('ISF_FatrasMaterialUpdator')
    result.merge(acc)
    kwargs.setdefault("MaterialEffectsUpdators", [material_updator])

    acc = fatrasChargedPropagatorCfg(flags)
    charged_propagator = acc.getPublicTool('ISF_FatrasChargedPropagator')
    result.merge(acc)
    kwargs.setdefault("Propagators", [charged_propagator])

    acc = fatrasSTEP_PropagatorCfg(flags)
    step_propagator = acc.getPublicTool('ISF_FatrasSTEP_Propagator')
    result.merge(acc)
    kwargs.setdefault("STEP_Propagator", step_propagator)

    # Fatras specific: stop the trajectory
    kwargs.setdefault("StopWithNavigationBreak", True)
    kwargs.setdefault("StopWithUpdateKill", True)
    kwargs.setdefault("ResolveMuonStation", True)
    kwargs.setdefault("UseMuonMatApproximation", True)

    TimedExtrapolator = CompFactory.Trk.TimedExtrapolator
    result.addPublicTool(TimedExtrapolator(name=name, **kwargs))

    return result

################################################################################
# SIMULATION TOOL and SERVICE
################################################################################

def fatrasKinematicFilterCfg(flags, name="ISF_FatrasKinematicFilter", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    kwargs.setdefault("MaxEtaSymmetric", 10.)

    ISF__KinematicParticleFilter = CompFactory.ISF.KinematicParticleFilter
    result.addPublicTool(ISF__KinematicParticleFilter(name=name, **kwargs))

    return result


def fatrasConversionCreatorCfg(flags, name="ISF_FatrasConversionCreator", **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    acc = fatrasPhysicsValidationToolCfg(flags)
    phys_val_cfg = acc.getPublicTool('ISF_FatrasPhysicsValidationTool')
    result.merge(acc)
    kwargs.setdefault("PhysicsValidationTool", phys_val_cfg)

    iFatras__PhotonConversionTool = CompFactory.iFatras.PhotonConversionTool
    result.addPublicTool(iFatras__PhotonConversionTool(name=name, **kwargs))

    return result


def fatrasG4HadIntProcessorCfg(flags, name="ISF_FatrasG4HadIntProcessor", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    acc = fatrasPhysicsValidationToolCfg(flags)
    phys_val_cfg = acc.getPublicTool('ISF_FatrasPhysicsValidationTool')
    result.merge(acc)
    kwargs.setdefault("PhysicsValidationTool", phys_val_cfg)

    iFatras__G4HadIntProcessor = CompFactory.iFatras.G4HadIntProcessor
    result.addPublicTool(iFatras__G4HadIntProcessor(name=name, **kwargs))

    return result


def fatrasProcessSamplingToolCfg(flags, name="ISF_FatrasProcessSamplingTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    # decays
    acc = fatrasParticleDecayHelperCfg(flags)
    pd_helper_cfg = acc.getPublicTool('ISF_FatrasParticleDecayHelper')
    result.merge(acc)
    kwargs.setdefault("ParticleDecayHelper", pd_helper_cfg)

    # photon conversion
    acc = fatrasConversionCreatorCfg(flags)
    ph_conv_cfg = acc.getPublicTool('ISF_FatrasConversionCreator')
    result.merge(acc)
    kwargs.setdefault("PhotonConversionTool", ph_conv_cfg)

    # Hadronic interactions
    acc = fatrasG4HadIntProcessorCfg(flags)
    g4had_proc_cfg = acc.getPublicTool('ISF_FatrasG4HadIntProcessor')
    result.merge(acc)
    kwargs.setdefault("HadronicInteractionProcessor", g4had_proc_cfg)

    # Validation Tool
    acc = fatrasPhysicsValidationToolCfg(flags)
    phys_val_cfg = acc.getPublicTool('ISF_FatrasPhysicsValidationTool')
    result.merge(acc)
    kwargs.setdefault("PhysicsValidationTool", phys_val_cfg)

    iFatras__ProcessSamplingTool = CompFactory.iFatras.ProcessSamplingTool
    result.addPublicTool(iFatras__ProcessSamplingTool(name=name, **kwargs))

    return result


def fatrasSimToolCfg(flags, name="ISF_FatrasSimTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    acc = fatrasSimHitCreatorIDCfg(flags)
    id_cfg = acc.getPublicTool('ISF_FatrasSimHitCreatorID')
    result.merge(acc)
    kwargs.setdefault("SimHitCreatorID", id_cfg)

    acc = fatrasSimHitCreatorMSCfg(flags)
    ms_cfg = acc.getPublicTool('ISF_FatrasSimHitCreatorMS')
    result.merge(acc)
    kwargs.setdefault("SimHitCreatorMS", ms_cfg)

    acc = fatrasParticleDecayHelperCfg(flags)
    pdhelper_cfg = acc.getPublicTool('ISF_FatrasParticleDecayHelper')
    result.merge(acc)
    kwargs.setdefault("ParticleDecayHelper", pdhelper_cfg)

    acc = fatrasParticleHelperCfg(flags)
    part_helper_cfg = acc.getPublicTool('ISF_ParticleHelper')
    result.merge(acc)
    kwargs.setdefault("ParticleHelper", part_helper_cfg)

    acc = fatrasKinematicFilterCfg(flags)
    kin_filter_cfg = acc.getPublicTool('ISF_FatrasKinematicFilter')
    result.merge(acc)
    kwargs.setdefault("TrackFilter", kin_filter_cfg)
    kwargs.setdefault("NeutralFilter", kin_filter_cfg)
    kwargs.setdefault("PhotonFilter", kin_filter_cfg)

    acc = fatrasExtrapolatorCfg(flags)
    extrapolator_cfg = acc.getPublicTool('ISF_FatrasExtrapolator')
    kwargs.setdefault("Extrapolator", extrapolator_cfg)
    result.merge(acc)

    acc = fatrasPhysicsValidationToolCfg(flags)
    phys_val_cfg = acc.getPublicTool('ISF_FatrasPhysicsValidationTool')
    result.merge(acc)
    kwargs.setdefault("PhysicsValidationTool", phys_val_cfg)

    acc = fatrasProcessSamplingToolCfg(flags)
    proc_samp_cfg = acc.getPublicTool('ISF_FatrasProcessSamplingTool')
    result.merge(acc)
    kwargs.setdefault("ProcessSamplingTool", proc_samp_cfg)

    iFatras__TransportTool = CompFactory.iFatras.TransportTool
    result.setPrivateTools(iFatras__TransportTool(name=name, **kwargs))
    return result
