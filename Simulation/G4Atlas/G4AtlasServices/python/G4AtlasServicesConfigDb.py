# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService

addService("G4AtlasServices.G4AtlasServicesConfig.getDetectorGeometrySvc","DetectorGeometrySvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getG4AtlasSvc","G4AtlasSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getG4GeometryNotifierSvc","G4GeometryNotifierSvc")

addService("G4AtlasServices.G4AtlasUserActionConfig.getV1UserActionSvc","UserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getV1ISFUserActionSvc","ISFUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getV1ISFFullUserActionSvc","ISFFullUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getV1ISFPassBackUserActionSvc","ISFPassBackUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getV1ISF_AFIIUserActionSvc","ISF_AFIIUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getV1ISFQuasiStableUserActionSvc","ISFQuasiStableUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getV1CTBUserActionSvc","CTBUserActionSvc")

addService("G4AtlasServices.G4AtlasUserActionConfig.getV2UserActionSvc","G4UA::UserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getV2ISFUserActionSvc","G4UA::ISFUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getV2ISFFullUserActionSvc","G4UA::ISFFullUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getV2ISFPassBackUserActionSvc","G4UA::ISFPassBackUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getV2ISF_AFIIUserActionSvc","G4UA::ISF_AFIIUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getV2ISFQuasiStableUserActionSvc","G4UA::ISFQuasiStableUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getV2CTBUserActionSvc","G4UA::CTBUserActionSvc")

addService("G4AtlasServices.G4AtlasServicesConfig.getStandardFieldSvc","StandardField")
addService("G4AtlasServices.G4AtlasServicesConfig.getForwardFieldSvc","ForwardField")
