# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addService, addAlgorithm
addService("ISF_HepMC_Services.ISF_HepMC_ServicesConfig.getTruthService"              , "ISF_TruthService")
addService("ISF_HepMC_Services.ISF_HepMC_ServicesConfig.getMC12TruthService"          , "ISF_MC12TruthService")
addService("ISF_HepMC_Services.ISF_HepMC_ServicesConfig.getMC15TruthService"          , "ISF_MC15TruthService")
addService("ISF_HepMC_Services.ISF_HepMC_ServicesConfig.getValidationTruthService"    , "ISF_ValidationTruthService")
