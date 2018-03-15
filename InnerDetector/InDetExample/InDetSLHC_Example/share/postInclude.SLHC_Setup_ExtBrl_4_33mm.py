#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------

# build GeoModel geometry
from InDetSLHC_Example.SLHC_Setup_ExtBrl_4_33mm import SLHC_Setup
SLHC_Setup = SLHC_Setup()

include("InDetSLHC_Example/postInclude.SLHC_Setup_Common.py")

if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
    from AthenaCommon.AppMgr import ToolSvc

    ## Improved pixel clustering with extended barrel
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.makeRIO.pixel_on():
        ToolSvc.InDetMergedPixelsTool.DoMergeBrokenClusters = True
        ToolSvc.InDetMergedPixelsTool.DoRemoveClustersWithToTequalSize = False
        ToolSvc.InDetMergedPixelsTool.DoCheckSizeBeforeMerging = True
