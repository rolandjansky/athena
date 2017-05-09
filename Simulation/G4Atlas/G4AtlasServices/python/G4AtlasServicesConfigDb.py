# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService

addService("G4AtlasServices.G4AtlasServicesConfig.getDetectorGeometrySvc","DetectorGeometrySvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getG4AtlasSvc","G4AtlasSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getG4GeometryNotifierSvc","G4GeometryNotifierSvc")

addService("G4AtlasServices.G4AtlasUserActionConfig.getUserActionSvc","G4UA::UserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getISFUserActionSvc","G4UA::ISFUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getISFFullUserActionSvc","G4UA::ISFFullUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getISFPassBackUserActionSvc","G4UA::ISFPassBackUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getISF_AFIIUserActionSvc","G4UA::ISF_AFIIUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getISFQuasiStableUserActionSvc","G4UA::ISFQuasiStableUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getCTBUserActionSvc","G4UA::CTBUserActionSvc")

addService("G4AtlasServices.G4AtlasServicesConfig.getStandardFieldSvc","StandardField")
addService("G4AtlasServices.G4AtlasServicesConfig.getForwardFieldSvc","ForwardField")
