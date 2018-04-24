#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------

# build GeoModel geometry
from InDetSLHC_Example.SLHC_Setup_ExtBrl_32 import SLHC_Setup
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

#    ## Cuts need to be re-tuned in SiSpacePointsSeedTool_xk/src/SiSpacePointsSeedMaker_ITK.cxx
#    ## Improved pattern recognition with extended barrel
#    ToolSvc.InDetSpSeedsMakerSLHC.usePixelClusterCleanUp = True
#    ToolSvc.InDetSpSeedsMakerSLHC.usePixelClusterCleanUpSizePhicutsB = True
#    ToolSvc.InDetSpSeedsMakerSLHC.usePixelClusterCleanUpSizeZcutsB = True
#    ToolSvc.InDetSpSeedsMakerSLHC.usePixelClusterCleanUpSizePhicutsE = True
#    ToolSvc.InDetSpSeedsMakerSLHC.usePixelClusterCleanUpSizeZcutsE = True
#    ToolSvc.InDetSpSeedsMakerSLHC.useITKseedCuts = True
#    ToolSvc.InDetSpSeedsMakerSLHC.useITKseedCutsSizeDiff = True
#    ToolSvc.InDetSpSeedsMakerSLHC.useITKseedCutsDeltaSize = True
#    ToolSvc.InDetSpSeedsMakerSLHC.useITKseedCutsL01hit = True
#    ToolSvc.InDetSpSeedsMakerSLHC.NsigmaClusSizeZcut = 4.0
#    ToolSvc.InDetSpSeedsMakerSLHC.useITKseedCutsPixHole = False
#    ToolSvc.InDetSpSeedsMakerSLHC.useITKseedCutsSctHole = False
#    ToolSvc.InDetSiTrackMakerSLHC.ITKclusterSizeCuts = True
#
#    if InDetFlags.doSLHCVeryForward():
#        ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.usePixelClusterCleanUp = True
#        ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.usePixelClusterCleanUpSizePhicutsB = True
#        ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.usePixelClusterCleanUpSizeZcutsB = True
#        ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.usePixelClusterCleanUpSizePhicutsE = True
#        ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.usePixelClusterCleanUpSizeZcutsE = True
#        ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.useITKseedCuts = True
#        ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.useITKseedCutsSizeDiff = True
#        ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.useITKseedCutsDeltaSize = True
#        ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.useITKseedCutsL01hit = True
#        ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.NsigmaClusSizeZcut = 4.0
#        ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.useITKseedCutsPixHole = False
#        ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.useITKseedCutsSctHole = False
#        ToolSvc.InDetSiTrackMakerVeryForwardSLHCTracks.ITKclusterSizeCuts = True

