# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm
addTool("LArDigitization.LArDigitizationConfig.getLArRangeEM"                , "LArRangeEM")
addTool("LArDigitization.LArDigitizationConfig.getLArRangeHEC"               , "LArRangeHEC")
addTool("LArDigitization.LArDigitizationConfig.getLArRangeFCAL"              , "LArRangeFCAL")
addTool("LArDigitization.LArDigitizationConfig.getLArPileUpTool"             , "LArPileUpTool")
addAlgorithm("LArDigitization.LArDigitizationConfig.getLArHitEMapToDigitAlg" , "LArHitEMapToDigitAlg")
addAlgorithm("LArDigitization.LArDigitizationConfig.getLArDigitMaker"        , "digitmaker1")
