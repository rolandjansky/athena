# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService
addService("ISF_Geant4CommonServices.ISF_Geant4CommonServicesConfigLegacy.getG4PolyconeGeoIDSvc",    "ISF_G4PolyconeGeoIDSvc"   )
addService("ISF_Geant4CommonServices.ISF_Geant4CommonServicesConfigLegacy.getAFIIG4PolyconeGeoIDSvc","ISF_AFIIG4PolyconeGeoIDSvc")
