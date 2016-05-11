# Modify the hit wrapping action to wrap on 150 ns

try:
    from G4AtlasServices.G4AtlasUserActionConfig import UAStore
except ImportError:
    from G4AtlasServices.UserActionStore import UAStore
from AthenaCommon.CfgGetter import getPublicTool

getPublicTool('HitWrapper',tryDefaultConfigurable=True).WrapTime=150
UAStore.addAction('HitWrapper',['EndOfEvent'])

