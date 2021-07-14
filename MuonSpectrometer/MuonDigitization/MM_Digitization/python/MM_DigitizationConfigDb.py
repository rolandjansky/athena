# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool,addAlgorithm
addTool("MM_Digitization.MM_DigitizationConfig.MM_DigitizationTool","MM_DigitizationTool")
addTool("MM_Digitization.MM_DigitizationConfig.getMMRange", "MMRange")
addTool("MM_Digitization.MM_DigitizationConfig.MM_OverlayDigitizationTool", "MM_OverlayDigitizationTool")
addAlgorithm("MM_Digitization.MM_DigitizationConfig.getMM_OverlayDigitizer", "MM_OverlayDigitizer")
