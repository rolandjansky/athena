# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm
addTool("TRT_Digitization.TRT_DigitizationConfigLegacy.getTRTRange"            , "TRTRange")
addTool("TRT_Digitization.TRT_DigitizationConfigLegacy.TRTDigitizationTool"    , "TRTDigitizationTool")
addTool("TRT_Digitization.TRT_DigitizationConfigLegacy.TRTGeantinoTruthDigitizationTool"    , "TRTGeantinoTruthDigitizationTool")
addTool("TRT_Digitization.TRT_DigitizationConfigLegacy.TRTDigitizationToolHS"  , "TRTDigitizationToolHS")
addTool("TRT_Digitization.TRT_DigitizationConfigLegacy.TRTDigitizationToolPU"  , "TRTDigitizationToolPU")
addTool("TRT_Digitization.TRT_DigitizationConfigLegacy.TRTDigitizationToolSplitNoMergePU", "TRTDigitizationToolSplitNoMergePU")
addTool("TRT_Digitization.TRT_DigitizationConfigLegacy.TRT_OverlayDigitizationTool", "TRT_OverlayDigitizationTool")
addAlgorithm("TRT_Digitization.TRT_DigitizationConfigLegacy.TRTDigitizationHS" , "TRTDigitizationHS")
addAlgorithm("TRT_Digitization.TRT_DigitizationConfigLegacy.TRTDigitizationPU" , "TRTDigitizationPU")
addAlgorithm("TRT_Digitization.TRT_DigitizationConfigLegacy.TRT_OverlayDigitization", "TRT_OverlayDigitization")
