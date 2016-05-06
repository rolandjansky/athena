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

if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
    ## Improved pixel clustering with extended barrel
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc.InDetMergedPixelsTool.DoMergeBrokenClusters = True
    ToolSvc.InDetMergedPixelsTool.DoRemoveClustersWithToTequalSize = False
    ToolSvc.InDetMergedPixelsTool.DoCheckSizeBeforeMerging = True

    ## Improved	pattern	recognition with extended barrel
    ToolSvc.InDetSpSeedsMakerSLHC.usePixelClusterCleanUp = True
    ToolSvc.InDetSpSeedsMakerSLHC.usePixelClusterCleanUpSizePhicutsB = True
    ToolSvc.InDetSpSeedsMakerSLHC.usePixelClusterCleanUpSizeZcutsB = True
    ToolSvc.InDetSpSeedsMakerSLHC.usePixelClusterCleanUpSizePhicutsE = True
    ToolSvc.InDetSpSeedsMakerSLHC.usePixelClusterCleanUpSizeZcutsE = True
    ToolSvc.InDetSpSeedsMakerSLHC.useITKseedCuts = True
    ToolSvc.InDetSpSeedsMakerSLHC.useITKseedCutsSizeDiff = True
    ToolSvc.InDetSpSeedsMakerSLHC.useITKseedCutsDeltaSize = True  
    ToolSvc.InDetSpSeedsMakerSLHC.useITKseedCutsL01hit = True  
    ToolSvc.InDetSpSeedsMakerSLHC.NsigmaClusSizeZcut = 4.0
    ToolSvc.InDetSpSeedsMakerSLHC.useITKseedCutsPixHole = False
    ToolSvc.InDetSpSeedsMakerSLHC.useITKseedCutsSctHole = False

    ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.usePixelClusterCleanUp = True
    ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.usePixelClusterCleanUpSizePhicutsB = True
    ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.usePixelClusterCleanUpSizeZcutsB = True
    ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.usePixelClusterCleanUpSizePhicutsE = True
    ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.usePixelClusterCleanUpSizeZcutsE = True
    ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.useITKseedCuts = True
    ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.useITKseedCutsSizeDiff = True
    ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.useITKseedCutsDeltaSize = True
    ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.useITKseedCutsL01hit = True
    ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.NsigmaClusSizeZcut = 4.0
    ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.useITKseedCutsPixHole = False
    ToolSvc.InDetSpSeedsMakerVeryForwardSLHCTracks.useITKseedCutsSctHole = False
