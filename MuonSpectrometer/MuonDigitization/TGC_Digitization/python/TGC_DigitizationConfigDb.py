# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("TGC_Digitization.TGC_DigitizationConfig.TgcDigitizationTool" , "TgcDigitizationTool") 
addTool("TGC_Digitization.TGC_DigitizationConfig.getTgcRange"         , "TgcRange")
addTool("TGC_Digitization.TGC_DigitizationConfig.Tgc_OverlayDigitizationTool" , "Tgc_OverlayDigitizationTool") 
