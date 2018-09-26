# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService
addService("ISF_Geant4Services.ISF_Geant4ServicesConfig.getGeant4SimSvc",          "ISF_Geant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfig.getFullGeant4SimSvc",      "ISF_FullGeant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfig.getPassBackGeant4SimSvc",  "ISF_PassBackGeant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfig.getAFIIGeant4SimSvc",      "ISF_AFIIGeant4SimSvc")
addService("ISF_Geant4Services.ISF_Geant4ServicesConfig.getLongLivedGeant4SimSvc", "ISF_LongLivedGeant4SimSvc")

addTool("ISF_Geant4Services.ISF_Geant4ServicesConfig.getGeant4Tool",               "ISF_Geant4Tool")
addTool("ISF_Geant4Services.ISF_Geant4ServicesConfig.getFullGeant4Tool",           "ISF_FullGeant4Tool")
addTool("ISF_Geant4Services.ISF_Geant4ServicesConfig.getLongLivedGeant4Tool",      "ISF_LongLivedGeant4Tool")
addTool("ISF_Geant4Services.ISF_Geant4ServicesConfig.getPassBackGeant4Tool",       "ISF_PassBackGeant4Tool")
addTool("ISF_Geant4Services.ISF_Geant4ServicesConfig.getAFIIGeant4Tool",           "ISF_AFIIGeant4Tool")

#addTool("ISF_Geant4Services.ISF_Geant4ServicesConfig.getGeant4MTTool",             "ISF_Geant4MTTool")
#addTool("ISF_Geant4Services.ISF_Geant4ServicesConfig.getFullGeant4MTTool",         "ISF_FullGeant4MTTool")
#addTool("ISF_Geant4Services.ISF_Geant4ServicesConfig.getLongLivedGeant4MTTool",    "ISF_LongLivedGeant4MTTool")
#addTool("ISF_Geant4Services.ISF_Geant4ServicesConfig.getPassBackGeant4MTTool",     "ISF_PassBackGeant4MTTool")
#addTool("ISF_Geant4Services.ISF_Geant4ServicesConfig.getAFIIGeant4MTTool",         "ISF_AFIIGeant4MTTool")
