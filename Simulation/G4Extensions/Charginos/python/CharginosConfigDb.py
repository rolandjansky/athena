# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("Charginos.CharginosConfigLegacy.getCharginosPhysicsTool"         , "CharginosPhysicsTool" )
addTool("Charginos.CharginosConfigLegacy.getCharginoPlusToPiPlusNeutralino", "CharginoPlusToPiPlusNeutralino")
addTool("Charginos.CharginosConfigLegacy.getCharginoMinusToPiMinusNeutralino", "CharginoMinusToPiMinusNeutralino")
addTool("Charginos.CharginosConfigLegacy.getCharginoPlusToEPlusNeutralino", "CharginoPlusToEPlusNeutralino")
addTool("Charginos.CharginosConfigLegacy.getCharginoMinusToEMinusNeutralino", "CharginoMinusToEMinusNeutralino")
addTool("Charginos.CharginosConfigLegacy.getCharginoPlusToMuPlusNeutralino", "CharginoPlusToMuPlusNeutralino")
addTool("Charginos.CharginosConfigLegacy.getCharginoMinusToMuMinusNeutralino", "CharginoMinusToMuMinusNeutralino")
