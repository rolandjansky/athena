# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addService, addAlgorithm


################################################################################
# Standard BS algorithms
################################################################################
addTool( "MuonCnvExample.MuonReadBSConfig.MdtROD_Decoder",         "MdtROD_Decoder" )
addTool( "MuonCnvExample.MuonReadBSConfig.MdtRawDataProviderTool", "MdtRawDataProviderTool" )
addAlgorithm( "Muon::MdtRawDataProvider",                          "MuonMdtRawDataProvider" )

addTool( "MuonCnvExample.MuonReadBSConfig.RpcROD_Decoder",         "RpcROD_Decoder" )
addTool( "MuonCnvExample.MuonReadBSConfig.RpcRawDataProviderTool", "RpcRawDataProviderTool" )
addAlgorithm( "Muon::RpcRawDataProvider",                          "MuonRpcRawDataProvider" )

addTool( "MuonCnvExample.MuonReadBSConfig.TgcROD_Decoder",         "TgcROD_Decoder" )
addTool( "MuonCnvExample.MuonReadBSConfig.TgcRawDataProviderTool", "TgcRawDataProviderTool" )
addAlgorithm("Muon::TgcRawDataProvider",                           "MuonTgcRawDataProvider" )

addTool( "MuonCnvExample.MuonReadBSConfig.CscROD_Decoder",         "CscROD_Decoder" )
addTool( "MuonCnvExample.MuonReadBSConfig.CscRawDataProviderTool", "CscRawDataProviderTool" )
addAlgorithm("Muon::CscRawDataProvider",                           "MuonCscRawDataProvider" )

addTool( "MuonCnvExample.MuonReadBSConfig.MmROD_Decoder",          "MmROD_Decoder" )
addTool( "MuonCnvExample.MuonReadBSConfig.MmRawDataProviderTool",  "MmRawDataProviderTool" )
addAlgorithm( "Muon::MM_RawDataProvider",                          "MuonMmRawDataProvider" )

addTool( "MuonCnvExample.MuonReadBSConfig.sTgcROD_Decoder",         "sTgcROD_Decoder" )
addTool( "MuonCnvExample.MuonReadBSConfig.sTgcRawDataProviderTool", "sTgcRawDataProviderTool" )
addAlgorithm("Muon::sTgcRawDataProvider",                           "MuonsTgcRawDataProvider" )

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
addAlgorithm( "MuonCnvExample.MuonCnvConfig.CscDigitToCscRDO4", "CscDigitToCscRDO4" )

