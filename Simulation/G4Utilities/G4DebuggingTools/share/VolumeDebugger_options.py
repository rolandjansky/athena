## Add a volume debug action to the list of user actions
# This should be configured to point at a specific volume
# It will then dump that volume to GDML and run an overlap
# checking routine on that volume

# property setting can be done using ConfiguredFactory, i.e.
# with something like
#
# from AthenaCommon.CfgGetter import getPublicTool
# getPublicTool('VolumeDebugger').property=value

from G4AtlasServices.G4AtlasUserActionConfig import UAStore
UAStore.addAction('VolumeDebugger',['EndOfRun'])

# optional config
#from AthenaCommon.CfgGetter import getPublicTool
#getPublicTool('VolumeDebugger',tryDefaultConfigurable=True).RunGeoTest=True
#getPublicTool('VolumeDebugger').TargetVolume='Muon::Muon'
#getPublicTool('VolumeDebugger').OutputPath='./prova.gdml'
