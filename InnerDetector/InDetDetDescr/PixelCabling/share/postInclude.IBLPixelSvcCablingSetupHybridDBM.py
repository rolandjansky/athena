# These parameters are no longer accepted by PixelCabling

#ServiceMgr.PixelCablingSvc.UseIBLParameterSvc = False
#ServiceMgr.PixelCablingSvc.LayerColumnsPerFE = [80]
#ServiceMgr.PixelCablingSvc.LayerRowsPerFE = [336]
#ServiceMgr.PixelCablingSvc.LayerFEsPerHalfModule = [2]
#ServiceMgr.PixelCablingSvc.DBMColumnsPerFE = 80
#ServiceMgr.PixelCablingSvc.DBMRowsPerFE = 336
#ServiceMgr.PixelCablingSvc.DBMFEsPerHalfModule = 1
#ServiceMgr.PixelCablingSvc.IBLHybridModuleType = [1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1]

# Overriding the mapping file is possible, but not recommended
#ServiceMgr.PixelCablingSvc.MappingFile = "PixelCabling/Pixels_Atlas_IdMapping_inclIBL3D_DBM.dat"

# Print a warning if this jO is included
from AthenaCommon.Logging import logging
logging.getLogger().warning("Use of PixelCabling postIncludes is depreciated - postInclude.IBLPixelSvcCablingSetupHybridDBM.py has no effect")
