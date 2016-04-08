# Modify the hit wrapping action to wrap on 75 ns

from G4AtlasServices.G4AtlasUserActionConfig import UAStore
from AthenaCommon.CfgGetter import getPublicTool

getPublicTool('HitWrapper',tryDefaultConfigurable=True).WrapTime=75
UAStore.addAction('HitWrapper',['EndOfEvent']) 
