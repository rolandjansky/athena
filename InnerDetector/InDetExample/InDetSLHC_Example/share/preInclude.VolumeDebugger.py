target_volume = "IDET::IDET"

import os
if 'TARGETVOLUME' in os.environ:
    target_volume = os.environ['TARGETVOLUME']
    print("VolumeDebugger: Running overlap check in "+target_volume)
else:
    print("VolumeDebugger: Running overlap check in default volume ("+target_volume+")")


from AthenaCommon.CfgGetter import getPublicTool
from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::VolumeDebuggerTool',['Run'])

getPublicTool('G4UA::VolumeDebuggerTool').TargetVolume=target_volume
getPublicTool('G4UA::VolumeDebuggerTool').Resolution=1000 # Default = 1000.   smaller is faster, this is an int
getPublicTool('G4UA::VolumeDebuggerTool').Tolerance=0.
getPublicTool('G4UA::VolumeDebuggerTool').Verbose=False
getPublicTool('G4UA::VolumeDebuggerTool').RunGeoTest=True
getPublicTool('G4UA::VolumeDebuggerTool').MaxCopiesToCheck=2.
getPublicTool('G4UA::VolumeDebuggerTool').DumpGDML=True


"""
...some examples of different configuration possibilities
getPublicTool('G4UA::VolumeDebuggerTool', tryDefaultConfigurable=True).OutputPath=gdmlFile
getPublicTool('G4UA::VolumeDebuggerTool').TargetVolume='IDET::IDET'
getPublicTool('G4UA::VolumeDebuggerTool').TargetVolume='SCT::SCT'
getPublicTool('G4UA::VolumeDebuggerTool').TargetVolume='Pixel::Pixel'
getPublicTool('G4UA::VolumeDebuggerTool').TargetVolume='Layer3_Sector1_Module_2_3_3_0'
getPublicTool('G4UA::VolumeDebuggerTool').TargetVolume='av_27654_impr_1_SCT::ABC_Chip_pv_3_ABC_Chip'
getPublicTool('G4UA::VolumeDebuggerTool').TargetVolume='dumpAll'

"""
