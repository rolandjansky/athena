# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = "New configuration for the ISF_FatrasSimTool"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name
from ISF_Tools.ISF_ToolsConfigNew import ParticleHelperCfg
from ISF_Services.ISF_ServicesCoreConfigNew import (
    ParticleBrokerSvcCfg, TruthServiceCfg,
)
from RngComps.RandomServices import RNG
from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg

################################################################################
# HIT CREATION SECTION
################################################################################

#   Fatras Hadronic Interaction Processor
#   hadronic interaction creator


def fatrasHitCreatorPixelCfg(flags, name="ISF_FatrasHitCreatorPixel", **kwargs):
    """Return ISF_FatrasHitCreatorPixel configured with ComponentAccumulator"""
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    hits_collection_name = generate_mergeable_collection_name(bare_collection_name="PixelHits",
                                                              mergeable_collection_suffix="_Fatras",
                                                              merger_input_property="PixelHits")

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)
    kwargs.setdefault("IdHelperName", 'PixelID')
    kwargs.setdefault("CollectionName", hits_collection_name)

    # NOTE   why it is here ?
    # FastHitConvertTool = CompFactory.FastHitConvertTool
    kwargs.setdefault("UseConditionsTool", False)

    iFatras__HitCreatorSilicon = CompFactory.iFatras.HitCreatorSilicon
    result.addPublicTool(iFatras__HitCreatorSilicon(name=name, **kwargs))
    return result


def fatrasHitCreatorSCTCfg(flags, name="ISF_FatrasHitCreatorSCT", **kwargs):
    """Return ISF_FatrasHitCreatorSCT configured with ComponentAccumulator"""
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    hits_collection_name = generate_mergeable_collection_name(bare_collection_name="SCT_Hits",
                                                              mergeable_collection_suffix="_Fatras",
                                                              merger_input_property="SCTHits")
    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)
    kwargs.setdefault("IdHelperName", 'SCT_ID')
    kwargs.setdefault("CollectionName", hits_collection_name)
    kwargs.setdefault("UseConditionsTool", False)

    iFatras__HitCreatorSilicon = CompFactory.iFatras.HitCreatorSilicon
    result.addPublicTool(iFatras__HitCreatorSilicon(name=name, **kwargs))
    return result


def fatrasHitCreatorTRTCfg(flags, name="ISF_FatrasHitCreatorTRT", **kwargs):
    """Return ISF_FatrasHitCreatorTRT configured with ComponentAccumulator"""
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    hits_collection_name = generate_mergeable_collection_name(bare_collection_name="TRTUncompressedHits",
                                                              mergeable_collection_suffix="_Fatras",
                                                              merger_input_property="TRTUncompressedHits")
    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)
    kwargs.setdefault("CollectionName", hits_collection_name)

    iFatras__HitCreatorTRT = CompFactory.iFatras.HitCreatorTRT
    result.addPublicTool(iFatras__HitCreatorTRT(name=name, **kwargs))
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

    kwargs.setdefault("OutputLevel", flags.Exec.OutputLevel)

    iFatras__SimHitCreatorID = CompFactory.iFatras.SimHitCreatorID
    result.addPublicTool(iFatras__SimHitCreatorID(name=name, **kwargs))
    return result


def fatrasPileupSimHitCreatorIDCfg(flags, name="ISF_FatrasPileupSimHitCreatorID", **kwargs):
    """Return ISF_FatrasSimHitCreatorID configured for pileup with ComponentAccumulator"""

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    acc = fatrasPileupHitCreatorPixelCfg(flags)
    pixel_hit_cfg = acc.getPublicTool('ISF_FatrasHitCreatorPixel')
    result.merge(acc)
    kwargs.setdefault("PixelHitCreator", pixel_hit_cfg)

    acc = fatrasPileupHitCreatorSCTCfg(flags)
    sct_hit_cfg = acc.getPublicTool('ISF_FatrasHitCreatorSCT')
    result.merge(acc)
    kwargs.setdefault("SctHitCreator", sct_hit_cfg)

    acc = fatrasPileupHitCreatorTRTCfg(flags)
    trt_hit_cfg = acc.getPublicTool("ISF_FatrasHitCreatorTRT")
    result.merge(acc)
    kwargs.setdefault("TrtHitCreator", trt_hit_cfg)

    return fatrasSimHitCreatorIDCfg(flags, name, **kwargs)


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
    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)
    #####
    # Extrapolator from ACTS to be added TODO
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


