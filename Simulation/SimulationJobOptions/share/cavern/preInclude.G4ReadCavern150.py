# Modify the hit wrapping action to wrap on 150 ns

from G4AtlasServices.G4AtlasUserActionConfig import UAStore
from AthenaCommon.CfgGetter import getPublicTool

getPublicTool('HitWrapper',tryDefaultConfigurable=True).WrapTime=150
UAStore.addAction('HitWrapper',['EndOfEvent']) 

