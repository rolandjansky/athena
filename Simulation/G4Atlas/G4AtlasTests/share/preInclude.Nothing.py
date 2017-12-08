#Memory benchmarking.  

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doMallocMonitoring = True
jobproperties.PerfMonFlags.doPostProcessing = True

from AthenaCommon.DetFlags import DetFlags
#DetFlags.ID_setOn()
#DetFlags.Calo_setOn()
#DetFlags.Muon_setOn()

from G4AtlasApps.SimFlags import simFlags
#simFlags.load_atlas_flags()
simFlags.SimLayout.set_On() # uses the default value

simFlags.CalibrationRun.set_Off()
simFlags.EventFilter.set_On()

simFlags.MagneticField.set_Off()

from G4AtlasApps import AtlasG4Eng

AtlasG4Eng.G4Eng._InitList.append('init_FastSimModel')
AtlasG4Eng.G4Eng._InitList.append('init_PhysicsRegions')
AtlasG4Eng.G4Eng._InitList.append('init_SenDetector')
AtlasG4Eng.G4Eng._InitList.append('init_MCTruth')
AtlasG4Eng.G4Eng._InitList.append('init_Fields')
AtlasG4Eng.G4Eng._InitList.append('init_FieldIntegrationParameters')
AtlasG4Eng.G4Eng._InitList.append('init_RecEnvelope')
AtlasG4Eng.G4Eng._InitList.append('init_Graphics')
simFlags.ReleaseGeoModel.set_Off()

