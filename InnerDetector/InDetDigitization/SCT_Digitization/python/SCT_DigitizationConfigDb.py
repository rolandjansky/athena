# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm
addTool("SCT_Digitization.SCT_DigitizationConfig.SCT_DigitizationTool"          , "SCT_DigitizationTool")
addTool("SCT_Digitization.SCT_DigitizationConfig.SCT_GeantinoTruthDigitizationTool"          , "SCT_GeantinoTruthDigitizationTool")
addTool("SCT_Digitization.SCT_DigitizationConfig.SCT_DigitizationToolHS"        , "SCT_DigitizationToolHS")
addTool("SCT_Digitization.SCT_DigitizationConfig.SCT_DigitizationToolPU"        , "SCT_DigitizationToolPU")
addTool("SCT_Digitization.SCT_DigitizationConfig.SCT_DigitizationToolSplitNoMergePU", "SCT_DigitizationToolSplitNoMergePU")
addAlgorithm("SCT_Digitization.SCT_DigitizationConfig.SCT_DigitizationHS"       , "SCT_DigitizationHS")
addAlgorithm("SCT_Digitization.SCT_DigitizationConfig.SCT_DigitizationPU"       , "SCT_DigitizationPU")
addTool("SCT_Digitization.SCT_DigitizationConfig.getSiliconRange"               , "SiliconRange" )
addTool("SCT_Digitization.SCT_DigitizationConfig.getSCT_RandomDisabledCellGenerator", "SCT_RandomDisabledCellGenerator")
addTool("SCT_Digitization.SCT_DigitizationConfig.getSCT_Amp", "SCT_Amp" )
addTool("SCT_Digitization.SCT_DigitizationConfig.getSCT_FrontEnd"               , "SCT_FrontEnd" )
addTool("SCT_Digitization.SCT_DigitizationConfig.getPileupSCT_FrontEnd"         , "PileupSCT_FrontEnd" )
addTool("SCT_Digitization.SCT_DigitizationConfig.getSCT_SurfaceChargesGenerator", "SCT_SurfaceChargesGenerator" )
addTool("SCT_Digitization.SCT_DigitizationConfig.SCT_OverlayDigitizationTool", "SCT_OverlayDigitizationTool")
addAlgorithm("SCT_Digitization.SCT_DigitizationConfig.SCT_OverlayDigitization", "SCT_OverlayDigitization")
