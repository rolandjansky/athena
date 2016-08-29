#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------

# build GeoModel geometry
from InDetSLHC_Example.SLHC_Setup_InclBrl_4_25x100 import SLHC_Setup
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

if rec.OutputFileNameForRecoStep():
    if InDetFlags.useInDetDynamicCuts():
        SLHCDynamicCutsInclBrl4 = ConfiguredNewTrackingCuts("SLHCDynamicCutsInclBrl4")    #get the cuts for InclBrl4

        #Set the configurables
        InDetDynamicCutsTool.maxEta                  = SLHCDynamicCutsInclBrl4.maxEta()
        InDetDynamicCutsTool.etaBins                 = SLHCDynamicCutsInclBrl4.etaBins()
        InDetDynamicCutsTool.etaWidthBrem            = SLHCDynamicCutsInclBrl4.etaWidthBrem()
        InDetDynamicCutsTool.maxdImpactSSSSeeds      = SLHCDynamicCutsInclBrl4.maxdImpactSSSSeeds()
        InDetDynamicCutsTool.maxDoubleHoles          = SLHCDynamicCutsInclBrl4.maxDoubleHoles()
        InDetDynamicCutsTool.maxHoles                = SLHCDynamicCutsInclBrl4.maxHoles()
        InDetDynamicCutsTool.maxPixelHoles           = SLHCDynamicCutsInclBrl4.maxPixelHoles()
        InDetDynamicCutsTool.maxPrimaryImpact        = SLHCDynamicCutsInclBrl4.maxPrimaryImpact()
        InDetDynamicCutsTool.maxSctHoles             = SLHCDynamicCutsInclBrl4.maxSCTHoles()
        InDetDynamicCutsTool.maxShared               = SLHCDynamicCutsInclBrl4.maxShared()
        InDetDynamicCutsTool.maxZImpact              = SLHCDynamicCutsInclBrl4.maxZImpact()
        InDetDynamicCutsTool.minClusters             = SLHCDynamicCutsInclBrl4.minClusters()
        InDetDynamicCutsTool.minPixelHits            = SLHCDynamicCutsInclBrl4.minPixel()
        InDetDynamicCutsTool.minPT                   = SLHCDynamicCutsInclBrl4.minPT()
        InDetDynamicCutsTool.minPTBrem               = SLHCDynamicCutsInclBrl4.minPTBrem()
        InDetDynamicCutsTool.minSiNotShared          = SLHCDynamicCutsInclBrl4.minSiNotShared()
        InDetDynamicCutsTool.maxHolesGapPattern      = SLHCDynamicCutsInclBrl4.nHolesGapMax()
        InDetDynamicCutsTool.maxHolesPattern         = SLHCDynamicCutsInclBrl4.nHolesMax()
        InDetDynamicCutsTool.nWeightedClustersMin    = SLHCDynamicCutsInclBrl4.nWeightedClustersMin()
        InDetDynamicCutsTool.phiWidthBrem            = SLHCDynamicCutsInclBrl4.phiWidthBrem()
        InDetDynamicCutsTool.Xi2max                  = SLHCDynamicCutsInclBrl4.Xi2max()
        InDetDynamicCutsTool.Xi2maxNoAdd             = SLHCDynamicCutsInclBrl4.Xi2maxNoAdd()

        if InDetFlags.doPrintConfigurables():
            print InDetDynamicCutsTool
