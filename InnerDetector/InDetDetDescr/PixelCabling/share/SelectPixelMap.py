
## --postExec='ServiceMgr.PixelCablingSvc.MappingFile='Pixels_Atlas_IdMapping_M5.dat''
## will override the below settings


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

# Add the HitDiscCnfg folder if it has not been requested yet
if (conddb.dbdata == "CONDBR2" or (conddb.dbmc == "OFLP200" and geoFlags.isIBL() == True)) and not conddb.folderRequested("/PIXEL/HitDiscCnfg"):
    conddb.addFolderSplitMC("PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg")
    logger.debug("Requested HitDiscCnfg folder")



# Online mode
if (athenaCommonFlags.isOnline == True):
    logger.debug("Running in online mode")

    # Request the CablingMap folder
    # (COOL is the default mapping type)
    if not conddb.folderRequested("/PIXEL/CablingMap"):
        conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap")
        logger.debug("Requested CablingMap folder")



# Offline mode
else:

    logger.debug("Running in offline mode")

    # SIMULATION
    if (globalflags.DataSource() == 'geant4'):
        logger.debug("Running on simulation")

        # Set cabling svc to get map from file
        ServiceMgr.PixelCablingSvc.MappingType = "Final"

        # No IBL
        if (geoFlags.isIBL() == False):
            ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping.dat"

        else:
            # Planar IBL
            if (geoFlags.IBLLayout() == "planar"):

                # DBM or not
                if (geoFlags.isDBM() == True):
                    ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_inclIBL_DBM.dat"
                else:
                    ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_inclIBL.dat"

            # Hybrid IBL plus DBM
            elif (geoFlags.IBLLayout() == "3D"):
                #ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_inclIBL3D_DBM.dat"
                ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_Run2.dat"

            elif (geoFlags.IBLLayout() == "UNDEFINED"):
                logger.warning("Got GeometryFlags.isIBL = True, but IBLLayout is UNDEFINED")
                logger.warning("Pixel cabling map cannot be set at this point")


    # DATA
    elif (globalflags.DataSource == 'data'):

        runNum = GetRunNumber()
        logger.debug("Running on data, run number %d" % runNum)

        # For data older than run number 222222, use the appropriate text file
        if (runNum < 222222):
            ServiceMgr.PixelCablingSvc.MappingType = "Final"
            ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_May08.dat"

        # For Run-2 onwards, get cabling map from database
        else:
            ServiceMgr.PixelCablingSvc.MappingType = "COOL"
            ServiceMgr.PixelCablingSvc.KeyCabling = "/PIXEL/CablingMap"

            # Request the CablingMap folder
            if not conddb.folderRequested("/PIXEL/CablingMap"):
                conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap")
                logger.debug("Requested CablingMap folder")

            # Even though we are reading from COOL, set the correct fallback map.
            if (runNum >= 289350): # 2016
                ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_2016.dat"
            elif (runNum > 265796 and runNum < 289350): # 2015
                ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_Run2.dat"
            elif (runNum > 242841 and runNum <= 265796): # M7 and M8
                ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_M7.dat"
            else:
                ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_May08.dat"


    # Unknown input
    else:
        logger.warning("Unknown input source. Pixel cabling map cannot be set at this point")


    # For debugging, print a summary of the chosen settings
    logger.debug("Summary from SelectPixelMap.py:")
    logger.debug("Mapping type: %s" % ServiceMgr.PixelCablingSvc.MappingType)
    if (ServiceMgr.PixelCablingSvc.MappingType == "Final"):
        logger.debug("Selected mapping file: %s" % ServiceMgr.PixelCablingSvc.MappingFile)
    else:
        logger.debug("Cabling folder: %s" % ServiceMgr.PixelCablingSvc.KeyCabling)
        logger.debug("Fallback mapping file: %s" % ServiceMgr.PixelCablingSvc.MappingFile)

