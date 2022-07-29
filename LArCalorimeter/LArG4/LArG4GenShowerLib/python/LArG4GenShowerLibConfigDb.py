# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm, addTool

addTool("LArG4GenShowerLib.LArG4GenShowerLibConfigLegacy.getTestActionShowerLibTool", "G4UA::TestActionShowerLibTool")
addAlgorithm("LArG4GenShowerLib.LArG4GenShowerLibConfigLegacy.getLArG4GenShowerLib", "LArG4GenShowerLib")
