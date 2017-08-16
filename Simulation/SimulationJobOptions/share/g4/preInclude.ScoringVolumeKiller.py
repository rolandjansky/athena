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

from G4AtlasApps.SimFlags import simFlags
simFlags.NeutronTimeCut = int(2**31 - 1) #probably not required.
simFlags.NeutronTimeCut.set_Off()

simFlags.OptionalUserActionList.addAction('G4UA::ScoringVolumeTrackKillerTool',['EndOfEvent', 'Step'])
