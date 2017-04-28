##
## postInclude common to all ITk layouts
##

include("PyJobTransforms/UseFrontier.py")

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.DetFlags import DetFlags

# If /Indet/IBLDist is available, InnerDetector/InDetDetDescr/InDetReadoutGeometry/src/InDetDetectorManager.cxx will try to perform alignment and lead to crash
# InnerDetector/InDetConditions/InDetCondFolders/python/InDetAlignFolders.py configures /Indet/IBLDist
from IOVDbSvc.CondDB import conddb
conddb.blockFolder("/Indet/IBLDist")

if rec.OutputFileNameForRecoStep() == 'RAWtoESD' or rec.OutputFileNameForRecoStep() == 'ESDtoDPD':
    ServiceMgr.PixelOfflineCalibSvc.HDCFromCOOL = False

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

# fixing the EtaPtFilterTool settings for HL-LHC
from McParticleTools.McParticleToolsConf import EtaPtFilterTool
EtaPtFilterTool.InnerEtaRegionCuts = [0.0,4.2,300]
EtaPtFilterTool.OuterEtaRegionCuts = [4.2,5.5,1000]

if rec.OutputFileNameForRecoStep():
    xAODMaker__xAODTruthCnvAlg("GEN_AOD2xAOD",WriteInTimePileUpTruth=True)

## TEMPORARY setting while we fix the values in the PixelCalibCondDB
if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
    if DetFlags.makeRIO.pixel_on():
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc.InDetClusterMakerTool.UsePixelCalibCondDB=False
