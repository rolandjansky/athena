# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addToolClone, addService, addAlgorithm

################################################################################
# Tools/algorithms/services from MuonCnvExample.MuonReadBSConfig.py
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
addAlgorithm( "MuonCnvExample.MuonCnvConfig.CscDigitToCscRDO", "CscDigitToCscRDO" )

