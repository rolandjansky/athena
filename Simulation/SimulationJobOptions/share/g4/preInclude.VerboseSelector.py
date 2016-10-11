# Include the verbose selector based on track ID
# Very handy for debugging crashes on the grid when you know what track caused the crash
# Most powerful in conjunction with the verbose selector area

try:
    from G4AtlasServices.G4AtlasUserActionConfig import UAStore
except ImportError:
    from G4AtlasServices.UserActionStore import UAStore
from AthenaCommon.CfgGetter import getPublicTool

verboseSelector= getPublicTool('VerboseSelector',tryDefaultConfigurable=True).TargetEvent=1
verboseSelector.TargetTrack=15932
verboseSelector.VerboseLevel=2
verboseSelector.TargetBarcode=-1

UAStore.addAction('VerboseSelector',['EndOfEvent','BeginOfTracking','EndOfTracking'])
