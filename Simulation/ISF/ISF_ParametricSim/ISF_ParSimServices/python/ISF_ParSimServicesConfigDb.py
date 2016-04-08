# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF
Miha Muskinja, 14/01/2015
"""

from AthenaCommon.CfgGetter import addTool, addToolClone, addService, addAlgorithm, \
     addTypesToExcludeIfDefaultValue, addNamesToExcludeIfDefaultValue, addFullNamesToExcludeIfDefaultValue, \
     addPropertiesToExcludeIfDefault, \
     addTypesToSkipIfNotAvailable, addNamesToSkipIfNotAvailable, addFullNamesToSkipIfNotAvailable, \
     addTypesOnlyToSkip

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
import AthenaCommon.SystemOfUnits as Units


addService("ISF_ParSimServices.ISF_ParSimServicesConfig.getParametricSimServiceID"  ,   "ISF_ParametricSimSvc"     )
addService("ISF_ParSimServices.ISF_ParSimServicesConfig.getTrackParticleSmearer"    ,   "ISF_TrackParticleSmearer" )
addService("ISF_ParSimServices.ISF_ParSimServicesConfig.getISPtoPerigeeTool"	    ,   "ISF_ISPtoPerigeeTool"	   )
addService("ISF_ParSimServices.ISF_ParSimServicesConfig.getDefaultSmearer"          ,   "ISF_DefaultSmearer"       )
addService("ISF_ParSimServices.ISF_ParSimServicesConfig.getMuonSmearer"             ,   "ISF_MuonSmearer"          )
addService("ISF_ParSimServices.ISF_ParSimServicesConfig.getElectronSmearer"         ,   "ISF_ElectronSmearer"      )
addService("ISF_ParSimServices.ISF_ParSimServicesConfig.getPionSmearer"             ,   "ISF_PionSmearer"          )