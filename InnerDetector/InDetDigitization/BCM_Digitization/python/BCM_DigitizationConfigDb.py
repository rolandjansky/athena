# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm
addTool("BCM_Digitization.BCM_DigitizationConfigLegacy.BCM_DigitizationTool", "BCM_DigitizationTool")
addTool("BCM_Digitization.BCM_DigitizationConfigLegacy.getBCM_Range",         "BCM_Range" )
addTool("BCM_Digitization.BCM_DigitizationConfigLegacy.BCM_OverlayDigitizationTool", "BCM_OverlayDigitizationTool")
addAlgorithm("BCM_Digitization.BCM_DigitizationConfigLegacy.BCM_OverlayDigitization", "BCM_OverlayDigitization")
