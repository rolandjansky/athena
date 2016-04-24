# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("MDT_Digitization.MdtDigitizationConfig.MdtDigitizationTool"     , "MdtDigitizationTool")
addTool("MDT_Digitization.MdtDigitizationConfig.RT_Relation_DB_DigiTool" , "RT_Relation_DB_DigiTool")
addTool("MDT_Digitization.MdtDigitizationConfig.MDT_Response_DigiTool"   , "MDT_Response_DigiTool")
addTool("MDT_Digitization.MdtDigitizationConfig.getMdtRange"             , "MdtRange")
addTool("MDT_Digitization.MdtDigitizationConfig.Mdt_OverlayDigitizationTool", "Mdt_OverlayDigitizationTool")


