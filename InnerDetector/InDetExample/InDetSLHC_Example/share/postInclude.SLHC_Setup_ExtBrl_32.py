#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------

# build GeoModel geometry
from InDetSLHC_Example.SLHC_Setup_ExtBrl_32 import SLHC_Setup
SLHC_Setup = SLHC_Setup()

if rec.OutputFileNameForRecoStep() == 'RAWtoESD' or rec.OutputFileNameForRecoStep() == 'ESDtoDPD':
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.PixelOfflineCalibSvc.HDCFromCOOL = False

## Improved pixel clustering with extended barrel
if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc.InDetMergedPixelsTool.DoMergeBrokenClusters = True
    ToolSvc.InDetMergedPixelsTool.DoRemoveClustersWithToTequalSize = False  # True
    ToolSvc.InDetMergedPixelsTool.DoCheckSizeBeforeMerging = True
