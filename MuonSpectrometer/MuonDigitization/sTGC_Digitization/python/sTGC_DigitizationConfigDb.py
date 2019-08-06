# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool,addAlgorithm
addTool("sTGC_Digitization.sTGC_DigitizationConfig.sTgcDigitizationTool","sTgcDigitizationTool")
addTool("sTGC_Digitization.sTGC_DigitizationConfig.getSTGCRange","sTgcRange")
addTool("sTGC_Digitization.sTGC_DigitizationConfig.STGC_OverlayDigitizationTool", "STGC_OverlayDigitizationTool")
addAlgorithm("sTGC_Digitization.sTGC_DigitizationConfig.getSTGC_OverlayDigitizer", "STGC_OverlayDigitizer")
