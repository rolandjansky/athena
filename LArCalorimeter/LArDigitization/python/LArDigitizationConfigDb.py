# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm
addTool("LArDigitization.LArDigitizationConfigLegacy.getLArRangeEM"                 , "LArRangeEM")
addTool("LArDigitization.LArDigitizationConfigLegacy.getLArRangeHEC"                , "LArRangeHEC")
addTool("LArDigitization.LArDigitizationConfigLegacy.getLArRangeFCAL"               , "LArRangeFCAL")
addTool("LArDigitization.LArDigitizationConfigLegacy.getLArPileUpTool"              , "LArPileUpTool")
addAlgorithm("LArDigitization.LArDigitizationConfigLegacy.getLArHitEMapMaker"         , "digitmaker1")
addAlgorithm("LArDigitization.LArDigitizationConfigLegacy.getLArHitEMapToDigitAlg"  , "LArHitEMapToDigitAlg")
