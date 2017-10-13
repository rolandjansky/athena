# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("FastSiDigitization.FastSiDigitizationConfig.PixelFastDigitizationTool"              , "PixelFastDigitizationTool")
addTool("FastSiDigitization.FastSiDigitizationConfig.PixelFastDigitizationToolHS"            , "PixelFastDigitizationToolHS")
addTool("FastSiDigitization.FastSiDigitizationConfig.PixelFastDigitizationToolPU"            , "PixelFastDigitizationToolPU")
addTool("FastSiDigitization.FastSiDigitizationConfig.PixelFastDigitizationToolSplitNoMergePU", "PixelFastDigitizationToolSplitNoMergePU")
addTool("FastSiDigitization.FastSiDigitizationConfig.getFastPixelRange"                      , "FastPixelRange")

addTool("FastSiDigitization.FastSiDigitizationConfig.FastClusterMakerTool"                   , "FastClusterMakerTool")
addTool("FastSiDigitization.FastSiDigitizationConfig.SCT_FastDigitizationTool"               , "SCT_FastDigitizationTool")
addTool("FastSiDigitization.FastSiDigitizationConfig.SCT_FastDigitizationToolHS"             , "SCT_FastDigitizationToolHS")
addTool("FastSiDigitization.FastSiDigitizationConfig.SCT_FastDigitizationToolPU"             , "SCT_FastDigitizationToolPU")
addTool("FastSiDigitization.FastSiDigitizationConfig.SCT_FastDigitizationToolSplitNoMergePU" , "SCT_FastDigitizationToolSplitNoMergePU")
addTool("FastSiDigitization.FastSiDigitizationConfig.getFastSCTRange"                        , "FastSCTRange" )
