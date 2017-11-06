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

from AthenaCommon.CfgGetter import getPrivateTool,getService
getService('DetectorGeometrySvc').RegionCreators += [getPrivateTool('MuonPhysicsRegionTool')]
#FIXME need to add this to the list of RegionCreators, but
#currently initializing the DetectorGeometrySvc too early causes
#problems with the GeoModelSvc configuration.
