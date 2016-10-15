#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------

# build GeoModel geometry
from InDetSLHC_Example.SLHC_Setup_ExtBrl_4_25x100 import SLHC_Setup
SLHC_Setup = SLHC_Setup()

include("InDetSLHC_Example/postInclude.SLHC_Setup_Common.py")

##   this block is commented out for now, as re-tuning is necessary for 25x100
##       ideally the code should be made aware of pixel pitch to adjust the cuts accordingly
#
#if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
#    ## Improved pixel clustering with extended barrel
#    from AthenaCommon.AppMgr import ToolSvc
#    ToolSvc.InDetMergedPixelsTool.DoMergeBrokenClusters = True
#    ToolSvc.InDetMergedPixelsTool.DoRemoveClustersWithToTequalSize = False
#    ToolSvc.InDetMergedPixelsTool.DoCheckSizeBeforeMerging = True
