print 'SINGLE PARTICLE GENERATOR'

## Run ParticleGenerator
import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: constant 13',
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: flat -1.0 1.0',
             'phi: flat  0 6.28318',
             'e: constant 5000']
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
job += ParticleGenerator()
job.ParticleGenerator.orders = sorted(spgorders)
from G4AtlasApps.SimFlags import simFlags
job.ParticleGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doMallocMonitoring = True
jobproperties.PerfMonFlags.doPostProcessing = True

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()

#simFlags.load_atlas_flags()
simFlags.SimLayout.set_On() # uses the default value

simFlags.CalibrationRun.set_Off()
simFlags.EventFilter.set_On()

simFlags.MagneticField.set_Off()

simFlags.ReleaseGeoModel.set_Off()
