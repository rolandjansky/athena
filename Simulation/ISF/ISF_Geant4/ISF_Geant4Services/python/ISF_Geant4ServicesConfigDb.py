# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService
addService("ISF_Geant4Services.ISF_Geant4ServicesConfigLegacy.getGeant4SimSvc",          "ISF_Geant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfigLegacy.getFullGeant4SimSvc",      "ISF_FullGeant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfigLegacy.getPassBackGeant4SimSvc",  "ISF_PassBackGeant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfigLegacy.getAFIIGeant4SimSvc",      "ISF_AFIIGeant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfigLegacy.getLongLivedGeant4SimSvc", "ISF_LongLivedGeant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfigLegacy.getAFII_QS_Geant4SimSvc",  "ISF_AFII_QS_Geant4SimSvc")
