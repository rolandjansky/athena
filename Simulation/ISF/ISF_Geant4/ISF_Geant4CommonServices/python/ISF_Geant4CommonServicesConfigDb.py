# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService
addService("ISF_Geant4CommonServices.ISF_Geant4CommonServicesConfig.getG4PolyconeGeoIDSvc",    "ISF_G4PolyconeGeoIDSvc"   )
addService("ISF_Geant4CommonServices.ISF_Geant4CommonServicesConfig.getAFIIG4PolyconeGeoIDSvc","ISF_AFIIG4PolyconeGeoIDSvc")
#addService("ISF_Geant4CommonServices.ISF_Geant4CommonServicesConfig.getG4PolyconeGeoIDSvc_G4", "ISF_G4PolyconeGeoIDSvc_G4")
