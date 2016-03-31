# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## -----------------------------------------------------------------------------
### Base Version
def getMCTruthUserAction(name='ISFMCTruthUserAction', **kwargs):
    from ISF_Config.ISF_jobProperties import ISF_Flags
    kwargs.setdefault('TruthRecordSvc',  ISF_Flags.TruthService.get_Value())
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import iGeant4__MCTruthUserAction
    return iGeant4__MCTruthUserAction(name, **kwargs)

## -----------------------------------------------------------------------------
### Base Version
def getPhysicsValidationUserAction(name="ISFG4PhysicsValidationUserAction", **kwargs):
    kwargs.setdefault('ParticleBroker'     , 'ISF_ParticleBrokerSvc')
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import iGeant4__PhysicsValidationUserAction
    return iGeant4__PhysicsValidationUserAction(name, **kwargs)
### Specialized Versions
def getG4OnlyPhysicsValidationUserAction(name="G4OnlyPhysicsValidationUserAction", **kwargs):
    kwargs.setdefault('ParticleBroker'     , 'ISF_ParticleBrokerSvcNoOrdering')
    return getPhysicsValidationUserAction(name, **kwargs)

def getAFII_G4PhysicsValidationUserAction(name="AFII_G4PhysicsValidationUserAction", **kwargs):
    kwargs.setdefault('ParticleBroker'     , 'ISF_AFIIParticleBrokerSvc')
    return getPhysicsValidationUserAction(name, **kwargs)

def getQuasiStableG4PhysicsValidationUserAction(name="QuasiStableG4PhysicsValidationUserAction", **kwargs):
    kwargs.setdefault('ParticleBroker'     , 'ISF_LongLivedParticleBrokerSvc')
    return getPhysicsValidationUserAction(name, **kwargs)

## -----------------------------------------------------------------------------
### Base Version
def getTrackProcessorUserAction(name="ISFG4TrackProcessorUserAction", **kwargs):
    from AthenaCommon.BeamFlags import jobproperties
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('ParticleBroker', 'ISF_ParticleBrokerSvc')
    kwargs.setdefault('GeoIDSvc',       'ISF_GeoIDSvc'         )
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import iGeant4__TrackProcessorUserActionPassBack
    return iGeant4__TrackProcessorUserActionPassBack(name, **kwargs)

### Specialized Versions
def getFullG4TrackProcessorUserAction(name='FullG4TrackProcessorUserAction', **kwargs):
    kwargs.setdefault('EntryLayerTool', 'ISF_EntryLayerTool')
    kwargs.setdefault('GeoIDSvc',       'ISF_GeoIDSvc'      )
    from AthenaCommon.BeamFlags import jobproperties
    from G4AtlasApps.SimFlags import simFlags
    if jobproperties.Beam.beamType() == 'cosmics' or \
       (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
        kwargs.setdefault('TruthVolumeLevel',  2)
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import iGeant4__TrackProcessorUserActionFullG4
    return iGeant4__TrackProcessorUserActionFullG4(name, **kwargs)

def getPassBackG4TrackProcessorUserAction(name='PassBackG4TrackProcessorUserAction', **kwargs):
    kwargs.setdefault('ParticleBroker', 'ISF_ParticleBrokerSvcNoOrdering')
    return getTrackProcessorUserAction(name, **kwargs)

def getAFII_G4TrackProcessorUserAction(name='AFII_G4TrackProcessorUserAction', **kwargs):
    from AthenaCommon.SystemOfUnits import MeV
    kwargs.setdefault('ParticleBroker'                     , 'ISF_AFIIParticleBrokerSvc')
    kwargs.setdefault('GeoIDSvc'                           , 'ISF_AFIIGeoIDSvc'         )
    kwargs.setdefault('PassBackEkinThreshold'              , 0.05*MeV                   )
    kwargs.setdefault('KillBoundaryParticlesBelowThreshold', True                       )
    return getPassBackG4TrackProcessorUserAction(name, **kwargs)

## -----------------------------------------------------------------------------
### Base Version
def getG4TransportTool(name='ISFG4TransportTool', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    from ISF_Config.ISF_jobProperties import ISF_Flags
    kwargs.setdefault('BarcodeSvc',               ISF_Flags.BarcodeService()     )
    kwargs.setdefault('TrackProcessorUserAction', 'ISFG4TrackProcessorUserAction')
    kwargs.setdefault('RandomGenerator', 'athena')
    kwargs.setdefault('RandomNumberService', simFlags.RandomSvc())
    kwargs.setdefault('PhysicsValidationUserAction', 'ISFG4PhysicsValidationUserAction')
    kwargs.setdefault('MCTruthUserAction', 'ISFMCTruthUserAction')
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import iGeant4__G4TransportTool
    return iGeant4__G4TransportTool(name, **kwargs)
### Specialized Versions
def getFullG4TransportTool(name='FullG4TransportTool', **kwargs):
    kwargs.setdefault('PhysicsValidationUserAction', 'G4OnlyPhysicsValidationUserAction')
    kwargs.setdefault('TrackProcessorUserAction', 'FullG4TrackProcessorUserAction')
    return getG4TransportTool(name, **kwargs)

def getPassBackG4TransportTool(name='PassBackG4TransportTool', **kwargs):
    kwargs.setdefault('PhysicsValidationUserAction', 'G4OnlyPhysicsValidationUserAction')
    kwargs.setdefault('TrackProcessorUserAction', 'PassBackG4TrackProcessorUserAction')
    return getG4TransportTool(name, **kwargs)

def getAFII_G4TransportTool(name='AFII_G4TransportTool', **kwargs):
    kwargs.setdefault('TrackProcessorUserAction', 'AFII_G4TrackProcessorUserAction')
    kwargs.setdefault('PhysicsValidationUserAction', 'AFII_G4PhysicsValidationUserAction')
    return getG4TransportTool(name, **kwargs)

def getQuasiStableG4TransportTool(name='QuasiStableG4TransportTool', **kwargs):
    kwargs.setdefault('TrackProcessorUserAction',    'FullG4TrackProcessorUserAction')
    kwargs.setdefault('PhysicsValidationUserAction', 'QuasiStableG4PhysicsValidationUserAction')
    kwargs.setdefault('QuasiStableParticlesIncluded', True)
    return getG4TransportTool(name, **kwargs)

## -----------------------------------------------------------------------------
