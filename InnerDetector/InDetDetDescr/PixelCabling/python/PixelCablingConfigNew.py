"""Define a method to construct configured PixelCablingSvc

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Logging import logging
from IOVDbSvc.CondDB import conddb
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline, addFolders
from PixelCabling.PixelCablingConf import PixelCablingSvc
from RecExConfig.AutoConfiguration import GetRunNumber

def PixelCablingSvcCfg(flags, name="PixelCablingSvc", **kwargs):
    """Return a ComponentAccumulator with configured PixelCablingSvc"""
    acc = ComponentAccumulator()
    logger = logging.getLogger("PixelCablingSvc")
    if flags.Input.isMC:
        acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/ReadoutSpeed", "/PIXEL/ReadoutSpeed"))
    else:
        acc.merge(addFolders(flags, "/PIXEL/ReadoutSpeed", "PIXEL"))
    # Add the HitDiscCnfg folder
    if (conddb.dbdata == "CONDBR2" or (conddb.dbmc == "OFLP200" and flags.GeoModel.Run == "RUN2")):
        acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/HitDiscCnfg", "/PIXEL/HitDiscCnfg"))
    # Online mode
    if flags.Common.isOnline:
        logger.debug("Running in online mode")
        # Request the CablingMap folder
        # (COOL is the default mapping type)
        if (conddb.dbdata == "CONDBR2"):
            acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap"))
            logger.debug("Requested CablingMap folder")
        else:
            kwargs.setdefault("MappingType", "Final")
            kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_May08.dat")
    # Offline mode
    else:
        logger.debug("Running in offline mode")
        # SIMULATION
        if flags.Input.isMC:
            logger.debug("Running on simulation")
            # Set cabling svc to get map from file
            kwargs.setdefault("MappingType", "Final")
            # ITk:
            if flags.GeoModel.Run == "RUN4":
                IdMappingDat = "ITk_Atlas_IdMapping.dat"
                if flags.GeoModel.Type == "BrlIncl4.0_ref":
                    IdMappingDat = "ITk_Atlas_IdMapping_InclBrl4.dat"
                elif flags.GeoModel.Type == "IBrlExt4.0ref":
                    IdMappingDat = "ITk_Atlas_IdMapping_IExtBrl4.dat"
                elif flags.GeoModel.Type == "BrlExt4.0_ref":
                    IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl4.dat"
                elif flags.GeoModel.Type == "BrlExt3.2_ref":
                    IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl32.dat"
                logger.info("Using ITk pixel mapping: %s" % IdMappingDat)
                kwargs.setdefault("MappingFile", IdMappingDat)
            # IBL
            elif flags.GeoModel.Run == "RUN2" or flags.GeoModel.Run == "RUN3":
                # Planar IBL
                if flags.GeoModel.IBLLayout == "planar":
                    # DBM or not
                    if flags.GeoModel.Run == "RUN2":
                        kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat")
                    else:
                        kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_inclIBL.dat")
                # Hybrid IBL plus DBM
                elif flags.GeoModel.IBLLayout == "3D":
                    #kwargs.setdefault("MappingFile", "Pixels_Atlas_IdMapping_inclIBL3D_DBM.dat")
                    kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_Run2.dat")
                elif flags.GeoModel.IBLLayout == "UNDEFINED":
                    logger.warning("IBL is required, but flags.GeoModel.IBLLayout is \"UNDEFINED\"")
                    logger.warning("Pixel cabling map cannot be set at this point")
            # No IBL
            else:
                kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping.dat")
        # DATA
        elif not flags.Input.isMC:
            runNum = GetRunNumber()
            logger.debug("Running on data, run number %d" % runNum)
            # For data older than run number 222222, use the appropriate text file
            if runNum < 222222:
                kwargs.setdefault("MappingType", "Final")
                kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_May08.dat")
            # For Run-2 onwards, get cabling map from database
            else:
                kwargs.setdefault("MappingType", "COOL")
                kwargs.setdefault("KeyCabling", "/PIXEL/CablingMap")
                # Request the CablingMap folder
                acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap"))
                logger.debug("Requested CablingMap folder")
                # Even though we are reading from COOL, set the correct fallback map.
                if runNum >= 289350: # 2016
                    kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_2016.dat")
                elif runNum >= 222222 and runNum < 289350: # 2015
                    kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_Run2.dat")
                else:
                    kwargs.setdefault("MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_May08.dat")
        # Unknown input
        else:
            logger.warning("Unknown input source. Pixel cabling map cannot be set at this point")
    acc.addService(PixelCablingSvc(name, **kwargs))
    return acc
