# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## -----------------------------------------------------------------------------
### Base Version

def getMCTruthUserActionTool(name='ISFMCTruthUserActionTool', **kwargs):
    # get the MT action
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('TruthRecordSvc',  simFlags.TruthStrategy.TruthServiceName())
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import G4UA__iGeant4__MCTruthUserActionTool
    return G4UA__iGeant4__MCTruthUserActionTool(name, **kwargs)


## -----------------------------------------------------------------------------
### Base Version
def getPhysicsValidationUserActionTool(name="ISFG4PhysicsValidationUserActionTool", **kwargs):
    from AthenaCommon import CfgMgr
    return CfgMgr.G4UA__iGeant4__PhysicsValidationUserActionTool(name, **kwargs)

## -----------------------------------------------------------------------------
### Base Version

def getTrackProcessorUserActionTool(name="ISFG4TrackProcessorUserActionTool", **kwargs):
    from ISF_Config.ISF_jobProperties import ISF_Flags
    kwargs.setdefault('ParticleBroker'     , ISF_Flags.ParticleBroker())
    kwargs.setdefault('GeoIDSvc',       'ISF_GeoIDSvc'         )
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import G4UA__iGeant4__TrackProcessorUserActionPassBackTool
    return G4UA__iGeant4__TrackProcessorUserActionPassBackTool(name, **kwargs)


### Specialized Versions

def getFullG4TrackProcessorUserActionTool(name='FullG4TrackProcessorUserActionTool', **kwargs):
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.Simulator.get_Value() in ['FullG4MT']:
        kwargs.setdefault('EntryLayerTool', 'ISF_EntryLayerToolMT')
    kwargs.setdefault('EntryLayerTool', 'ISF_EntryLayerTool')
    kwargs.setdefault('GeoIDSvc',       'ISF_GeoIDSvc'      )
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.SimulateCavern.get_Value():
        kwargs.setdefault('TruthVolumeLevel',  2)
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import G4UA__iGeant4__TrackProcessorUserActionFullG4Tool
    return G4UA__iGeant4__TrackProcessorUserActionFullG4Tool(name, **kwargs)

def getPassBackG4TrackProcessorUserActionTool(name='PassBackG4TrackProcessorUserActionTool', **kwargs):
    return getTrackProcessorUserActionTool(name, **kwargs)

def getAFII_G4TrackProcessorUserActionTool(name='AFII_G4TrackProcessorUserActionTool', **kwargs):
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.Simulator.get_Value() in ['PassBackG4MT', 'ATLFASTIIMT', 'G4FastCaloMT']:
        kwargs.setdefault('ParticleBroker', '')
    from AthenaCommon.SystemOfUnits import MeV
    kwargs.setdefault('GeoIDSvc'                           , 'ISF_AFIIGeoIDSvc'         )
    kwargs.setdefault('PassBackEkinThreshold'              , 0.05*MeV                   )
    kwargs.setdefault('KillBoundaryParticlesBelowThreshold', True                       )
    return getPassBackG4TrackProcessorUserActionTool(name, **kwargs)

## -----------------------------------------------------------------------------
### Base Version
def getGeant4Tool(name="ISF_Geant4Tool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('RandomNumberService', simFlags.RandomSvcMT())
    kwargs.setdefault('InputConverter', 'ISF_InputConverter')
    kwargs.setdefault('UserActionSvc','G4UA::ISFUserActionSvc')
    if hasattr(simFlags, 'RecordFlux') and simFlags.RecordFlux.statusOn:
        kwargs.setdefault('RecordFlux',simFlags.RecordFlux())

    # Multi-threading settinggs
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() > 0:
        is_hive = True
    else:
        is_hive = False
    kwargs.setdefault('MultiThreading', is_hive)
    # Set commands for the G4AtlasAlg
    kwargs.setdefault("G4Commands", simFlags.G4Commands.get_Value())
    from ISF_Config.ISF_jobProperties import ISF_Flags
    kwargs.setdefault('PrintTimingInfo'       , ISF_Flags.DoTimeMonitoring() )
    kwargs.setdefault('SenDetMasterTool', 'SensitiveDetectorMasterTool')
    kwargs.setdefault('FastSimMasterTool', 'FastSimulationMasterTool')
    from AthenaCommon import CfgMgr
    # Workaround to keep other simulation flavours working while we migrate everything to be AthenaMT-compatible.
    if ISF_Flags.Simulator.get_Value() in ['FullG4', 'FullG4MT', 'PassBackG4', 'PassBackG4MT', 'G4FastCalo', 'G4FastCaloMT']:
        return CfgMgr.iGeant4__G4TransportTool(name, **kwargs)
    else:
        return CfgMgr.iGeant4__G4LegacyTransportTool(name, **kwargs)

def getFullGeant4Tool(name="ISF_FullGeant4Tool", **kwargs):
    kwargs.setdefault('UserActionSvc','G4UA::ISFFullUserActionSvc')
    return getGeant4Tool(name, **kwargs)

### Specialized Versions
def getPassBackGeant4Tool(name="ISF_PassBackGeant4Tool", **kwargs):
    kwargs.setdefault('UserActionSvc','G4UA::ISFPassBackUserActionSvc')
    return getGeant4Tool(name, **kwargs)

def getAFIIGeant4Tool(name="ISF_AFIIGeant4Tool", **kwargs):
    kwargs.setdefault('UserActionSvc','G4UA::ISF_AFIIUserActionSvc')
    return getPassBackGeant4Tool(name, **kwargs)

def getLongLivedGeant4Tool(name="ISF_LongLivedGeant4Tool", **kwargs):
    kwargs.setdefault('InputConverter', 'ISF_LongLivedInputConverter')
    return getFullGeant4Tool(name, **kwargs)

def getAFII_QS_Geant4Tool(name='AFII_QS_Geant4Tool', **kwargs):
    kwargs.setdefault('InputConverter', 'ISF_LongLivedInputConverter')
    return getAFIIGeant4Tool(name, **kwargs)

## -----------------------------------------------------------------------------
