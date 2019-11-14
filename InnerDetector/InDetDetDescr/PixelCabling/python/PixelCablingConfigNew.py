"""Define a method to construct configured PixelCablingSvc

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Logging import logging
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline, addFolders
from PixelCabling.PixelCablingConf import PixelCablingSvc
from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelReadoutSpeedAlg
from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelCablingCondAlg
from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelHitDiscCnfgAlg


def PixelCablingSvcCfg(flags, name="PixelCablingSvc", **kwargs):
    """Return a ComponentAccumulator with configured PixelCablingSvc"""
    acc = ComponentAccumulator()
    logger = logging.getLogger("PixelCablingSvc")

    if flags.Input.isMC:
        acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/ReadoutSpeed", "/PIXEL/ReadoutSpeed", className="AthenaAttributeList"))
    else:
        acc.merge(addFolders(flags, "/PIXEL/ReadoutSpeed", "PIXEL", className="AthenaAttributeList"))

    # default arguments for PixelCablingCondAlg
    AlgKwargs = {}
    AlgKwargs["UseConditions"] = False
    AlgKwargs["MappingFile"] = "PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
    AlgKwargs["RodIDForSingleLink40"] = 0
    AlgKwargs["DumpMapToFile"] = False

    # Add the HitDiscCnfg folder
    if (flags.IOVDb.DatabaseInstance == "CONDBR2") or (flags.Input.isMC and flags.GeoModel.Run == "RUN2"):
        acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/HitDiscCnfg", "/PIXEL/HitDiscCnfg", className="AthenaAttributeList"))
        acc.addCondAlgo(PixelHitDiscCnfgAlg(name="PixelHitDiscCnfgAlg"))

    # Online mode
    if flags.Common.isOnline:
        logger.debug("Running in online mode")
        # Request the CablingMap folder
        # (COOL is the default mapping type)
        if flags.IOVDb.DatabaseInstance == "CONDBR2":
            acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap", className="AthenaAttributeList"))
            logger.debug("Requested CablingMap folder")

        AlgKwargs["MappingFile"] = "PixelCabling/Pixels_Atlas_IdMapping_344494.dat"
        AlgKwargs["RodIDForSingleLink40"] = 1300000

    # Offline mode
    else:
        logger.debug("Running in offline mode")
        # SIMULATION
        if flags.Input.isMC:
            logger.debug("Running on simulation")

            AlgKwargs["RodIDForSingleLink40"] = 0

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
                logger.info("Using ITk pixel mapping: %s", IdMappingDat)
                AlgKwargs["MappingFile"] = IdMappingDat
            # IBL
            elif flags.GeoModel.Run == "RUN2" or flags.GeoModel.Run == "RUN3":
                # Planar IBL
                if flags.GeoModel.IBLLayout == "planar":
                    # DBM or not
                    if flags.GeoModel.Run == "RUN2":
                        AlgKwargs["MappingFile"] = "PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat"
                    else:
                        AlgKwargs["MappingFile"] = "PixelCabling/Pixels_Atlas_IdMapping_inclIBL.dat"
                # Hybrid IBL plus DBM
                elif flags.GeoModel.IBLLayout == "3D":
                    AlgKwargs["MappingFile"] = "PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
                elif flags.GeoModel.IBLLayout == "UNDEFINED":
                    logger.warning("IBL is required, but flags.GeoModel.IBLLayout is \"UNDEFINED\"")
                    logger.warning("Pixel cabling map cannot be set at this point")
            # No IBL
            else:
                AlgKwargs["MappingFile"] = "PixelCabling/Pixels_Atlas_IdMapping.dat"
        # DATA
        elif not flags.Input.isMC:
            runNum = flags.Input.RunNumber
            logger.debug("Running on data, run number %d", runNum)

            AlgKwargs["RodIDForSingleLink40"] = 1300000

            # For data older than run number 222222, use the appropriate text file
            if runNum < 222222:
                AlgKwargs["MappingFile"] = "PixelCabling/Pixels_Atlas_IdMapping_May08.dat"
            # For Run-2 onwards, get cabling map from database
            else:
                # Request the CablingMap folder
                acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap", className="AthenaAttributeList"))
                logger.debug("Requested CablingMap folder")

                # Even though we are reading from COOL, set the correct fallback map.
                if (runNum >= 344494):
                    AlgKwargs["MappingFile"] = "PixelCabling/Pixels_Atlas_IdMapping_344494.dat"
                elif (runNum >= 314940 and runNum < 344494):
                    AlgKwargs["MappingFile"] = "PixelCabling/Pixels_Atlas_IdMapping_314940.dat"
                elif (runNum >= 289350 and runNum < 314940): # 2016
                    AlgKwargs["MappingFile"] = "PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
                elif (runNum >= 222222 and runNum < 289350): # 2015
                    AlgKwargs["MappingFile"] = "PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
                else:
                    AlgKwargs["MappingFile"] = "PixelCabling/Pixels_Atlas_IdMapping_May08.dat"
        # Unknown input
        else:
            logger.warning("Unknown input source. Pixel cabling map cannot be set at this point")

    acc.addCondAlgo(PixelReadoutSpeedAlg(name="PixelReadoutSpeedAlg"))
    acc.addCondAlgo(PixelCablingCondAlg(name="PixelCablingCondAlg", **AlgKwargs))

    acc.addService(PixelCablingSvc(name, **kwargs))
    return acc
