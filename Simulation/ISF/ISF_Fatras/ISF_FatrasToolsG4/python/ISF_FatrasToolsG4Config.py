# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from ISF_Config.ISF_jobProperties import ISF_Flags # IMPORTANT: Flags must be set before tools are retrieved
from ISF_FatrasServices.ISF_FatrasJobProperties import ISF_FatrasFlags
from ISF_FatrasServices.FatrasTuning import FatrasTuningFlags

################################################################################
# PARTICLE DECAY SECTION
################################################################################

def getFatrasPdgG4Particle(name="ISF_FatrasPdgG4Particle", **kwargs):
    from ISF_FatrasToolsG4.ISF_FatrasToolsG4Conf import iFatras__PDGToG4Particle
    return iFatras__PDGToG4Particle(name, **kwargs )


def getFatrasParticleDecayHelper(name="ISF_FatrasParticleDecayHelper", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed( "FatrasG4" ):
      simFlags.RandomSeedList.addSeed( "FatrasG4" , 23491234, 23470291 )
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("G4RandomStreamName"    , "FatrasG4") # TODO: read stream name "FatrasG4" from Fatras jobProperties
    kwargs.setdefault('ParticleBroker'  , ISF_Flags.ParticleBroker())
    kwargs.setdefault("ParticleTruthSvc", simFlags.TruthStrategy.TruthServiceName())
    kwargs.setdefault("PDGToG4ParticleConverter", 'ISF_FatrasPdgG4Particle')
    # the validation output
    kwargs.setdefault("ValidationMode"              , ISF_Flags.ValidationMode())
    kwargs.setdefault("PhysicsValidationTool"       , 'ISF_FatrasPhysicsValidationTool')
    #kwargs.setdefault("G4RunManagerHelper"  , 'ISF_G4RunManagerHelper')

    from ISF_FatrasToolsG4.ISF_FatrasToolsG4Conf import iFatras__G4ParticleDecayHelper
    return iFatras__G4ParticleDecayHelper(name, **kwargs )


############################################################################
# (1)  Charged Leptons and Hadrons
#
# FATRAS internal physics list
#
# SubProcesses handled
# (i) Hadronic Interactions
#     the G4 MaterialEffectsUpdator

def getFatrasG4HadIntProcessor(name="ISF_FatrasG4HadIntProcessor", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault('ParticleBroker'      , ISF_Flags.ParticleBroker())
    kwargs.setdefault("TruthRecordSvc"      , simFlags.TruthStrategy.TruthServiceName())
    kwargs.setdefault("PhysicsValidationTool"       , 'ISF_FatrasPhysicsValidationTool')
    kwargs.setdefault('ValidationMode'      , ISF_Flags.ValidationMode())
    kwargs.setdefault("MomentumCut"        , FatrasTuningFlags.MomCutOffSec())

    from ISF_FatrasToolsG4.ISF_FatrasToolsG4Conf import iFatras__G4HadIntProcessor
    return iFatras__G4HadIntProcessor(name, **kwargs )
