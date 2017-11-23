# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("Charginos.CharginosConfig.getCharginosPhysicsTool"         , "CharginosPhysicsTool" )
addTool("Charginos.CharginosConfig.getCharginoPlusToPiPlusNeutralino", "CharginoPlusToPiPlusNeutralino")
addTool("Charginos.CharginosConfig.getCharginoMinusToPiMinusNeutralino", "CharginoMinusToPiMinusNeutralino")
addTool("Charginos.CharginosConfig.getCharginoPlusToEPlusNeutralino", "CharginoPlusToEPlusNeutralino")
addTool("Charginos.CharginosConfig.getCharginoMinusToEMinusNeutralino", "CharginoMinusToEMinusNeutralino")
addTool("Charginos.CharginosConfig.getCharginoPlusToMuPlusNeutralino", "CharginoPlusToMuPlusNeutralino")
addTool("Charginos.CharginosConfig.getCharginoMinusToMuMinusNeutralino", "CharginoMinusToMuMinusNeutralino")
