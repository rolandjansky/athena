# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService

addService("G4AtlasServices.G4AtlasServicesConfig.getDetectorGeometrySvc","DetectorGeometrySvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getG4AtlasSvc","G4AtlasSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getG4GeometryNotifierSvc","G4GeometryNotifierSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getUserActionSvc","UserActionSvc")
