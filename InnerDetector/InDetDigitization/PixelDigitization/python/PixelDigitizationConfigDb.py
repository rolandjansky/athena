# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool,addService,addAlgorithm

addTool("PixelDigitization.PixelDigitizationConfig.PixelDigitizationTool", "PixelDigitizationTool")
addTool("PixelDigitization.PixelDigitizationConfig.PixelGeantinoTruthDigitizationTool", "PixelGeantinoTruthDigitizationTool")
addTool("PixelDigitization.PixelDigitizationConfig.PixelDigitizationToolHS", "PixelDigitizationToolHS")
addTool("PixelDigitization.PixelDigitizationConfig.PixelDigitizationToolPU", "PixelDigitizationToolPU")
addTool("PixelDigitization.PixelDigitizationConfig.PixelDigitizationToolSplitNoMergePU", "PixelDigitizationToolSplitNoMergePU")

addAlgorithm("PixelDigitization.PixelDigitizationConfig.PixelDigitizationHS"  , "PixelDigitizationHS")
addAlgorithm("PixelDigitization.PixelDigitizationConfig.PixelDigitizationPU"  , "PixelDigitizationPU")
addTool("PixelDigitization.PixelDigitizationConfig.getPixelRange", "PixelRange")
addService("PixelDigitization.PixelDigitizationConfig.ChargeCollProbSvc","ChargeCollProbSvc")
addTool("PixelDigitization.PixelDigitizationConfig.SensorSimPlanarTool","SensorSimPlanarTool")
addTool("PixelDigitization.PixelDigitizationConfig.SensorSim3DTool","SensorSim3DTool")
addTool("PixelDigitization.PixelDigitizationConfig.SensorSimTool","SensorSimTool")
addTool("PixelDigitization.PixelDigitizationConfig.FrontEndSimTool","FrontEndSimTool")

addTool("PixelDigitization.PixelDigitizationConfig.BarrelFEI4SimTool","BarrelFEI4SimTool")
addTool("PixelDigitization.PixelDigitizationConfig.DBMFEI4SimTool","DBMFEI4SimTool")
addTool("PixelDigitization.PixelDigitizationConfig.BarrelFEI3SimTool","BarrelFEI3SimTool")
addTool("PixelDigitization.PixelDigitizationConfig.EndcapFEI3SimTool","EndcapFEI3SimTool")
addTool("PixelDigitization.PixelDigitizationConfig.BarrelRD53SimTool","BarrelRD53SimTool")
addTool("PixelDigitization.PixelDigitizationConfig.EndcapRD53SimTool","EndcapRD53SimTool")

addTool("PixelDigitization.PixelDigitizationConfig.PixelOverlayDigitizationTool", "PixelOverlayDigitizationTool")
addAlgorithm("PixelDigitization.PixelDigitizationConfig.PixelOverlayDigitization", "PixelOverlayDigitization")
addTool("PixelDigitization.PixelDigitizationConfig.EnergyDepositionTool", "EnergyDepositionTool")

