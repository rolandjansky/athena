from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, "PixelOfflineCalibSvc"):
    from PixelConditionsServices.PixelConditionsServicesConf import PixelOfflineCalibSvc
    ServiceMgr +=PixelOfflineCalibSvc()
    ServiceMgr.PixelOfflineCalibSvc.HDCFromCOOL = False
