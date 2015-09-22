
# need to set the default SD name

from AthenaCommon.CfgGetter import getPublicTool
cdp = getPublicTool('CalibrationDefaultProcessing',tryDefaultConfigurable=True)
#cdp.TargetRegions=["EMEC","EMB"]

# this SD name is for ATLAS, H8 
cdp.SDName="Default::Dead::Uninstrumented::Calibration::Region";
# this is for H6
#cdp.SDName="Default::Dead::Uninstrumented::Calibration::H6"

# add the CalibrationDefaultProcessing to the list of user actions
from G4AtlasServices.G4AtlasUserActionConfig import UAStore
UAStore.addAction(cdp,['BeginOfEvent','Step'])
