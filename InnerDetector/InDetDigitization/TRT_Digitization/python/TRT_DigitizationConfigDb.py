# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm
addTool("TRT_Digitization.TRT_DigitizationConfig.getTRTRange"            , "TRTRange")
addTool("TRT_Digitization.TRT_DigitizationConfig.TRTDigitizationTool"    , "TRTDigitizationTool")
addTool("TRT_Digitization.TRT_DigitizationConfig.TRTGeantinoTruthDigitizationTool"    , "TRTGeantinoTruthDigitizationTool")
addTool("TRT_Digitization.TRT_DigitizationConfig.TRTDigitizationToolHS"  , "TRTDigitizationToolHS")
addTool("TRT_Digitization.TRT_DigitizationConfig.TRTDigitizationToolPU"  , "TRTDigitizationToolPU")
addTool("TRT_Digitization.TRT_DigitizationConfig.TRTDigitizationToolSplitNoMergePU", "TRTDigitizationToolSplitNoMergePU")
addTool("TRT_Digitization.TRT_DigitizationConfig.TRT_OverlayDigitizationTool", "TRT_OverlayDigitizationTool")
addAlgorithm("TRT_Digitization.TRT_DigitizationConfig.TRTDigitizationHS" , "TRTDigitizationHS")
addAlgorithm("TRT_Digitization.TRT_DigitizationConfig.TRTDigitizationPU" , "TRTDigitizationPU")
addAlgorithm("TRT_Digitization.TRT_DigitizationConfig.TRT_OverlayDigitization", "TRT_OverlayDigitization")
