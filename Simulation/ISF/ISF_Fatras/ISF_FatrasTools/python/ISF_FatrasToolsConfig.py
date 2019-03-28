# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from ISF_Config.ISF_jobProperties import ISF_Flags # IMPORTANT: Flags must be set before tools are retrieved
from ISF_FatrasServices.ISF_FatrasJobProperties import ISF_FatrasFlags
from ISF_FatrasServices.FatrasTuning import FatrasTuningFlags

######################################################################################
# validation & process sampling
def getFatrasPhysicsValidationTool(name="ISF_FatrasPhysicsValidationTool", **kwargs):
    kwargs.setdefault("ValidationStreamName", "ISFFatras")

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__PhysicsValidationTool
    return iFatras__PhysicsValidationTool(name, **kwargs)


############################################################################
# (1)  Charged Leptons and Hadrons
#
# FATRAS internal physics list
#
# SubProcesses handled
# (i) Hadronic Interactions
#     the G4 MaterialEffectsUpdator

def getFatrasParametricHadIntProcessor(name="ISF_FatrasParametricHadIntProcessor", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    #   Fatras Hadronic Interaction Processor
    #   hadronic interaction creator
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault('ParticleBroker'      , ISF_Flags.ParticleBroker())
    kwargs.setdefault("TruthRecordSvc"      , simFlags.TruthStrategy.TruthServiceName())
    kwargs.setdefault("HadronicInteractionScaleFactor"  , FatrasTuningFlags.HadronicInteractionProbabilityScalor())
    kwargs.setdefault("MinimumHadronicInitialEnergy"    , FatrasTuningFlags.MomCutOffSec())
    kwargs.setdefault("MinimumHadronicOutEnergy"        , FatrasTuningFlags.MomCutOffSec())
    kwargs.setdefault("HadronicInteractionValidation"   , False)
    kwargs.setdefault("PhysicsProcessCode"              , 121) # TODO: to be taken from central definition
    kwargs.setdefault("PhysicsValidationTool"       , 'ISF_FatrasPhysicsValidationTool')
    kwargs.setdefault('ValidationMode'      , ISF_Flags.ValidationMode())


    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__HadIntProcessorParametric
    return iFatras__HadIntProcessorParametric(name, **kwargs )


# (iv) photon conversion
def getFatrasConversionCreator(name="ISF_FatrasConversionCreator", **kwargs):
    #   Fatras Hadronic Interaction Processor
    #   hadronic interaction creator
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault('ParticleBroker'  , ISF_Flags.ParticleBroker())
    kwargs.setdefault("TruthRecordSvc"  , simFlags.TruthStrategy.TruthServiceName())
    kwargs.setdefault("PhysicsValidationTool"       , 'ISF_FatrasPhysicsValidationTool')
    kwargs.setdefault("PhysicsProcessCode"              , 14) # TODO: to be taken from central definition
    kwargs.setdefault('ValidationMode'      , ISF_Flags.ValidationMode())

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__PhotonConversionTool
    return iFatras__PhotonConversionTool(name, **kwargs )


def getFatrasProcessSamplingTool(name="ISF_FatrasProcessSamplingTool", **kwargs):

    # random number service
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault( "RandomNumberService", simFlags.RandomSvc())
    # truth record
    kwargs.setdefault("TruthRecordSvc"      , simFlags.TruthStrategy.TruthServiceName())
    # decays
    kwargs.setdefault("ParticleDecayHelper" , 'ISF_FatrasParticleDecayHelper')
    # photon conversion
    kwargs.setdefault("PhotonConversionTool"     , 'ISF_FatrasConversionCreator')
    # hadronic interactions
    kwargs.setdefault("HadronicInteraction"         , True)
    #kwargs.setdefault("HadronicInteractionProcessor", 'ISF_FatrasParametricHadIntProcessor')
    kwargs.setdefault("HadronicInteractionProcessor", 'ISF_FatrasG4HadIntProcessor')
    # the validation output
    kwargs.setdefault("ValidationMode"              , ISF_Flags.ValidationMode())
    kwargs.setdefault("PhysicsValidationTool"       , 'ISF_FatrasPhysicsValidationTool')

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__ProcessSamplingTool
    return iFatras__ProcessSamplingTool(name, **kwargs)
#
# (ii) Energy Loss
#      - Ionization and Bremstrahlung loss
#      - assing the Bethe-Heitler Eloss updator
def getFatrasEnergyLossUpdator(name="ISF_FatrasEnergyLossUpdator", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("UsePDG_EnergyLossFormula", True)
    kwargs.setdefault("EnergyLossDistribution", 2)
    #kwargs.setdefault("EnergyLossUpdator"  , 'AtlasEnergyLossUpdator')

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__McEnergyLossUpdator
    return iFatras__McEnergyLossUpdator(name, **kwargs )


def getFatrasEnergyLossSamplerBetheHeitler(name="ISF_FatrasEnergyLossSamplerBetheHeitler", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("ScaleFactor"  , FatrasTuningFlags.BetheHeitlerScalor())

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__EnergyLossSamplerBetheHeitler
    return iFatras__EnergyLossSamplerBetheHeitler(name, **kwargs )


#
# (iii) Multiple scattering
def getFatrasMultipleScatteringSamplerHighland(name="ISF_MultipleScatteringSamplerHighland", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed( "TrkExRnd" ):
      simFlags.RandomSeedList.addSeed( "TrkExRnd" , 12412330, 37849324 )
    kwargs.setdefault("RandomNumberService"   , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"      , 'TrkExRnd' ) # TODO: read stream name "TrkExRnd" from Fatras jobProperties
    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__MultipleScatteringSamplerHighland
    return iFatras__MultipleScatteringSamplerHighland(name, **kwargs )


def getFatrasMultipleScatteringSamplerGaussianMixture(name="ISF_MultipleScatteringSamplerGaussianMixture", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed( "TrkExRnd" ):
      simFlags.RandomSeedList.addSeed( "TrkExRnd" , 12412330, 37849324 )
    kwargs.setdefault("RandomNumberService"   , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"      , 'TrkExRnd' ) # TODO: read stream name "TrkExRnd" from Fatras jobProperties
    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__MultipleScatteringSamplerGaussianMixture
    return iFatras__MultipleScatteringSamplerGaussianMixture(name, **kwargs )


def getFatrasMultipleScatteringSamplerGeneralMixture(name="ISF_MultipleScatteringSamplerGeneralMixture", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed( "TrkExRnd" ):
      simFlags.RandomSeedList.addSeed( "TrkExRnd" , 12412330, 37849324 )
    kwargs.setdefault("RandomNumberService"   , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"      , 'TrkExRnd' ) # TODO: read stream name "TrkExRnd" from Fatras jobProperties
    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__MultipleScatteringSamplerGeneralMixture
    return iFatras__MultipleScatteringSamplerGeneralMixture(name, **kwargs )


# Combining all in the MaterialEffectsUpdator
def getFatrasMaterialUpdator(name="ISF_FatrasMaterialUpdator", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault('ParticleBroker'              , ISF_Flags.ParticleBroker())
    kwargs.setdefault("TruthRecordSvc"              , simFlags.TruthStrategy.TruthServiceName())
    # hadronic interactions
    kwargs.setdefault("HadronicInteraction"         , True)
    #kwargs.setdefault("HadronicInteractionProcessor", 'ISF_FatrasParametricHadIntProcessor')
    kwargs.setdefault("HadronicInteractionProcessor", 'ISF_FatrasG4HadIntProcessor')
    # enrgy loss
    kwargs.setdefault("EnergyLoss"                  , True)
    kwargs.setdefault("EnergyLossUpdator"           , 'ISF_FatrasEnergyLossUpdator')
    # mutiple scattering
    kwargs.setdefault("MultipleScattering"          , True)
    kwargs.setdefault("MultipleScatteringUpdator"   , 'ISF_FatrasMultipleScatteringUpdator')
    # photon conversion
    kwargs.setdefault("PhotonConversionTool"        , 'ISF_FatrasConversionCreator')
    # the validation output
    kwargs.setdefault("ValidationMode"              , ISF_Flags.ValidationMode())
    kwargs.setdefault("BremPhotonValidation"        , False)
    kwargs.setdefault("EnergyDepositValidation"     , False)
    # the properties given throuth the JobProperties interface
    kwargs.setdefault("MomentumCut"                 , FatrasTuningFlags.MomCutOffSec())
    kwargs.setdefault("MinimumBremPhotonMomentum"   , FatrasTuningFlags.MomCutOffSec())
    #
    kwargs.setdefault("PhysicsValidationTool"       , 'ISF_FatrasPhysicsValidationTool')
    kwargs.setdefault("ProcessSamplingTool"         , 'ISF_FatrasProcessSamplingTool')
    kwargs.setdefault("ParticleDecayHelper"             , 'ISF_FatrasParticleDecayHelper')
    # MCTruth Process Code
    kwargs.setdefault("BremProcessCode"             , 3) # TODO: to be taken from central definition
    kwargs.setdefault("TrackingGeometrySvc"         , AtlasTrackingGeometrySvc)

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__McMaterialEffectsUpdator
    return iFatras__McMaterialEffectsUpdator(name, **kwargs )


def getFatrasMaterialEffectsEngine(name="ISF_FatrasMaterialEffectsEngine", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("RandomNumberService"         , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"            , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault('ParticleBroker'              , ISF_Flags.ParticleBroker())
    kwargs.setdefault("TruthRecordSvc"              , simFlags.TruthStrategy.TruthServiceName())
    kwargs.setdefault("ProcessSamplingTool"         , 'ISF_FatrasProcessSamplingTool')
    kwargs.setdefault("ParticleDecayHelper"         , 'ISF_FatrasParticleDecayHelper')
    # energy loss
    kwargs.setdefault("EnergyLoss"                  , True)
    kwargs.setdefault("EnergyLossSampler"           , 'ISF_FatrasEnergyLossUpdator')
    kwargs.setdefault("UseElectronSampler"          , True)
    kwargs.setdefault("ElectronEnergyLossSampler"   , 'ISF_FatrasEnergyLossSamplerBetheHeitler')
    kwargs.setdefault("CreateBremPhotons"           , True)
    # multiple scattering
    kwargs.setdefault("MultipleScattering"          , True)
    kwargs.setdefault("MultipleScatteringSampler"   , 'ISF_FatrasMultipleScatteringSamplerHighland')
    # the properties given throuth the JobProperties interface
    kwargs.setdefault("MomentumCut"                 , FatrasTuningFlags.MomCutOffSec())
    kwargs.setdefault("MinimumBremPhotonMomentum"   , FatrasTuningFlags.MomCutOffSec())
    # MCTruth Process Code
    kwargs.setdefault("BremProcessCode"             , 3) # TODO: to be taken from central definition
    # the validation output
    kwargs.setdefault("ValidationMode"              , ISF_Flags.ValidationMode())
    kwargs.setdefault("PhysicsValidationTool"       , 'ISF_FatrasPhysicsValidationTool')
    kwargs.setdefault("OutputPrefix", '[McME] - ')
    kwargs.setdefault("OutputPostfix", ' - ')
    kwargs.setdefault("OutputLevel", ISF_FatrasFlags.OutputLevelGeneral())

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__McMaterialEffectsEngine
    return iFatras__McMaterialEffectsEngine(name, **kwargs )


################################################################################
# SIMULATION TOOL and SERVICE
################################################################################
def getFatrasSimTool(name="ISF_FatrasSimTool", **kwargs):
    kwargs.setdefault("SimHitCreatorID" , 'ISF_FatrasSimHitCreatorID')
    kwargs.setdefault("SimHitCreatorMS" , 'ISF_FatrasSimHitCreatorMS')
    # TODO: G4 Tools can not be used at the same time as Geant4 inside ISF
    kwargs.setdefault("ParticleDecayHelper" , 'ISF_FatrasParticleDecayHelper')
    kwargs.setdefault("ParticleHelper"      , 'ISF_ParticleHelper')
    # the filter setup
    kwargs.setdefault("TrackFilter"         , 'ISF_FatrasKinematicFilter')
    kwargs.setdefault("NeutralFilter"       , 'ISF_FatrasKinematicFilter')
    kwargs.setdefault("PhotonFilter"        , 'ISF_FatrasKinematicFilter')
    # extrapolator - test setup
    kwargs.setdefault("Extrapolator"           , 'ISF_FatrasExtrapolator')
    #
    kwargs.setdefault("PhysicsValidationTool"       , 'ISF_FatrasPhysicsValidationTool')
    kwargs.setdefault("ProcessSamplingTool"         , 'ISF_FatrasProcessSamplingTool')
    # set the output level
    kwargs.setdefault("OutputLevel"         , ISF_FatrasFlags.OutputLevelGeneral())
    kwargs.setdefault("ValidationOutput"              , ISF_Flags.ValidationMode())
    # random number service
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault( "RandomNumberService", simFlags.RandomSvc())

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__TransportTool
    return iFatras__TransportTool(name, **kwargs )


def getFatrasPileupSimTool(name="ISF_FatrasPileupSimTool", **kwargs):
    kwargs.setdefault("SimHitCreatorID" , 'ISF_FatrasPileupSimHitCreatorID')
    return getFatrasSimTool(name, **kwargs)


def getFatrasSimEngine(name="ISF_FatrasSimEngine", **kwargs):
    kwargs.setdefault("SimHitCreatorID" , 'ISF_FatrasSimHitCreatorID')
    # TODO: G4 Tools can not be used at the same time as Geant4 inside ISF
    kwargs.setdefault("ParticleDecayHelper" , 'ISF_FatrasParticleDecayHelper')
    # the filter setup
    kwargs.setdefault("TrackFilter"         , 'ISF_FatrasKinematicFilter')
    kwargs.setdefault("NeutralFilter"       , 'ISF_FatrasKinematicFilter')
    kwargs.setdefault("PhotonFilter"        , 'ISF_FatrasKinematicFilter')
    # extrapolator - test setup
    kwargs.setdefault("Extrapolator"        , 'ISF_FatrasExEngine')
    #
    kwargs.setdefault("ProcessSamplingTool" , 'ISF_FatrasProcessSamplingTool')
    # set the output level
    # kwargs.setdefault("OutputLevel"         , ISF_FatrasFlags.OutputLevelGeneral())
    # the validation
    kwargs.setdefault("ValidationMode"              , ISF_Flags.ValidationMode())
    kwargs.setdefault("PhysicsValidationTool"       , 'ISF_FatrasPhysicsValidationTool')
    # random number service
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault( "RandomNumberService", simFlags.RandomSvc())

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__TransportEngine
    return iFatras__TransportEngine(name, **kwargs )
