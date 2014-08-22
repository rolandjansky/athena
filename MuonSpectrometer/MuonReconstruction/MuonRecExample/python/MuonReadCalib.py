# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# MDT Calibration tools/services are defined in MuonCnvExample/python/MuonCalibConfig.py
# For backwards compatibility, they are instantiated here, but this whole module
# will be obsolete as soon as all clients have moved to the ConfiguredFactory and all C++ declares the Calib Services & Tools
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from AthenaCommon.CfgGetter import getPublicTool,getService
from MuonRecFlags import muonRecFlags
muonRecFlags.setDefaults()

if muonRecFlags.doCSCs():
    CscCalibTool = getPublicTool("CscCalibTool")

if muonRecFlags.doMDTs():
    MdtDbTool     = getPublicTool("MdtCalibDbTool")
    MdtCalibDbSvc = getService("MdtCalibrationDbSvc")
    MdtCalibSvc   = getService("MdtCalibrationSvc")

