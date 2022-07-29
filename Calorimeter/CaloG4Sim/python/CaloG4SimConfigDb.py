# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("CaloG4Sim.CaloG4SimConfigLegacy.getCalibrationDefaultProcessingTool", "G4UA::CaloG4::CalibrationDefaultProcessingTool")
