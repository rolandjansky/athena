# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool,addService,addAlgorithm
addTool("SCT_Digitization.SCT_DigitizationConfig.SCT_DigitizationTool"          , "SCT_DigitizationTool")
addTool("SCT_Digitization.SCT_DigitizationConfig.SCT_DigitizationToolHS"        , "SCT_DigitizationToolHS")
addTool("SCT_Digitization.SCT_DigitizationConfig.SCT_DigitizationToolPU"        , "SCT_DigitizationToolPU")
addAlgorithm("SCT_Digitization.SCT_DigitizationConfig.SCT_DigitizationHS"       , "SCT_DigitizationHS")
addAlgorithm("SCT_Digitization.SCT_DigitizationConfig.SCT_DigitizationPU"       , "SCT_DigitizationPU")
addTool("SCT_Digitization.SCT_DigitizationConfig.getSiliconRange"               , "SiliconRange" )
addTool("SCT_Digitization.SCT_DigitizationConfig.getSCT_FrontEnd"               , "SCT_FrontEnd" )
addTool("SCT_Digitization.SCT_DigitizationConfig.getSCT_SurfaceChargesGenerator", "SCT_SurfaceChargesGenerator" )