def fatrasParticleDecayHelperCfg(flags, name="ISF_FatrasParticleDecayHelper", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)
    kwargs.setdefault("G4RandomStreamName", flags.Sim.Fatras.G4RandomStreamName)
    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)

    result.merge(ParticleBrokerSvcCfg(flags))
    kwargs.setdefault("ParticleBroker", result.getService("ISF_ParticleBrokerSvc"))

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
    kwargs.setdefault("G4RunManagerHelper", g4run_man_cfg)

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

    acc = TrackingGeometrySvcCfg(flags)
    kwargs.setdefault("TrackingGeometrySvc", acc.getPrimary())
    result.merge(acc)

    Trk__Navigator = CompFactory.Trk.Navigator
    result.addPublicTool(Trk__Navigator(name=name, **kwargs))

    return result


def fatrasNeutralPropagatorIDCfg(flags, name="ISF_FatrasNeutralPropagatorID", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()
    
    Trk__StraightLinePropagator = CompFactory.Trk.StraightLinePropagator
    result.addPublicTool(Trk__StraightLinePropagator(name=name, **kwargs))
    
    return result


def fatrasPropagatorCfg(flags, name="ISF_FatrasPropagator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()
    
    Trk__RungeKuttaPropagator = CompFactory.Trk.RungeKuttaPropagator
    result.addPublicTool(Trk__RungeKuttaPropagator(name=name, **kwargs))
    
    return result


# from the Propagator create a Propagation engine to handle path length
def fatrasStaticPropagatorCfg(flags, name="ISF_FatrasStaticPropagator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()
    
    result.merge(fatrasPropagatorCfg(flags))
    kwargs.setdefault("Propagator", result.getPublicTool("ISF_FatrasPropagator"))
    
    # configure output formatting
    kwargs.setdefault("OutputPrefix", "[SP] - ")
    kwargs.setdefault("OutputPostfix", " - ")
    kwargs.setdefault("OutputLevel", flags.Exec.OutputLevel)
    
    Trk__PropagationEngine = CompFactory.Trk.PropagationEngine
    result.addPublicTool(Trk__PropagationEngine(name, **kwargs))
    
    return result


# load the static navigation engine
def fatrasStaticNavigationEngineCfg(flags, name="ISF_FatrasStaticNavigationEngine", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(fatrasStaticPropagatorCfg(flags))
    kwargs.setdefault("PropagationEngine", result.getPublicTool("ISF_FatrasStaticPropagator"))
    result.merge(fatrasMaterialEffectsEngineCfg(flags))
    kwargs.setdefault("MaterialEffectsEngine", result.getPublicTool("ISF_FatrasMaterialEffectsEngine"))
    acc = TrackingGeometrySvcCfg(flags)
    kwargs.setdefault("TrackingGeometrySvc", acc.getPrimary())
    result.merge(acc)
    
    # configure output formatting
    kwargs.setdefault("OutputPrefix", "[SN] - ")
    kwargs.setdefault("OutputPostfix", " - ")
    kwargs.setdefault("OutputLevel", flags.Exec.OutputLevel)
    Trk__StaticNavigationEngine = CompFactory.Trk.StaticNavigationEngine
    acc.setPrivateTools(Trk__StaticNavigationEngine(name, **kwargs))
    return acc


def fatrasEnergyLossUpdatorCfg(flags, name="ISF_FatrasEnergyLossUpdator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)

    kwargs.setdefault("UsePDG_EnergyLossFormula", True)
    kwargs.setdefault("EnergyLossDistribution", 2)

    iFatras__McEnergyLossUpdator = CompFactory.iFatras.McEnergyLossUpdator
    result.addPublicTool(iFatras__McEnergyLossUpdator(name=name, **kwargs))

    return result


def fatrasEnergyLossSamplerBetheHeitlerCfg(flags, name="ISF_FatrasEnergyLossSamplerBetheHeitler", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)

    kwargs.setdefault("ScaleFactor", flags.Sim.Fatras.BetheHeitlerScale)

    iFatras__EnergyLossSamplerBetheHeitler = CompFactory.iFatras.EnergyLossSamplerBetheHeitler
    result.addPublicTool(iFatras__EnergyLossSamplerBetheHeitler(name=name, **kwargs))

    return result


def fatrasMultipleScatteringUpdatorCfg(flags, name="ISF_FatrasMultipleScatteringUpdator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.TrkExRandomStreamName)
    kwargs.setdefault("GaussianMixtureModel", flags.Sim.Fatras.GaussianMixtureModel)

    Trk__MultipleScatteringUpdator = CompFactory.Trk.MultipleScatteringUpdator
    result.addPublicTool(Trk__MultipleScatteringUpdator(name=name, **kwargs))

    return result


def fatrasMultipleScatteringSamplerHighlandCfg(flags, name="ISF_MultipleScatteringSamplerHighland", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.TrkExRandomStreamName)

    Trk__MultipleScatteringSamplerHighland = CompFactory.Trk.MultipleScatteringSamplerHighland
    result.addPublicTool(Trk__MultipleScatteringSamplerHighland(name=name, **kwargs))

    return result


def fatrasMultipleScatteringSamplerGaussianMixtureCfg(flags, name="ISF_MultipleScatteringSamplerGaussianMixture", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.TrkExRandomStreamName)

    Trk__MultipleScatteringSamplerGaussianMixture = CompFactory.Trk.MultipleScatteringSamplerGaussianMixture
    result.addPublicTool(Trk__MultipleScatteringSamplerGaussianMixture(name=name, **kwargs))

    return result


def fatrasMultipleScatteringSamplerGeneralMixtureCfg(flags, name="ISF_MultipleScatteringSamplerGeneralMixture", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.TrkExRandomStreamName)

    Trk__MultipleScatteringSamplerGeneralMixture = CompFactory.Trk.MultipleScatteringSamplerGeneralMixture
    result.addPublicTool(Trk__MultipleScatteringSamplerGeneralMixture(name=name, **kwargs))

    return result


# Combining all in the MaterialEffectsUpdator
def fatrasMaterialUpdatorCfg(flags, name="ISF_FatrasMaterialUpdator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)

    # Geometry Svc
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
    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)
    kwargs.setdefault("BremPhotonValidation", False)
    kwargs.setdefault("EnergyDepositValidation", False)

    kwargs.setdefault("MomentumCut", flags.Sim.Fatras.MomCutOffSec)
    kwargs.setdefault("MinimumBremPhotonMomentum", flags.Sim.Fatras.MomCutOffSec)

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

    # MCTruth Process Code
    kwargs.setdefault("BremProcessCode", 3) # TODO: to be taken from central definition
    acc = TrackingGeometrySvcCfg(flags)
    kwargs.setdefault("TrackingGeometrySvc", acc.getPrimary())
    result.merge(acc)

    iFatras__McMaterialEffectsUpdator = CompFactory.iFatras.McMaterialEffectsUpdator
    result.addPublicTool(iFatras__McMaterialEffectsUpdator(name=name, **kwargs))

    return result



def fatrasMaterialEffectsEngineCfg(flags, name="ISF_FatrasMaterialEffectsEngine", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)

    result.merge(ParticleBrokerSvcCfg(flags))
    kwargs.setdefault("ParticleBroker", result.getService("ISF_ParticleBrokerSvc"))
    
    acc = TruthServiceCfg(flags)
    kwargs.setdefault("TruthRecordSvc", acc.getPrimary())
    result.merge(acc)
    
    result.merge(fatrasProcessSamplingToolCfg(flags))
    kwargs.setdefault("ProcessSamplingTool", result.getPublicTool("ISF_FatrasProcessSamplingTool"))
    
    result.merge(fatrasParticleDecayHelperCfg(flags))
    kwargs.setdefault("ParticleDecayHelper", result.getPublicTool("ISF_FatrasParticleDecayHelper"))
    
    # energy loss
    result.merge(fatrasEnergyLossUpdatorCfg(flags))
    kwargs.setdefault("EnergyLossSampler", result.getPublicTool("ISF_FatrasEnergyLossUpdator"))
    kwargs.setdefault("EnergyLoss", True)
    
    result.merge(fatrasEnergyLossSamplerBetheHeitlerCfg(flags))
    tool = result.getPublicTool("ISF_FatrasEnergyLossSamplerBetheHeitler")
    kwargs.setdefault("ElectronEnergyLossSampler", tool)
    kwargs.setdefault("UseElectronSampler", True)
    kwargs.setdefault("CreateBremPhotons", True)
    
    # multiple scattering
    result.merge(fatrasMultipleScatteringSamplerHighlandCfg(flags))
    tool = result.getPublicTool("ISF_MultipleScatteringSamplerHighland")
    kwargs.setdefault("MultipleScatteringSampler", tool)
    kwargs.setdefault("MultipleScattering", True)
    
    # the properties given throuth the JobProperties interface
    kwargs.setdefault("MomentumCut", flags.Sim.Fatras.MomCutOffSec)
    kwargs.setdefault("MinimumBremPhotonMomentum", flags.Sim.Fatras.MomCutOffSec)
    
    # MCTruth Process Code
    kwargs.setdefault("BremProcessCode", 3) # TODO: to be taken from central definition
    
    # the validation output
    result.merge(fatrasPhysicsValidationToolCfg(flags))
    tool = acc.getPublicTool('ISF_FatrasPhysicsValidationTool')
    kwargs.setdefault("PhysicsValidationTool", tool)
    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)
    
    kwargs.setdefault("OutputPrefix", "[McME] - ")
    kwargs.setdefault("OutputPostfix", " - ")
    kwargs.setdefault("OutputLevel", flags.Exec.OutputLevel)
    
    iFatras__McMaterialEffectsEngine = CompFactory.iFatras.McMaterialEffectsEngine
    result.setPublicTool(iFatras__McMaterialEffectsEngine(name, **kwargs))
    
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
    kwargs.setdefault("MomentumCutOff", flags.Sim.Fatras.MomCutOffSec)
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
    kwargs.setdefault("RobustSampling", True)
    kwargs.setdefault("ResolveMuonStation", True)
    kwargs.setdefault("UseMuonMatApproximation", True)

    TimedExtrapolator = CompFactory.Trk.TimedExtrapolator
    result.addPublicTool(TimedExtrapolator(name=name, **kwargs))

    return result


def fatrasStaticExtrapolatorCfg(flags, name="ISF_FatrasStaticExEngine", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()
    
    result.merge(fatrasStaticPropagatorCfg(flags))
    kwargs.setdefault("PropagationEngine", result.getPublicTool("ISF_FatrasStaticPropagator"))
    result.merge(fatrasMaterialEffectsEngineCfg(flags))
    kwargs.setdefault("MaterialEffectsEngine", result.getPublicTool("ISF_FatrasMaterialEffectsEngine"))
    result.merge(fatrasStaticNavigationEngineCfg(flags))
    kwargs.setdefault("NavigationEngine", result.getPublicTool("ISF_FatrasStaticNavigationEngine"))
    
    # configure output formatting
    kwargs.setdefault("OutputPrefix", "[SE] - ")
    kwargs.setdefault("OutputPostfix", " - ")
    kwargs.setdefault("OutputLevel", flags.Exec.OutputLevel)
    
    Trk__StaticEngine = CompFactory.Trk.StaticEngine
    result.setPrivateTools(Trk__StaticEngine(name, **kwargs))
    return result


def fatrasExEngineCfg(flags, name="ISF_FatrasExEngine", **kwargs):
    # load the tracking geometry service
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()
    
    # assign the tools
    result.merge(fatrasStaticExtrapolatorCfg(flags))
    tool = result.getPublicTool("ISF_FatrasStaticExEngine")
    kwargs.setdefault("ExtrapolationEngines", [tool])
    acc = TrackingGeometrySvcCfg(flags)
    kwargs.setdefault("TrackingGeometrySvc", acc.getPrimary())
    result.merge(acc)
    result.merge(fatrasStaticPropagatorCfg(flags))
    kwargs.setdefault("PropagationEngine", result.getPublicTool("ISF_FatrasStaticPropagator"))
    
    # configure output formatting
    kwargs.setdefault("OutputPrefix", "[ME] - ")
    kwargs.setdefault("OutputPostfix", " - ")
    kwargs.setdefault("OutputLevel", flags.Exec.OutputLevel)
    
    Trk__ExtrapolationEngine = CompFactory.Trk.ExtrapolationEngine
    result.setPrivateTools(Trk__ExtrapolationEngine(name, **kwargs))
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
    result.addPublicTool(ISF__KinematicParticleFilter(name=name, **kwargs))

    return result


def fatrasConversionCreatorCfg(flags, name="ISF_FatrasConversionCreator", **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)

    result.merge(ParticleBrokerSvcCfg(flags))
    kwargs.setdefault("ParticleBroker", result.getService("ISF_ParticleBrokerSvc"))

    acc = fatrasPhysicsValidationToolCfg(flags)
    phys_val_cfg = acc.getPublicTool('ISF_FatrasPhysicsValidationTool')
    result.merge(acc)
    kwargs.setdefault("PhysicsValidationTool", phys_val_cfg)

    kwargs.setdefault("PhysicsProcessCode", 14) # TODO: to be taken from central definition
    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)

    iFatras__PhotonConversionTool = CompFactory.iFatras.PhotonConversionTool
    result.addPublicTool(iFatras__PhotonConversionTool(name=name, **kwargs))

    return result


def fatrasG4HadIntProcessorCfg(flags, name="ISF_FatrasG4HadIntProcessor", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)

    result.merge(ParticleBrokerSvcCfg(flags))
    kwargs.setdefault("ParticleBroker", result.getService("ISF_ParticleBrokerSvc"))
    
    acc = TruthServiceCfg(flags)
    kwargs.setdefault("TruthRecordSvc", acc.getPrimary())
    result.merge(acc)

    result.merge(fatrasPhysicsValidationToolCfg(flags))
    phys_val_cfg = acc.getPublicTool('ISF_FatrasPhysicsValidationTool')
    kwargs.setdefault("PhysicsValidationTool", phys_val_cfg)

    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)
    kwargs.setdefault("MomentumCut", flags.Sim.Fatras.MomCutOffSec)

    iFatras__G4HadIntProcessor = CompFactory.iFatras.G4HadIntProcessor
    result.addPublicTool(iFatras__G4HadIntProcessor(name=name, **kwargs))

    return result


#   Fatras Hadronic Interaction Processor
def fatrasParametricHadIntProcessorCfg(flags, name="ISF_FatrasParametricHadIntProcessor", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", flags.Sim.Fatras.RandomStreamName)
    
    result.merge(ParticleBrokerSvcCfg(flags))
    kwargs.setdefault("ParticleBroker", result.getService("ISF_ParticleBrokerSvc"))
    
    acc = TruthServiceCfg(flags)
    kwargs.setdefault("TruthRecordSvc", acc.getPrimary())
    result.merge(acc)
    
    kwargs.setdefault("HadronicInteractionScaleFactor", flags.Sim.Fatras.HadronIntProb)
    kwargs.setdefault("MinimumHadronicInitialEnergy", flags.Sim.Fatras.MomCutOffSec)
    kwargs.setdefault("MinimumHadronicOutEnergy", flags.Sim.Fatras.MomCutOffSec)
    kwargs.setdefault("HadronicInteractionValidation", False)
    kwargs.setdefault("PhysicsProcessCode", 121) # TODO: to be taken from central definition
    
    result.merge(fatrasPhysicsValidationToolCfg(flags))
    phys_val_cfg = acc.getPublicTool('ISF_FatrasPhysicsValidationTool')
    kwargs.setdefault("PhysicsValidationTool", phys_val_cfg)
    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)
    
    iFatras__HadIntProcessorParametric = CompFactory.iFatras.HadIntProcessorParametric
    result.setPrivateTools(iFatras__HadIntProcessorParametric(name, **kwargs))
    
    return result


def fatrasProcessSamplingToolCfg(flags, name="ISF_FatrasProcessSamplingTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))

    # truth record
    acc = TruthServiceCfg(flags)
    kwargs.setdefault("TruthRecordSvc", acc.getPrimary())
    result.merge(acc)

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
    kwargs.setdefault("HadronicInteraction", True)

    # Validation Tool
    acc = fatrasPhysicsValidationToolCfg(flags)
    phys_val_cfg = acc.getPublicTool('ISF_FatrasPhysicsValidationTool')
    result.merge(acc)
    kwargs.setdefault("PhysicsValidationTool", phys_val_cfg)
    kwargs.setdefault("ValidationMode", flags.Sim.ISF.ValidationMode)

    iFatras__ProcessSamplingTool = CompFactory.iFatras.ProcessSamplingTool
    result.addPublicTool(iFatras__ProcessSamplingTool(name=name, **kwargs))

    return result


def fatrasSimToolCfg(flags, name="ISF_FatrasSimTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    if "SimHitCreatorID" not in kwargs:
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

    acc = ParticleHelperCfg(flags)
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

    kwargs.setdefault("OutputLevel", flags.Exec.OutputLevel)
    kwargs.setdefault("ValidationOutput", flags.Sim.ISF.ValidationMode)
    
    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    
    iFatras__TransportTool = CompFactory.iFatras.TransportTool
    result.addPublicTool(iFatras__TransportTool(name=name, **kwargs))
    return result


def fatrasSimEngineCfg(flags, name="ISF_FatrasSimEngine", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    acc = fatrasSimHitCreatorIDCfg(flags)
    id_cfg = acc.getPublicTool('ISF_FatrasSimHitCreatorID')
    result.merge(acc)
    kwargs.setdefault("SimHitCreatorID", id_cfg)
    
    acc = fatrasParticleDecayHelperCfg(flags)
    pdhelper_cfg = acc.getPublicTool('ISF_FatrasParticleDecayHelper')
    result.merge(acc)
    kwargs.setdefault("ParticleDecayHelper", pdhelper_cfg)

    acc = fatrasKinematicFilterCfg(flags)
    kin_filter_cfg = acc.getPublicTool('ISF_FatrasKinematicFilter')
    result.merge(acc)
    kwargs.setdefault("TrackFilter", kin_filter_cfg)
    kwargs.setdefault("NeutralFilter", kin_filter_cfg)
    kwargs.setdefault("PhotonFilter", kin_filter_cfg)
    
    acc = fatrasExEngineCfg(flags)
    extrapolator_cfg = acc.getPublicTool('ISF_FatrasExEngine')
    kwargs.setdefault("Extrapolator", extrapolator_cfg)
    result.merge(acc)

    acc = fatrasProcessSamplingToolCfg(flags)
    proc_samp_cfg = acc.getPublicTool('ISF_FatrasProcessSamplingTool')
    result.merge(acc)
    kwargs.setdefault("ProcessSamplingTool", proc_samp_cfg)
    
    kwargs.setdefault("OutputLevel", flags.Exec.OutputLevel)
    kwargs.setdefault("ValidationOutput", flags.Sim.ISF.ValidationMode)
    
    result.merge(fatrasPhysicsValidationToolCfg(flags))
    tool = result.getPublicTool("ISF_FatrasPhysicsValidationTool")
    kwargs.setdefault("PhysicsValidationTool", tool)
    
    result.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomNumberService", result.getService("AthRNGSvc"))
    
    iFatras__TransportEngine = CompFactory.iFatras.TransportEngine
    result.addPublicTool(iFatras__TransportEngine(name=name, **kwargs))
    return result


def fatrasPileupSimToolCfg(flags, name="ISF_FatrasPileupSimTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(fatrasPileupSimHitCreatorIDCfg(flags))
    tool = result.getPublicTool("ISF_FatrasPileupSimHitCreatorID")
    kwargs.setdefault("SimHitCreatorID", tool)
    
    result.merge(fatrasSimToolCfg(flags, name, **kwargs))
    return result


# FatrasSimulatorTool
def fatrasSimulatorToolSTCfg(flags, name="ISF_FatrasSimulatorToolST", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(fatrasSimToolCfg(flags))
    tool = result.getPublicTool("ISF_FatrasSimTool")
    kwargs.setdefault("IDSimulationTool", tool)
    kwargs.setdefault("SimulationTool", tool)
    
    kwargs.setdefault("OutputLevel", flags.Exec.OutputLevel)
    
    ISF__FatrasSimTool = CompFactory.ISF.FatrasSimTool
    result.addPublicTool(ISF__FatrasSimTool(name, **kwargs))
    return result


def fatrasNewExtrapolationSimulatorToolSTCfg(flags, name="ISF_FatrasNewExtrapolationSimulatorToolST", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(fatrasSimEngineCfg(flags))
    tool = result.getPrivateTool("ISF_FatrasSimEngine")
    kwargs.setdefault("IDSimulationTool", tool)
    kwargs.setdefault("UseSimulationTool", True)
    
    result.merge(fatrasSimulatorToolSTCfg(flags, name, **kwargs))
    return result


def fatrasPileupSimulatorToolSTCfg(flags, name="ISF_FatrasPileupSimulatorToolST", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(fatrasPileupSimToolCfg(flags))
    tool = result.getPrivateTool("ISF_FatrasPileupSimTool")
    kwargs.setdefault("IDSimulationTool", tool)
    
    result.merge(fatrasSimToolCfg(flags))
    tool = result.getPrivateTool("ISF_FatrasSimTool")
    kwargs.setdefault("SimulationTool", tool)
    
    result.merge(fatrasSimulatorToolSTCfg(flags, name, **kwargs))
    return result


# FatrasSimulationSvc
def fatrasSimServiceIDCfg(flags, name="ISF_FatrasSimSvc", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    kwargs.setdefault("Identifier", "Fatras")
    
    result.merge(fatrasSimulatorToolSTCfg(flags))
    tool = result.getPublicTool("ISF_FatrasSimulatorToolST")
    kwargs.setdefault("SimulatorTool", tool)
    
    result.addService(CompFactory.ISF.LegacySimSvc(name, **kwargs))
    
    return result


def fatrasNewExtrapolationSimServiceIDCfg(flags, name="ISF_FatrasNewExtrapolationSimSvc", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(fatrasNewExtrapolationSimulatorToolSTCfg(flags))
    tool = result.getPublicTool("ISF_FatrasNewExtrapolationSimulatorToolST")
    kwargs.setdefault("SimulatorTool", tool)
    
    result.merge(fatrasSimServiceIDCfg(flags, name, **kwargs))
    
    return result


def fatrasGeoIDFixSimServiceIDCfg(flags, name="ISF_FatrasGeoIDFixSimSvc", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    kwargs.setdefault("EnableGeoIDOverride", True)
    kwargs.setdefault("GeoIDOverrideZ", 3150.)
    kwargs.setdefault("GeoIDOverride", 3) # ISF::fAtlasCalo
    
    result.merge(fatrasSimServiceIDCfg(flags, name, **kwargs))
    
    return result


def fatrasPileupSimServiceIDCfg(flags, name="ISF_FatrasPileupSimSvc", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    result = ComponentAccumulator()

    result.merge(fatrasPileupSimulatorToolSTCfg(flags))
    tool = result.getPublicTool("ISF_FatrasPileupSimulatorToolST")
    kwargs.setdefault("SimulatorTool", tool)
    
    result.merge(fatrasSimServiceIDCfg(flags, name, **kwargs))
    return result
