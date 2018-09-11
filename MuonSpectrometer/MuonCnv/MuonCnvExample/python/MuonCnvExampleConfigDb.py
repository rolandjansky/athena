# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addToolClone, addService, addAlgorithm

################################################################################
# Standard BS algorithms
################################################################################

addAlgorithm( "Muon::MdtRawDataProvider",                          "MuonMdtRawDataProvider" )
addAlgorithm( "Muon::RpcRawDataProvider",                          "MuonRpcRawDataProvider" )
addAlgorithm("Muon::TgcRawDataProvider",                           "MuonTgcRawDataProvider" )
addAlgorithm("Muon::CscRawDataProvider",                           "MuonCscRawDataProvider" )


################################################################################
# Tools/algorithms/services from MuonCnvExample.MuonCalibConfig
################################################################################
addTool( "MuonCnvExample.MuonCalibConfig.CscCalibTool", "CscCalibTool")
addTool( "MuonCnvExample.MuonCalibConfig.MdtCalibDbTool", "MdtCalibDbTool")
addService( "MuonCnvExample.MuonCalibConfig.MdtCalibrationDbSvc", "MdtCalibrationDbSvc")
addService( "MuonCnvExample.MuonCalibConfig.MdtCalibrationSvc", "MdtCalibrationSvc")


################################################################################
# Tools/algorithms/services from MuonCnvExample.MuonCnvConfig
################################################################################
addTool( "MuonCnvExample.MuonCnvConfig.CscDigitToCscRDOTool", "CscDigitToCscRDOTool" )
addTool( "MuonCnvExample.MuonCnvConfig.CscDigitToCscRDOTool2", "CscDigitToCscRDOTool2" )
addTool( "MuonCnvExample.MuonCnvConfig.CscDigitToCscRDOTool4", "CscDigitToCscRDOTool4" )
addAlgorithm( "MuonCnvExample.MuonCnvConfig.CscDigitToCscRDO", "CscDigitToCscRDO" )

