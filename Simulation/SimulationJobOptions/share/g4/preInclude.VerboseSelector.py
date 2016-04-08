# Include the verbose selector based on track ID
# Very handy for debugging crashes on the grid when you know what track caused the crash
# Most powerful in conjunction with the verbose selector area

from G4AtlasServices.G4AtlasUserActionConfig import UAStore
from AthenaCommon.CfgGetter import getPublicTool

getPublicTool('VerboseSelector',tryDefaultConfigurable=True).TargetEvent=1
getPublicTool('VerboseSelector').TargetTrack=15932
getPublicTool('VerboseSelector').VerboseLevel=2
getPublicTool('VerboseSelector').TargetBarcode=-1

UAStore.addAction('VerboseSelector',['EndOfEvent','BeginOfTracking','EndOfTracking'])
