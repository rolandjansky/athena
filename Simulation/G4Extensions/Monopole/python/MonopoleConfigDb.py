# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("Monopole.MonopoleConfigLegacy.getMonopolePhysicsTool"         , "MonopolePhysicsTool" )
addTool("Monopole.MonopoleConfigLegacy.getG4mplEqMagElectricFieldTool", "G4mplEqMagElectricField")
