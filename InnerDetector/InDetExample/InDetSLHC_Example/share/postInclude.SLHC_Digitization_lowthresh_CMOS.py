include("InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py")
getPublicTool("IblPlanarChargeTool").DisableDistortions = True
if DetFlags.digitize.pixel_on():
    ServiceMgr.PixelCalibSvc.IntimeThresh    = 1000
    ServiceMgr.PixelCalibSvc.DiscrThresh     = 500
    ServiceMgr.PixelCalibSvc.DiscrThreshVar  =  20
    ServiceMgr.PixelCalibSvc.NoiseThresh     =  50
