# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService
addService("RngComps.RngCompsConfig.getAthRNGSvc", "AthRNGSvc")
addService("RngComps.RngCompsConfig.getAthMixMaxRNGSvc", "AthMixMaxRNGSvc")
