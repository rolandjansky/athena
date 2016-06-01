# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("Monopole.MonopoleConfig.getMonopolePhysicsTool"         , "MonopolePhysicsTool" )
addTool("Monopole.MonopoleConfig.getG4mplEqMagElectricFieldTool", "G4mplEqMagElectricField")
