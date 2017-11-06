# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getPixelCablingSvc(name="PixelCablingSvc", **kwargs):
    from AthenaCommon.Logging import logging
    logger = logging.getLogger("PixelCablingSvc")
    from IOVDbSvc.CondDB import conddb
    from AthenaCommon.GlobalFlags import globalflags
    from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if not conddb.folderRequested("/PIXEL/ReadoutSpeed"):
        #FIXME is this only needed for RECO?
        if not (globalflags.DataSource() == 'geant4'):
            conddb.addFolder("PIXEL","/PIXEL/ReadoutSpeed")
        else:
            conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed")
    # Add the HitDiscCnfg folder if it has not been requested yet
    if (conddb.dbdata == "CONDBR2" or (conddb.dbmc == "OFLP200" and geoFlags.isIBL() == True)) and not conddb.folderRequested("/PIXEL/HitDiscCnfg"):
        conddb.addFolderSplitMC("PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg")
    # Online mode
    if (athenaCommonFlags.isOnline == True):
        logger.debug("Running in online mode")

        # Request the CablingMap folder
        # (COOL is the default mapping type)
        if (conddb.dbdata == "CONDBR2"):
            if not conddb.folderRequested("/PIXEL/CablingMap"):
                conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap")
                logger.debug("Requested CablingMap folder")
        else:
            kwargs.setdefault("MappingType", "Final")
            kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_May08.dat")


    # Offline mode
    else:

        logger.debug("Running in offline mode")

        # SIMULATION
        if (globalflags.DataSource() == 'geant4'):
            logger.debug("Running on simulation")

            # Set cabling svc to get map from file
            kwargs.setdefault("MappingType", "Final")
            
            # ITk:
            if geoFlags.isSLHC():
                IdMappingDat = "ITk_Atlas_IdMapping.dat"
                if "BrlIncl4.0_ref" == geoFlags.GeoType():
                    IdMappingDat = "ITk_Atlas_IdMapping_InclBrl4.dat"
                elif "IBrlExt4.0ref" == geoFlags.GeoType():
                    IdMappingDat = "ITk_Atlas_IdMapping_IExtBrl4.dat"
                elif "BrlExt4.0_ref" == geoFlags.GeoType():
                    IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl4.dat"
                elif "BrlExt3.2_ref" == geoFlags.GeoType():
                    IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl32.dat"
                logger.info("Using ITk pixel mapping: %s" % IdMappingDat)
                kwargs.setdefault("MappingFile", IdMappingDat)


            # No IBL
            elif (geoFlags.isIBL() == False):
                kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping.dat")

            else:
                # Planar IBL
                if (geoFlags.IBLLayout() == "planar"):

                    # DBM or not
                    if (geoFlags.isDBM() == True):
                        kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat")
                    else:
                        kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_inclIBL.dat")

                # Hybrid IBL plus DBM
                elif (geoFlags.IBLLayout() == "3D"):
                    #kwargs.setdefault("MappingFile", "Pixels_Atlas_IdMapping_inclIBL3D_DBM.dat")
                    kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_Run2.dat")

                elif (geoFlags.IBLLayout() == "UNDEFINED"):
                    logger.warning("Got GeometryFlags.isIBL = True, but IBLLayout is UNDEFINED")
                    logger.warning("Pixel cabling map cannot be set at this point")


        # DATA
        elif (globalflags.DataSource == 'data'):
            from RecExConfig.AutoConfiguration import GetRunNumber
            runNum = GetRunNumber()
            logger.debug("Running on data, run number %d" % runNum)

            # For data older than run number 222222, use the appropriate text file
            if (runNum < 222222):
                kwargs.setdefault("MappingType", "Final")
                kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_May08.dat")

            # For Run-2 onwards, get cabling map from database
            else:
                kwargs.setdefault("MappingType", "COOL")
                kwargs.setdefault("KeyCabling", "/PIXEL/CablingMap")

                # Request the CablingMap folder
                if not conddb.folderRequested("/PIXEL/CablingMap"):
                    conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap")
                    logger.debug("Requested CablingMap folder")

                # Even though we are reading from COOL, set the correct fallback map.
                if (runNum >= 289350): # 2016
                    kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_2016.dat")
                elif (runNum >= 222222 and runNum < 289350): # 2015
                    kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_Run2.dat")
                else:
                    kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_May08.dat")


        # Unknown input
        else:
            logger.warning("Unknown input source. Pixel cabling map cannot be set at this point")


    return CfgMgr.PixelCablingSvc(name, **kwargs)
