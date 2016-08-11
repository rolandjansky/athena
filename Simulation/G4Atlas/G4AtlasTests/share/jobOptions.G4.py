## Stripped options for memory benchmarking:

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doMallocMonitoring = True

from AthenaCommon.DetFlags import DetFlags
#DetFlags.ID_setOn()
#DetFlags.Calo_setOn()
#DetFlags.Muon_setOn()
#DetFlags.Truth_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput.set_Off()
athenaCommonFlags.EvtMax = 3

## Global conditions tag
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-RUN12-SDR-19"

from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout.set_On() # uses the default value

simFlags.CalibrationRun.set_Off()
simFlags.EventFilter.set_On()
#simFlags.LArParameterization = 2
simFlags.MagneticField.set_Off()

simFlags.KinematicsMode = 'SingleParticle'
simFlags.ParticlePDG = '999'
simFlags.Energy = 10000

messageSvc = Service("MessageSvc")
messageSvc.OutputLevel = WARNING

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
job += PyG4AtlasAlg()
