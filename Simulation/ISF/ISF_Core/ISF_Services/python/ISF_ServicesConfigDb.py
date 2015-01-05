# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_Services
Elmar Ritsch, 16/12/2014
"""

from AthenaCommon.CfgGetter import addTool, addToolClone, addService, addAlgorithm, \
     addTypesToExcludeIfDefaultValue, addNamesToExcludeIfDefaultValue, addFullNamesToExcludeIfDefaultValue, \
     addPropertiesToExcludeIfDefault, \
     addTypesToSkipIfNotAvailable, addNamesToSkipIfNotAvailable, addFullNamesToSkipIfNotAvailable, \
     addTypesOnlyToSkip

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
import AthenaCommon.SystemOfUnits as Units

# Common tools, services and algorithms used by jobs
addService("ISF_Services.ISF_ServicesConfig.getISFEnvelopeDefSvc"           , "ISF_ISFEnvelopeDefSvc"               )
addService("ISF_Services.ISF_ServicesConfig.getAFIIEnvelopeDefSvc"          , "ISF_AFIIEnvelopeDefSvc"              )
addService("ISF_Services.ISF_ServicesConfig.getGeoIDSvc"                    , "ISF_GeoIDSvc"                        )
addService("ISF_Services.ISF_ServicesConfig.getAFIIGeoIDSvc"                , "ISF_AFIIGeoIDSvc"                    )
addService("ISF_Services.ISF_ServicesConfig.getParticleBrokerSvc"           , "ISF_ParticleBrokerSvc"               )
addService("ISF_Services.ISF_ServicesConfig.getLongLivedParticleBrokerSvc"  , "ISF_LongLivedParticleBrokerSvc"      )
addService("ISF_Services.ISF_ServicesConfig.getParticleBrokerSvcNoOrdering" , "ISF_ParticleBrokerSvcNoOrdering"     )
addService("ISF_Services.ISF_ServicesConfig.getAFIIParticleBrokerSvc"       , "ISF_AFIIParticleBrokerSvc"           )
addService("ISF_Services.ISF_ServicesConfig.getSimHitService"               , "ISF_SimHitService"                   )
