# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
#from AthenaCommon import CfgMgr

from  G4AtlasAlg.G4AtlasAlgConf import G4AtlasAlg

def getAthenaStackingActionTool(name='G4UA::AthenaStackingActionTool', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    ## Killing neutrinos
    if "ATLAS" in simFlags.SimLayout():
        kwargs.setdefault('KillAllNeutrinos',  True)
    ## Neutron Russian Roulette
    if hasattr(simFlags, 'NRRThreshold') and simFlags.NRRThreshold.statusOn and \
       hasattr(simFlags, 'NRRWeight') and simFlags.NRRWeight.statusOn:
        if simFlags.CalibrationRun.statusOn:
            raise NotImplementedError("Neutron Russian Roulette should not be used in Calibration Runs.")
        kwargs.setdefault('NRRThreshold',  simFlags.NRRThreshold.get_Value())
        kwargs.setdefault('NRRWeight',  simFlags.NRRWeight.get_Value())
    ## Photon Russian Roulette
    if hasattr(simFlags, 'PRRThreshold') and simFlags.PRRThreshold.statusOn and \
       hasattr(simFlags, 'PRRWeight') and simFlags.PRRWeight.statusOn:
        if simFlags.CalibrationRun.statusOn:
            raise NotImplementedError("Photon Russian Roulette should not be used in Calibration Runs.")
        kwargs.setdefault('PRRThreshold',  simFlags.PRRThreshold.get_Value())
        kwargs.setdefault('PRRWeight',  simFlags.PRRWeight.get_Value())
    kwargs.setdefault('IsISFJob', simFlags.ISFRun())
    return CfgMgr.G4UA__AthenaStackingActionTool(name,**kwargs)

def getAthenaTrackingActionTool(name='G4UA::AthenaTrackingActionTool', **kwargs):
    kwargs.setdefault('SecondarySavingLevel', 2)
    subDetLevel=1
    from AthenaCommon.BeamFlags import jobproperties
    from G4AtlasApps.SimFlags import simFlags
    if "ATLAS" in simFlags.SimLayout() and \
    (jobproperties.Beam.beamType() == 'cosmics' or \
     (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ) ):
        subDetLevel=2
    kwargs.setdefault('SubDetVolumeLevel', subDetLevel)
    return CfgMgr.G4UA__AthenaTrackingActionTool(name,**kwargs)

def G4AtlasAlgCfg(ConfigFlags, name='G4AtlasAlg', **kwargs):
    kwargs.setdefault("InputTruthCollection", "BeamTruthEvent")
    kwargs.setdefault("OutputTruthCollection", "TruthEvent")
    ## Killing neutrinos
    from G4AtlasApps.SimFlags import simFlags
    if hasattr(simFlags, 'ReleaseGeoModel') and simFlags.ReleaseGeoModel.statusOn:
        ## Don't drop the GeoModel
        kwargs.setdefault('ReleaseGeoModel' ,simFlags.ReleaseGeoModel.get_Value())

    if hasattr(simFlags, 'RecordFlux') and simFlags.RecordFlux.statusOn:
        ## Record the particle flux during the simulation
        kwargs.setdefault('RecordFlux' ,simFlags.RecordFlux.get_Value())

    if hasattr(simFlags, 'FlagAbortedEvents') and simFlags.FlagAbortedEvents.statusOn:
        ## default false
        kwargs.setdefault('FlagAbortedEvents' ,simFlags.FlagAbortedEvents.get_Value())
        if simFlags.FlagAbortedEvents.get_Value() and simFlags.KillAbortedEvents.get_Value():
            print 'WARNING When G4AtlasAlg.FlagAbortedEvents is True G4AtlasAlg.KillAbortedEvents should be False!!! Setting G4AtlasAlg.KillAbortedEvents = False now!'
            kwargs.setdefault('KillAbortedEvents' ,False)
    if hasattr(simFlags, 'KillAbortedEvents') and simFlags.KillAbortedEvents.statusOn:
        ## default true
        kwargs.setdefault('KillAbortedEvents' ,simFlags.KillAbortedEvents.get_Value())

    if hasattr(simFlags, 'RandomSvcMT') and simFlags.RandomSvcMT.statusOn:
        ## default true
        kwargs.setdefault('AtRndmGenSvc', simFlags.RandomSvcMT.get_Value())
    kwargs.setdefault("RandomGenerator", "athena")

    # Multi-threading settinggs
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    kwargs.setdefault('MultiThreading', is_hive)

    kwargs.setdefault('TruthRecordService', simFlags.TruthStrategy.TruthServiceName())
    kwargs.setdefault('GeoIDSvc', 'ISF_GeoIDSvc')

    ## G4AtlasAlg verbosities (available domains = Navigator, Propagator, Tracking, Stepping, Stacking, Event)
    ## Set stepper verbose = 1 if the Athena logging level is <= DEBUG
    # TODO: Why does it complain that G4AtlasAlgConf.G4AtlasAlg has no "Verbosities" object? Fix.
    verbosities=dict()
    #from AthenaCommon.AppMgr import ServiceMgr
    #if ServiceMgr.MessageSvc.OutputLevel <= 2:
    #    verbosities["Tracking"]='1'
    #    print verbosities
    kwargs.setdefault('Verbosities', verbosities)

    # Set commands for the G4AtlasAlg
    kwargs.setdefault("G4Commands", simFlags.G4Commands.get_Value())

    return G4AtlasAlg(name, **kwargs)






if __name__ == '__main__':
  from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
  import os

  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1


  #import config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT

  # Finalize 
  ConfigFlags.lock()

  ## Initialize a new component accumulator
  cfg = ComponentAccumulator()

  #add the algorithm
  Alg  = G4AtlasAlgCfg(ConfigFlags)
  cfg.addEventAlgo(Alg) #Event algo?
  #cfg.merge(acc)
  #cfg.addPublicTool(Alg)


  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()

  f=open("test.pkl","w")
  cfg.store(f) 
  f.close()



  print cfg._publicTools
  print "-----------------finished----------------------"
