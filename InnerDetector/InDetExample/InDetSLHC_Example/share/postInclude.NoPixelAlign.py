# --- switch alignment for IBL geometry off (gives WARNING)
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
pixelTool = svcMgr.GeoModelSvc.DetectorTools['PixelDetectorTool']
pixelTool.Alignable = False
