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
addTool("PixelDigitization.PixelDigitizationConfig.PixelProcessorTool","PixelProcessorTool")
addTool("PixelDigitization.PixelDigitizationConfig.PixelNoisyCellGenerator", "PixelNoisyCellGenerator")
addTool("PixelDigitization.PixelDigitizationConfig.PixelChargeSmearer", "PixelChargeSmearer")
addTool("PixelDigitization.PixelDigitizationConfig.PixelDiodeCrossTalkGenerator", "PixelDiodeCrossTalkGenerator")
addTool("PixelDigitization.PixelDigitizationConfig.PixelRandomDisabledCellGenerator", "PixelRandomDisabledCellGenerator")
addTool("PixelDigitization.PixelDigitizationConfig.SensorSimPlanarTool","SensorSimPlanarTool")
addTool("PixelDigitization.PixelDigitizationConfig.SensorSim3DTool","SensorSim3DTool")
addTool("PixelDigitization.PixelDigitizationConfig.SensorSimTool","SensorSimTool")
addTool("PixelDigitization.PixelDigitizationConfig.FrontEndSimTool","FrontEndSimTool")
addTool("PixelDigitization.PixelDigitizationConfig.RD53SimTool","RD53SimTool")
addTool("PixelDigitization.PixelDigitizationConfig.FEI4SimTool","FEI4SimTool")
addTool("PixelDigitization.PixelDigitizationConfig.FEI3SimTool","FEI3SimTool")
addTool("PixelDigitization.PixelDigitizationConfig.PixelOverlayDigitizationTool", "PixelOverlayDigitizationTool")
addAlgorithm("PixelDigitization.PixelDigitizationConfig.PixelOverlayDigitization", "PixelOverlayDigitization")
addTool("PixelDigitization.PixelDigitizationConfig.EnergyDepositionTool", "EnergyDepositionTool")

