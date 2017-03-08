# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool,addService,addAlgorithm

addTool("PixelDigitization.PixelDigitizationConfig.PixelDigitizationTool", "PixelDigitizationTool")
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
addTool("PixelDigitization.PixelDigitizationConfig.DBMChargeTool","DBMChargeTool")
addTool("PixelDigitization.PixelDigitizationConfig.PixelPlanarChargeTool","PixelPlanarChargeTool")
addTool("PixelDigitization.PixelDigitizationConfig.Pixel3DChargeTool","Pixel3DChargeTool")
addTool("PixelDigitization.PixelDigitizationConfig.SubChargesTool","SubChargesTool")
addTool("PixelDigitization.PixelDigitizationConfig.FrontEndSimTool","FrontEndSimTool")
addTool("PixelDigitization.PixelDigitizationConfig.FEI4SimTool","FEI4SimTool")
addTool("PixelDigitization.PixelDigitizationConfig.FEI3SimTool","FEI3SimTool")
addService("PixelDigitization.PixelDigitizationConfig.TimeSvc","TimeSvc")
addTool("PixelDigitization.PixelDigitizationConfig.PixelOverlayDigitizationTool", "PixelOverlayDigitizationTool")
addAlgorithm("PixelDigitization.PixelDigitizationConfig.PixelOverlayDigitization", "PixelOverlayDigitization")
addTool("PixelDigitization.PixelDigitizationConfig.BichselSimTool", "BichselSimTool")

