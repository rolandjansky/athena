
## --postExec='ServiceMgr.PixelCablingSvc.MappingFile='PixelCabling/Pixels_Atlas_IdMapping_M5.dat''
## will override the below settings
## To verify the cabling map contents in COOL, on can do
## --postExec='ServiceMgr.PixelCablingSvc.DumpMapToFile=True'


from RecExConfig.AutoConfiguration import *
from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.Logging import logging



logger = logging.getLogger("PixelCablingSvc")
#logger.setLevel(logging.DEBUG)
logger.debug("In SelectPixelMap.py")
#geoFlags.dump()



if not ('conddb' in dir()):
    IOVDbSvc = Service("IOVDbSvc")
    from IOVDbSvc.CondDB import conddb

from AthenaCommon.CfgGetter import getService
PixelCablingSvc = getService("PixelCablingSvc")
ServiceMgr += PixelCablingSvc

#See python/PixelCablingConfing.py for the main configuration.

# Offline mode
if (athenaCommonFlags.isOnline == False):
    # For debugging, print a summary of the chosen settings
    logger.debug("Summary from SelectPixelMap.py:")
    logger.debug("Mapping type: %s" % ServiceMgr.PixelCablingSvc.MappingType)
    if (ServiceMgr.PixelCablingSvc.MappingType == "Final"):
        logger.debug("Selected mapping file: %s" % ServiceMgr.PixelCablingSvc.MappingFile)
    else:
        logger.debug("Cabling folder: %s" % ServiceMgr.PixelCablingSvc.KeyCabling)
        logger.debug("Fallback mapping file: %s" % ServiceMgr.PixelCablingSvc.MappingFile)

