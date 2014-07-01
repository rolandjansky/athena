# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# by default only export the getter functions
__all__ = [ "getPublicTool", "getPublicToolClone", "getPrivateTool", "getPrivateToolClone",
            "getService", "getServiceClone", "getAlgorithm", "getAlgorithmClone" ]

from ConfiguredFactory import ConfiguredFactory

# (private) storage of tools and services
_configFactory = ConfiguredFactory( propertiesToExcludeIfDefaultValue = [ "EvtStore", "DetStore", "decSvc", "AcceptAlgs", "RequireAlgs", "VetoAlgs" ] )


# forward public user access functions
getPublicTool         = _configFactory.getPublicTool
getPublicToolClone    = _configFactory.getPublicToolClone
getPrivateTool        = _configFactory.getPrivateTool
getPrivateToolClone   = _configFactory.getPrivateToolClone
getService            = _configFactory.getService
getServiceClone       = _configFactory.getServiceClone
getAlgorithm          = _configFactory.getAlgorithm
getAlgorithmClone     = _configFactory.getAlgorithmClone

addTool               = _configFactory.addTool
addToolClone          = _configFactory.addToolClone
addService            = _configFactory.addService
addServiceClone       = _configFactory.addServiceClone
addAlgorithm          = _configFactory.addAlgorithm
addAlgorithmClone     = _configFactory.addAlgorithmClone

addTypesToExcludeIfDefaultValue     = _configFactory.addTypesToExcludeIfDefaultValue
addNamesToExcludeIfDefaultValue     = _configFactory.addNamesToExcludeIfDefaultValue
addFullNamesToExcludeIfDefaultValue = _configFactory.addFullNamesToExcludeIfDefaultValue
addPropertiesToExcludeIfDefault     = _configFactory.addPropertiesToExcludeIfDefault
addTypesToSkipIfNotAvailable        = _configFactory.addTypesToSkipIfNotAvailable
addNamesToSkipIfNotAvailable        = _configFactory.addNamesToSkipIfNotAvailable
addFullNamesToSkipIfNotAvailable    = _configFactory.addFullNamesToSkipIfNotAvailable
addTypesOnlyToSkip                  = _configFactory.addTypesOnlyToSkip

setLogLevel = _configFactory.setLogLevel
printStats  = _configFactory.printStats
