# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService

addService("G4AtlasServices.G4AtlasServicesConfig.getReducedStepSizeUserLimitsSvc","ReducedStepSizeUserLimitsSvc")

addService("G4AtlasServices.G4AtlasServicesConfig.getDetectorGeometrySvc","DetectorGeometrySvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getG4AtlasSvc","G4AtlasSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getG4GeometryNotifierSvc","G4GeometryNotifierSvc")

addService("G4AtlasServices.G4AtlasUserActionConfig.getUserActionSvc","G4UA::UserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getISFUserActionSvc","G4UA::ISFUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getISFFullUserActionSvc","G4UA::ISFFullUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getISFPassBackUserActionSvc","G4UA::ISFPassBackUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getISF_AFIIUserActionSvc","G4UA::ISF_AFIIUserActionSvc")
addService("G4AtlasServices.G4AtlasUserActionConfig.getCTBUserActionSvc","G4UA::CTBUserActionSvc")

addService("G4AtlasServices.G4AtlasServicesConfig.getStandardFieldSvc","StandardField")
addService("G4AtlasServices.G4AtlasServicesConfig.getForwardFieldSvc","ForwardField")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ1FwdG4FieldSvc", "Q1FwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ2FwdG4FieldSvc", "Q2FwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ3FwdG4FieldSvc", "Q3FwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getD1FwdG4FieldSvc", "D1FwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getD2FwdG4FieldSvc", "D2FwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ4FwdG4FieldSvc", "Q4FwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ5FwdG4FieldSvc", "Q5FwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ6FwdG4FieldSvc", "Q6FwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ7FwdG4FieldSvc", "Q7FwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ1HKickFwdG4FieldSvc", "Q1HKickFwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ1VKickFwdG4FieldSvc", "Q1VKickFwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ2HKickFwdG4FieldSvc", "Q2HKickFwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ2VKickFwdG4FieldSvc", "Q2VKickFwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ3HKickFwdG4FieldSvc", "Q3HKickFwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ3VKickFwdG4FieldSvc", "Q3VKickFwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ4VKickAFwdG4FieldSvc", "Q4VKickAFwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ4HKickFwdG4FieldSvc", "Q4HKickFwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ4VKickBFwdG4FieldSvc", "Q4VKickBFwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ5HKickFwdG4FieldSvc", "Q5HKickFwdG4FieldSvc")
addService("G4AtlasServices.G4AtlasServicesConfig.getQ6VKickFwdG4FieldSvc", "Q6VKickFwdG4FieldSvc")
