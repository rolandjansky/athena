#############################################################
#
#  postOptions.ScoringVolume.py : calls ScoringVolumeKiller
#  while running a G4 simulation of the ATLAS detector.
#  The ScoringVolumeKiller will kill all particles leaving
#  the scoring volumes surrounding the muon stations.
#
#  Dennis Wright (SLAC)
#  25 May 2010
#############################################################


def scoringvolumekiller_setup():
    from G4AtlasApps import AtlasG4Eng, PyG4Atlas
    myMuon = AtlasG4Eng.G4Eng.Dict_DetFacility.get("Muon:Muon")
    myMuon_Region = PyG4Atlas.PhysicsReg('MuonSys')
    myMuon_Region.add_Volumes('Muon::MuonSys')
    myMuon_Region.add_Cuts('gamma', 0.1)
    myMuon_Region.add_Cuts('e-', 0.1)
    myMuon_Region.add_Cuts('e+', 0.1)
    myMuon.add_PhysicsReg(myMuon_Region)

    ## Enable the user actions
    myAction = PyG4Atlas.UserAction('G4UserActions', 'ScoringVolumeTrackKiller', ['BeginOfEvent', 'EndOfEvent', 'Step'])
    #myAction = PyG4Atlas.UserAction('CBG_G4Base', 'ScoringVolumeTrackKiller', ['BeginOfEvent', 'EndOfEvent', 'Step'])
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(myAction)

from G4AtlasApps.SimFlags import simFlags
simFlags.NeutronTimeCut = int(2**31 - 1) #probably not required.
simFlags.NeutronTimeCut.set_Off()
simFlags.InitFunctions.add_function("postInit", scoringvolumekiller_setup)
