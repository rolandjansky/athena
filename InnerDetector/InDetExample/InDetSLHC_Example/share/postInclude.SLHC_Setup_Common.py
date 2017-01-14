##
## postInclude common to all ITk layouts
##

include("PyJobTransforms/UseFrontier.py")

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

# fixing the EtaPtFilterTool settings for HL-LHC
from McParticleTools.McParticleToolsConf import EtaPtFilterTool
EtaPtFilterTool.InnerEtaRegionCuts = [0.0,4.2,300]
EtaPtFilterTool.OuterEtaRegionCuts = [4.2,5.5,1000]


if rec.OutputFileNameForRecoStep():
    xAODMaker__xAODTruthCnvAlg("GEN_AOD2xAOD",WriteInTimePileUpTruth=True)

if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
    from AthenaCommon.AppMgr import ToolSvc

    ## TEMPORARY setting while we fix the values in the PixelCalibCondDB
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.makeRIO.pixel_on():
        ToolSvc.InDetClusterMakerTool.UsePixelCalibCondDB=False

