#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------

# build GeoModel geometry
from InDetSLHC_Example.SLHC_Setup_ExtBrl_32 import SLHC_Setup
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
    from AthenaCommon.AppMgr import ToolSvc

    ## TEMPORARY setting while we fix the values in the PixelCalibCondDB 
    ToolSvc.InDetClusterMakerTool.UsePixelCalibCondDB=False

    ## Improved pixel clustering with extended barrel
    ToolSvc.InDetMergedPixelsTool.DoMergeBrokenClusters = True
    ToolSvc.InDetMergedPixelsTool.DoRemoveClustersWithToTequalSize = False
    ToolSvc.InDetMergedPixelsTool.DoCheckSizeBeforeMerging = True

    ## Improved pattern recognition with extended barrel
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
    ToolSvc.InDetSiTrackMakerSLHC.ITKclusterSizeCuts = True

    if InDetFlags.doSLHCVeryForward():
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
        ToolSvc.InDetSiTrackMakerVeryForwardSLHCTracks.ITKclusterSizeCuts = True

if rec.OutputFileNameForRecoStep():
    if InDetFlags.useInDetDynamicCuts():
        SLHCDynamicCutsExtBrl4 = ConfiguredNewTrackingCuts("SLHCDynamicCutsExtBrl4")    #get the cuts for ExtBrl4

        #Set the configurables
        InDetDynamicCutsTool.maxEta                  = SLHCDynamicCutsExtBrl4.maxEta()
        InDetDynamicCutsTool.etaBins                 = SLHCDynamicCutsExtBrl4.etaBins()
        InDetDynamicCutsTool.etaWidthBrem            = SLHCDynamicCutsExtBrl4.etaWidthBrem()
        InDetDynamicCutsTool.maxdImpactSSSSeeds      = SLHCDynamicCutsExtBrl4.maxdImpactSSSSeeds()
        InDetDynamicCutsTool.maxDoubleHoles          = SLHCDynamicCutsExtBrl4.maxDoubleHoles()
        InDetDynamicCutsTool.maxHoles                = SLHCDynamicCutsExtBrl4.maxHoles()
        InDetDynamicCutsTool.maxPixelHoles           = SLHCDynamicCutsExtBrl4.maxPixelHoles()
        InDetDynamicCutsTool.maxPrimaryImpact        = SLHCDynamicCutsExtBrl4.maxPrimaryImpact()
        InDetDynamicCutsTool.maxSctHoles             = SLHCDynamicCutsExtBrl4.maxSCTHoles()
        InDetDynamicCutsTool.maxShared               = SLHCDynamicCutsExtBrl4.maxShared()
        InDetDynamicCutsTool.maxZImpact              = SLHCDynamicCutsExtBrl4.maxZImpact()
        InDetDynamicCutsTool.minClusters             = SLHCDynamicCutsExtBrl4.minClusters()
        InDetDynamicCutsTool.minPixelHits            = SLHCDynamicCutsExtBrl4.minPixel()
        InDetDynamicCutsTool.minPT                   = SLHCDynamicCutsExtBrl4.minPT()
        InDetDynamicCutsTool.minPTBrem               = SLHCDynamicCutsExtBrl4.minPTBrem()
        InDetDynamicCutsTool.minSiNotShared          = SLHCDynamicCutsExtBrl4.minSiNotShared()
        InDetDynamicCutsTool.maxHolesGapPattern      = SLHCDynamicCutsExtBrl4.nHolesGapMax()
        InDetDynamicCutsTool.maxHolesPattern         = SLHCDynamicCutsExtBrl4.nHolesMax()
        InDetDynamicCutsTool.nWeightedClustersMin    = SLHCDynamicCutsExtBrl4.nWeightedClustersMin()
        InDetDynamicCutsTool.phiWidthBrem            = SLHCDynamicCutsExtBrl4.phiWidthBrem()
        InDetDynamicCutsTool.Xi2max                  = SLHCDynamicCutsExtBrl4.Xi2max()
        InDetDynamicCutsTool.Xi2maxNoAdd             = SLHCDynamicCutsExtBrl4.Xi2maxNoAdd()

        if InDetFlags.doPrintConfigurables():
            print InDetDynamicCutsTool
