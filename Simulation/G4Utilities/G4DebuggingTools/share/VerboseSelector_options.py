
from G4AtlasServices.G4AtlasUserActionConfig import UAStore
UAStore.addAction('VerboseSelector',['EndOfEvent','BeginOfTracking','EndOfTracking'])
# uncomment to add area checking
# UAStore.addAction('VerboseSelector',['Step'])
# if you want only area checking you can remove the other roles, so the two lines above become
# UAStore.addAction('VerboseSelector',['EndOfEvent','Step'])

# optional config
from AthenaCommon.CfgGetter import getPublicTool
getPublicTool('VerboseSelector',tryDefaultConfigurable=True).TargetEvent=2
getPublicTool('VerboseSelector').TargetTrack=0
getPublicTool('VerboseSelector').VerboseLevel=5

# optional config for area check. requires the action to be added as a stepping action
# these parameters will be ignored if action is not a stepping action

#getPublicTool('VerboseSelector').Xmin=-10000
#getPublicTool('VerboseSelector').Xmax=10000
#getPublicTool('VerboseSelector').Ymax=10000
#getPublicTool('VerboseSelector').Ymin=-10000
#getPublicTool('VerboseSelector').Zmax=10000
#getPublicTool('VerboseSelector').Zmin=-10000
