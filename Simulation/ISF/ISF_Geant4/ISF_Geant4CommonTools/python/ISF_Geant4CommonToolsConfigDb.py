# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_Geant4CommonTools
Elmar Ritsch, 31/10/2014
"""

from AthenaCommon.CfgGetter import addTool

# Common tools, services and algorithms used by jobs
addTool("ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfig.getEntryLayerToolMT",    "ISF_EntryLayerToolMT")
addTool("ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfig.getEntryLayerTool",      "ISF_EntryLayerTool")
addTool("ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfig.getAFIIEntryLayerTool",  "ISF_AFIIEntryLayerTool")
addTool("ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfig.getAFIIEntryLayerToolMT",  "ISF_AFIIEntryLayerToolMT")
