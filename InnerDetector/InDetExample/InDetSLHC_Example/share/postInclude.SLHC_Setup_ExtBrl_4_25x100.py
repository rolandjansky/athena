#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------

# build GeoModel geometry
from InDetSLHC_Example.SLHC_Setup_ExtBrl_4_25x100 import SLHC_Setup
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

## this block is commented out for now, as re-tuning is necessary for 25x100
##     ideally the code should be made aware of pixel pitch to adjust the cuts accordingly
#if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
#    ## Improved pixel clustering with extended barrel
#    from AthenaCommon.AppMgr import ToolSvc
#    ToolSvc.InDetMergedPixelsTool.DoMergeBrokenClusters = True
#    ToolSvc.InDetMergedPixelsTool.DoRemoveClustersWithToTequalSize = False
#    ToolSvc.InDetMergedPixelsTool.DoCheckSizeBeforeMerging = True
