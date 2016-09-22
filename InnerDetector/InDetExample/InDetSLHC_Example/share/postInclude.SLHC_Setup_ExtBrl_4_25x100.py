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
    from AthenaCommon.AppMgr import ToolSvc

    ## TEMPORARY setting while we fix the values in the PixelCalibCondDB
    ToolSvc.InDetClusterMakerTool.UsePixelCalibCondDB=False

##   this block is commented out for now, as re-tuning is necessary for 25x100
##       ideally the code should be made aware of pixel pitch to adjust the cuts accordingly
#    ## Improved pixel clustering with extended barrel
#    from AthenaCommon.AppMgr import ToolSvc
#    ToolSvc.InDetMergedPixelsTool.DoMergeBrokenClusters = True
#    ToolSvc.InDetMergedPixelsTool.DoRemoveClustersWithToTequalSize = False
#    ToolSvc.InDetMergedPixelsTool.DoCheckSizeBeforeMerging = True

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
