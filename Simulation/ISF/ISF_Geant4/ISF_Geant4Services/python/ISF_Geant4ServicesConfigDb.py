# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService
addService("ISF_Geant4Services.ISF_Geant4ServicesConfig.getGeant4SimSvc",          "ISF_Geant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfig.getFullGeant4SimSvc",      "ISF_FullGeant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfig.getPassBackGeant4SimSvc",  "ISF_PassBackGeant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfig.getAFIIGeant4SimSvc",        "ISF_AFIIGeant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfig.getLongLivedGeant4SimSvc",   "ISF_LongLivedGeant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfig.getAFII_QS_Geant4SimSvc",    "ISF_AFII_QS_Geant4SimSvc")
