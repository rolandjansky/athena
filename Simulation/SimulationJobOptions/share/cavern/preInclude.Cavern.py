#########################################################
#
# SimulationJobOptions/Cavern.py
# Andrea Dell'Acqua
#
# Setting special range cuts in G4 for use with cavern
# background production.
#########################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.CalibrationRun.set_Off()
simFlags.CavernCuts = True
simFlags.LArParameterization = 0

## Callback for init at level 1
def cavern_setup():
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    myMuon = AtlasG4Eng.G4Eng.Dict_DetFacility.get("Muon:Muon")
    myMuon_Region = PyG4Atlas.PhysicsReg("MuonSys")
    myMuon_Region.add_Volumes("Muon::MuonSys")
    myMuon_Region.add_Cuts("gamma", 0.1)
    myMuon_Region.add_Cuts("e-", 0.1)
    myMuon_Region.add_Cuts("e+", 0.1)
    myMuon.add_PhysicsReg(myMuon_Region)

simFlags.InitFunctions.add_function("preInitPhysicsRegion", cavern_setup)
