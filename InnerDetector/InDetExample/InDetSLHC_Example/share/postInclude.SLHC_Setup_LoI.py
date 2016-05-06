#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------

# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup_LoI import SLHC_Setup
SLHC_Setup = SLHC_Setup()

from AthenaCommon.AppMgr import ServiceMgr

if rec.OutputFileNameForRecoStep() == 'RAWtoESD' or rec.OutputFileNameForRecoStep() == 'ESDtoDPD':
    ServiceMgr.PixelOfflineCalibSvc.HDCFromCOOL = False  

from AthenaCommon.DetFlags import DetFlags
if hasattr(ServiceMgr, 'PixelLorentzAngleSvc'):
    # this avoid to use hard-coded valus for IBL in the LorentzAngleSvc
    print "--> Running with ITkLayout" 
    ServiceMgr.PixelLorentzAngleSvc.ITkLayout = True
    
# checking if you need to force GeoModel in the SiliconConditionsSvc
if hasattr(ServiceMgr, 'PixelSiliconConditionsSvc'):
    print "Forcing the PixelSiliconConditionsSvc to use GeoModel" 
    ServiceMgr.PixelSiliconConditionsSvc.ForceUseGeoModel = True
if hasattr(ServiceMgr, 'SCT_SiliconConditionsSvc'):
    print "Forcing the SCT_SiliconConditionsSvc to use GeoModel" 
    ServiceMgr.SCT_SiliconConditionsSvc.ForceUseGeoModel = True
