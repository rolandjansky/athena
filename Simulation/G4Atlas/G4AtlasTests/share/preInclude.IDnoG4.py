include("G4AtlasTests/preInclude.ID.py")

## Disable the SD initialisation
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
