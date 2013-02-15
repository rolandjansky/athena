from G4AtlasApps import PyG4Atlas, AtlasG4Eng

MyAction = PyG4Atlas.UserAction('G4HitFilter','G4HitFilter',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])

# All of the hit collections normally found in a HITS file.
actionProperties = {}
actionProperties["VolumeName"] = "BCMHits"
actionProperties["VolumeName2"] = "BLMHits"
actionProperties["VolumeName3"] = "CSC_Hits"
actionProperties["VolumeName5"] = "LArCalibrationHitActive"
actionProperties["VolumeName6"] = "LArCalibrationHitDeadMaterial"
actionProperties["VolumeName7"] = "LArCalibrationHitInactive"
actionProperties["VolumeName8"] = "LArHitEMB"
actionProperties["VolumeName9"] = "LArHitEMEC"
actionProperties["VolumeName10"] = "LArHitFCAL"
actionProperties["VolumeName11"] = "LArHitHEC"
actionProperties["VolumeName12"] = "MBTSHits"
actionProperties["VolumeName13"] = "MDT_Hits"
actionProperties["VolumeName14"] = "PixelHits"
actionProperties["VolumeName15"] = "RPC_Hits"
actionProperties["VolumeName16"] = "SCT_Hits"
actionProperties["VolumeName17"] = "TGC_Hits"
actionProperties["VolumeName18"] = "TRTUncompressedHits"
actionProperties["VolumeName19"] = "TileHitVec"
#actionProperties["VolumeName20"] = "LucidSimHitsVector" #  LUCID is optional within the G4 setup

MyAction.set_Properties(actionProperties)
AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(MyAction)
