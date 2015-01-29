# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService
addService("G4AtlasServices.G4AtlasServicesConfig.getSensitiveDetectorSvc","SensitiveDetectorSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getFastSimulationSvc","FastSimulationSvc")
