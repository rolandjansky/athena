#ServiceMgr.PixelCablingSvc.UseIBLParameterSvc = False
#ServiceMgr.PixelCablingSvc.MappingFile = "PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat"
#ServiceMgr.PixelCablingSvc.LayerColumnsPerFE = [80]
#ServiceMgr.PixelCablingSvc.LayerRowsPerFE = [336]
#ServiceMgr.PixelCablingSvc.LayerFEsPerHalfModule = [2]
#ServiceMgr.PixelCablingSvc.DBMColumnsPerFE = 80
#ServiceMgr.PixelCablingSvc.DBMRowsPerFE = 336
#ServiceMgr.PixelCablingSvc.DBMFEsPerHalfModule = 2


# Print a warning if this jO is included
from AthenaCommon.Logging import logging
logging.getLogger().warning("Use of PixelCabling postIncludes is depreciated - postInclude.IBLPixelSvcCablingSetupDBM.py has no effect")
