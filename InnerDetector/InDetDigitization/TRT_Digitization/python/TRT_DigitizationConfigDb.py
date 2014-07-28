# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm
addTool("TRT_Digitization.TRT_DigitizationConfig.getTRTRange"            , "TRTRange")
addTool("TRT_Digitization.TRT_DigitizationConfig.TRTDigitizationTool"    , "TRTDigitizationTool")
addTool("TRT_Digitization.TRT_DigitizationConfig.TRT_SimDriftTimeTool"   , "TRT_SimDriftTimeTool")
addTool("TRT_Digitization.TRT_DigitizationConfig.TRTDigitizationToolHS"  , "TRTDigitizationToolHS")
addTool("TRT_Digitization.TRT_DigitizationConfig.TRTDigitizationToolPU"  , "TRTDigitizationToolPU")
addAlgorithm("TRT_Digitization.TRT_DigitizationConfig.TRTDigitizationHS" , "TRTDigitizationHS")
addAlgorithm("TRT_Digitization.TRT_DigitizationConfig.TRTDigitizationPU" , "TRTDigitizationPU")
