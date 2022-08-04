# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm

addTool("PixelDigitization.PixelDigitizationConfigLegacy.PixelDigitizationTool", "PixelDigitizationTool")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.PixelGeantinoTruthDigitizationTool", "PixelGeantinoTruthDigitizationTool")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.PixelDigitizationToolHS", "PixelDigitizationToolHS")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.PixelDigitizationToolPU", "PixelDigitizationToolPU")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.PixelDigitizationToolSplitNoMergePU", "PixelDigitizationToolSplitNoMergePU")

addAlgorithm("PixelDigitization.PixelDigitizationConfigLegacy.PixelDigitizationHS"  , "PixelDigitizationHS")
addAlgorithm("PixelDigitization.PixelDigitizationConfigLegacy.PixelDigitizationPU"  , "PixelDigitizationPU")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.getPixelRange", "PixelRange")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.SensorSimPlanarTool","SensorSimPlanarTool")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.SensorSim3DTool","SensorSim3DTool")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.SensorSimTool","SensorSimTool")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.FrontEndSimTool","FrontEndSimTool")

addTool("PixelDigitization.PixelDigitizationConfigLegacy.BarrelFEI4SimTool","BarrelFEI4SimTool")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.DBMFEI4SimTool","DBMFEI4SimTool")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.BarrelFEI3SimTool","BarrelFEI3SimTool")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.EndcapFEI3SimTool","EndcapFEI3SimTool")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.BarrelRD53SimTool","BarrelRD53SimTool")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.EndcapRD53SimTool","EndcapRD53SimTool")

addTool("PixelDigitization.PixelDigitizationConfigLegacy.PixelOverlayDigitizationTool", "PixelOverlayDigitizationTool")
addAlgorithm("PixelDigitization.PixelDigitizationConfigLegacy.PixelOverlayDigitization", "PixelOverlayDigitization")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.EnergyDepositionTool", "EnergyDepositionTool")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.RadDamageUtil", "RadDamageUtil")
addTool("PixelDigitization.PixelDigitizationConfigLegacy.EfieldInterpolator", "EfieldInterpolator")

