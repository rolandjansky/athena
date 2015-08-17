# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("FastTRT_Digitization.FastTRT_DigitizationConfig.getFastTRTRange"                           , "FastTRTRange")
addTool("FastTRT_Digitization.FastTRT_DigitizationConfig.TRTFastDigitizationTool"                   , "TRTFastDigitizationTool")
addTool("FastTRT_Digitization.FastTRT_DigitizationConfig.TRTFastDigitizationToolHS"             , "TRTFastDigitizationToolHS")
addTool("FastTRT_Digitization.FastTRT_DigitizationConfig.TRTFastDigitizationToolPU"             , "TRTFastDigitizationToolPU")
addTool("FastTRT_Digitization.FastTRT_DigitizationConfig.TRTFastDigitizationToolSplitNoMergePU" , "TRTFastDigitizationToolSplitNoMergePU")
