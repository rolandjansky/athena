from AthenaCommon.CfgGetter import getPublicTool
from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::VolumeDebuggerTool',['Run'])

# Choose a region to check from the following, to set as TargetVolume (dumpAll will give all possibilities): 'IDET::IDET', 'SCT::SCT','Layer3_Sector1_Module_2_3_3_0', 'av_27654_impr_1_SCT::ABC_Chip_pv_3_ABC_Chip','dumpAll', etc...
#------------------------
getPublicTool('G4UA::VolumeDebuggerTool').TargetVolume='Pixel::Pixel'
#------------------------
# Resolution is an integer (so don't try 1.e7)
getPublicTool('G4UA::VolumeDebuggerTool').Resolution=10 # Default = 1000.   smaller is faster
getPublicTool('G4UA::VolumeDebuggerTool').Tolerance=0.
#getPublicTool('G4UA::VolumeDebuggerTool').Verbose=True
getPublicTool('G4UA::VolumeDebuggerTool').RunGeoTest=True
getPublicTool('G4UA::VolumeDebuggerTool').MaxCopiesToCheck=2.
getPublicTool('G4UA::VolumeDebuggerTool').DumpGDML=False
# Set the output file, in case you want to dump the GDML
#getPublicTool('G4UA::VolumeDebuggerTool', tryDefaultConfigurable=True).OutputPath=gdmlFile

