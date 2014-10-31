# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_Geant4CommonTools
Elmar Ritsch, 31/10/2014
"""

from AthenaCommon.CfgGetter import addTool, addToolClone, addService, addAlgorithm, \
     addTypesToExcludeIfDefaultValue, addNamesToExcludeIfDefaultValue, addFullNamesToExcludeIfDefaultValue, \
     addPropertiesToExcludeIfDefault, \
     addTypesToSkipIfNotAvailable, addNamesToSkipIfNotAvailable, addFullNamesToSkipIfNotAvailable, \
     addTypesOnlyToSkip

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
import AthenaCommon.SystemOfUnits as Units

# Common tools, services and algorithms used by jobs
addTool("ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfig.getEntryLayerTool",      "ISF_EntryLayerTool")
addTool("ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfig.getAFIIEntryLayerTool",  "ISF_AFIIEntryLayerTool")
