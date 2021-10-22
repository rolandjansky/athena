# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool,addService,addAlgorithm
addTool("HGTD_Digitization.HGTD_DigitizationConfig.getHGTD_DigitizationTool", "HGTD_DigitizationTool")
## FIXME HGTD_Amp needs to be added at later point
# addTool("HGTD_Digitization.HGTD_DigitizationConfig.getHGTD_Amp", "HGTD_Amp" )
addTool("HGTD_Digitization.HGTD_DigitizationConfig.getHGTD_FrontEndTool", "HGTD_FrontEndTool" )
addTool("HGTD_Digitization.HGTD_DigitizationConfig.getHGTD_SurfaceChargesGenerator", "HGTD_SurfaceChargesGenerator" )
