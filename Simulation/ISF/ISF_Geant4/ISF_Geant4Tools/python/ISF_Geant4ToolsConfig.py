# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## -----------------------------------------------------------------------------
### Base Version

def getMCTruthUserActionTool(name='ISFMCTruthUserActionTool', **kwargs):
    # get the MT action
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('TruthRecordSvc',  simFlags.TruthStrategy.TruthServiceName())
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import G4UA__iGeant4__MCTruthUserActionTool
    return G4UA__iGeant4__MCTruthUserActionTool(name, **kwargs)

def addMCTruthUserActionTool(name="ISFMCTruthUserActionTool",system=False):
    from G4AtlasServices import G4AtlasServicesConfig
    G4AtlasServicesConfig.addAction(name,['Run','Tracking'],system)

## -----------------------------------------------------------------------------
### Base Version
def getPhysicsValidationUserActionTool(name="ISFG4PhysicsValidationUserActionTool", **kwargs):
    kwargs.setdefault('ParticleBroker'     , 'ISF_ParticleBrokerSvc')
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import G4UA__iGeant4__PhysicsValidationUserActionTool
    return G4UA__iGeant4__PhysicsValidationUserActionTool(name, **kwargs)
### Specialized Versions
def getG4OnlyPhysicsValidationUserActionTool(name="G4OnlyPhysicsValidationUserActionTool", **kwargs):
    kwargs.setdefault('ParticleBroker'     , 'ISF_ParticleBrokerSvcNoOrdering')
    return getPhysicsValidationUserActionTool(name, **kwargs)

def getAFII_G4PhysicsValidationUserActionTool(name="AFII_G4PhysicsValidationUserActionTool", **kwargs):
    kwargs.setdefault('ParticleBroker'     , 'ISF_AFIIParticleBrokerSvc')
    return getPhysicsValidationUserActionTool(name, **kwargs)

def getQuasiStableG4PhysicsValidationUserActionTool(name="QuasiStableG4PhysicsValidationUserActionTool", **kwargs):
    kwargs.setdefault('ParticleBroker'     , 'ISF_LongLivedParticleBrokerSvc')
    return getPhysicsValidationUserActionTool(name, **kwargs)

## -----------------------------------------------------------------------------
### Base Version

def getTrackProcessorUserActionTool(name="ISFG4TrackProcessorUserActionTool", **kwargs):
    from AthenaCommon.BeamFlags import jobproperties
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('ParticleBroker', 'ISF_ParticleBrokerSvc')
    kwargs.setdefault('GeoIDSvc',       'ISF_GeoIDSvc'         )
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import G4UA__iGeant4__TrackProcessorUserActionPassBackTool
    return G4UA__iGeant4__TrackProcessorUserActionPassBackTool(name, **kwargs)


### Specialized Versions

def getFullG4TrackProcessorUserActionTool(name='FullG4TrackProcessorUserActionTool', **kwargs):
    kwargs.setdefault('EntryLayerTool', 'ISF_EntryLayerTool')
    kwargs.setdefault('GeoIDSvc',       'ISF_GeoIDSvc'      )
    from AthenaCommon.BeamFlags import jobproperties
    from G4AtlasApps.SimFlags import simFlags
    if jobproperties.Beam.beamType() == 'cosmics' or \
       (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
        kwargs.setdefault('TruthVolumeLevel',  2)
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import G4UA__iGeant4__TrackProcessorUserActionFullG4Tool
    return G4UA__iGeant4__TrackProcessorUserActionFullG4Tool(name, **kwargs)

def getPassBackG4TrackProcessorUserActionTool(name='PassBackG4TrackProcessorUserActionTool', **kwargs):
    kwargs.setdefault('ParticleBroker', 'ISF_ParticleBrokerSvcNoOrdering')
    return getTrackProcessorUserActionTool(name, **kwargs)

def getAFII_G4TrackProcessorUserActionTool(name='AFII_G4TrackProcessorUserActionTool', **kwargs):
    from AthenaCommon.SystemOfUnits import MeV
    kwargs.setdefault('ParticleBroker'                     , 'ISF_AFIIParticleBrokerSvc')
    kwargs.setdefault('GeoIDSvc'                           , 'ISF_AFIIGeoIDSvc'         )
    kwargs.setdefault('PassBackEkinThreshold'              , 0.05*MeV                   )
    kwargs.setdefault('KillBoundaryParticlesBelowThreshold', True                       )
    return getPassBackG4TrackProcessorUserActionTool(name, **kwargs)

# all track processors need the same roles

def addTrackProcessorTool(name,system=False):
    from G4AtlasServices import G4AtlasServicesConfig
    G4AtlasServicesConfig.addAction(name,['Run','Tracking','Event','Step'],system)

## -----------------------------------------------------------------------------
### Base Version
def getG4TransportTool(name='ISFG4TransportTool', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('BarcodeSvc', simFlags.TruthStrategy.BarcodeServiceName())
    kwargs.setdefault('RandomGenerator', 'athena')
    kwargs.setdefault('RandomNumberService', simFlags.RandomSvc())

    kwargs.setdefault('UserActionSvc','G4UA::ISFUserActionSvc')

    # Multi-threading settinggs
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() > 0:
        is_hive = True
    else:
        is_hive = False
    kwargs.setdefault('MultiThreading', is_hive)
    # Set commands for the G4AtlasAlg
    kwargs.setdefault("G4Commands", simFlags.G4Commands.get_Value())
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import iGeant4__G4TransportTool
    return iGeant4__G4TransportTool(name, **kwargs)
### Specialized Versions
def getFullG4TransportTool(name='FullG4TransportTool', **kwargs):
    kwargs.setdefault('UserActionSvc','G4UA::ISFFullUserActionSvc')
    return getG4TransportTool(name, **kwargs)

def getPassBackG4TransportTool(name='PassBackG4TransportTool', **kwargs):
    kwargs.setdefault('UserActionSvc','G4UA::ISFPassBackUserActionSvc')
    return getG4TransportTool(name, **kwargs)

def getAFII_G4TransportTool(name='AFII_G4TransportTool', **kwargs):
    kwargs.setdefault('UserActionSvc','G4UA::ISF_AFIIUserActionSvc')
    return getG4TransportTool(name, **kwargs)

def getQuasiStableG4TransportTool(name='QuasiStableG4TransportTool', **kwargs):
    kwargs.setdefault('UserActionSvc','G4UA::ISFQuasiStableUserActionSvc')
    kwargs.setdefault('QuasiStableParticlesIncluded', True)
    return getG4TransportTool(name, **kwargs)

## -----------------------------------------------------------------------------
