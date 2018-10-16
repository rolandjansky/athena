# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF for ISF_FatrasServicesConfig
"""

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *

from ISF_FatrasServices.ISF_FatrasJobProperties import ISF_FatrasFlags

################################################################################
# PARTICLE DECAY SECTION
################################################################################

def getG4RunManagerHelper(name="ISF_G4RunManagerHelper", **kwargs):
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import iGeant4__G4RunManagerHelper
    return iGeant4__G4RunManagerHelper(name, **kwargs)


################################################################################
# SIMULATION TOOL and SERVICE
################################################################################
def getFatrasKinematicFilter(name="ISF_FatrasKinematicFilter", **kwargs):
    kwargs.setdefault("MaxEtaSymmetric" , 10.)
    from ISF_FatrasServices.FatrasTuning import FatrasTuningFlags
    kwargs.setdefault("MinMomentum"     , FatrasTuningFlags.MomCutOffSec())

    from ISF_Tools.ISF_ToolsConf import ISF__KinematicParticleFilter
    return ISF__KinematicParticleFilter(name, **kwargs )


def getFatrasSimServiceID(name="ISF_FatrasSimSvc", **kwargs):
    kwargs.setdefault("Identifier"      , "Fatras")
    kwargs.setdefault("IDSimulationTool"  , 'ISF_FatrasSimTool')
    kwargs.setdefault("SimulationTool"  , 'ISF_FatrasSimTool')

    # set the output level
    kwargs.setdefault("OutputLevel"         , ISF_FatrasFlags.OutputLevelGeneral())

    # register Fatras random number stream if not already registered
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed( "FatrasRnd" ):
      simFlags.RandomSeedList.addSeed( "FatrasRnd", 81234740, 23474923 )

    from ISF_FatrasServices.ISF_FatrasServicesConf import iFatras__FatrasSimSvc
    return iFatras__FatrasSimSvc(name, **kwargs )


def getFatrasNewExtrapolationSimServiceID(name="ISF_FatrasNewExtrapolationSimSvc", **kwargs):
    kwargs.setdefault("Identifier"        , "Fatras")
    kwargs.setdefault("IDSimulationTool"  , 'ISF_FatrasSimEngine')
    kwargs.setdefault("SimulationTool"    , 'ISF_FatrasSimTool')
    kwargs.setdefault("UseSimulationTool" , True)

    # set the output level
    kwargs.setdefault("OutputLevel"       , ISF_FatrasFlags.OutputLevelGeneral())

    # register Fatras random number stream if not already registered
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed( "FatrasRnd" ):
      simFlags.RandomSeedList.addSeed( "FatrasRnd", 81234740, 23474923 )

    from ISF_FatrasServices.ISF_FatrasServicesConf import iFatras__FatrasSimSvc
    return iFatras__FatrasSimSvc(name, **kwargs )


def getFatrasGeoIDFixSimServiceID(name="ISF_FatrasGeoIDFixSimSvc", **kwargs):
    kwargs.setdefault("EnableGeoIDOverride"      , True  )
    kwargs.setdefault("GeoIDOverrideZ"           , 3150. )
    kwargs.setdefault("GeoIDOverride"            , 3     ) # ISF::fAtlasCalo
    return getFatrasSimServiceID(name, **kwargs )


def getFatrasPileupSimServiceID(name="ISF_FatrasPileupSimSvc", **kwargs):
    kwargs.setdefault("IDSimulationTool", 'ISF_FatrasPileupSimTool')
    kwargs.setdefault("SimulationTool"  , 'ISF_FatrasSimTool')
    return getFatrasSimServiceID(name, **kwargs)
