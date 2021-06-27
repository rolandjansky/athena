# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool,addService,addAlgorithm

addTool("HGTD_FastDigitization.HGTD_SmearedDigitizationConfig.HGTD_SmearedDigitizationTool", "HGTD_SmearedDigitizationTool")
addTool("HGTD_FastDigitization.HGTD_SmearedDigitizationConfig.getSmearedHGTD_Range", "SmearedHGTDRange")
