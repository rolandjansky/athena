
## --postExec='ServiceMgr.PixelCablingSvc.MappingFile='Pixels_Atlas_IdMapping_M5.dat'' 
## will override the below settings


from RecExConfig.AutoConfiguration import *
from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.Logging import logging


# Testing:
#logging.getLogger().info("Run number = %d" % GetRunNumber())
#geoFlags.dump()


if not ('conddb' in dir()):
    IOVDbSvc = Service("IOVDbSvc")
    from IOVDbSvc.CondDB import conddb

if ( conddb.dbdata == "CONDBR2" or ( conddb.dbmc == "OFLP200" and geoFlags.isIBL() == True ) ) and not conddb.folderRequested("/PIXEL/HitDiscCnfg"):
    conddb.addFolderSplitMC("PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg")


# Online mode: Let PixelCablingSvc choose the most recent map
if (athenaCommonFlags.isOnline == True):
    pass

# Offline mode
else:

    # SIMULATION
    if (globalflags.DataSource() == 'geant4'):

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
                logging.getLogger("PixelCablingSvc").warning("Got GeometryFlags.isIBL = True, but IBLLayout is UNDEFINED")
                logging.getLogger("PixelCablingSvc").warning("Pixel cabling map cannot be set at this point")


    # DATA
    elif (globalflags.DataSource == 'data'):

        # The Run() parameter only exists for ATLAS-R1(...) and ATLAS-R2(...) geo tags,
        # not for ATLAS-GEO(...) and ATLAS-IBL(...) ones. Hence if Run() is undefined,
        # presence of IBL is used to switch between Run1/Run2
    
        # Run1:
        if (geoFlags.Run() == "RUN1" or (geoFlags.Run() == "UNDEFINED" and geoFlags.isIBL() == False)):
            ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_May08.dat"

        # Run2:
        elif (geoFlags.Run() == "RUN2" or (geoFlags.Run() == "UNDEFINED" and geoFlags.isIBL() == True)):


            # Do map selection based on run number
            runNum = GetRunNumber()

            # M5:
            if (runNum >= 238742 and runNum <= 239908):
                ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_M5.dat"

            # M6:
            elif (runNum > 239908 and runNum <= 242841):
                ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_M6.dat"

            # M7 and M8:
            elif (runNum > 242841 and runNum <= 265796):
                ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_M7.dat"

            # Run2 collisions:
            elif (runNum >= 265797 and runNum < 289350):
                ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_Run2.dat"
            
            # 2016
            elif (runNum >= 289350):
                ServiceMgr.PixelCablingSvc.MappingFile = "Pixels_Atlas_IdMapping_2016.dat"

            else:
                pass

        # Run undefined
        else:
            logging.getLogger("PixelCablingSvc").warning("Pixel cabling map cannot be set at this point")
        
    # Unknown input
    else:
        logging.getLogger("PixelCablingSvc").warning("Unknown input source. Pixel cabling map cannot be set at this point")

try:
    logging.getLogger("PixelCablingSvc").info("Selected map from jobOpts: %s", ServiceMgr.PixelCablingSvc.MappingFile)
except:
    logging.getLogger("PixelCablingSvc").info("Map not selected from jobOpts")
