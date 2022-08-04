# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_Geant4CommonTools
Elmar Ritsch, 31/10/2014
"""

from AthenaCommon.CfgGetter import addTool

# Common tools, services and algorithms used by jobs
addTool("ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfigLegacy.getEntryLayerToolMT",    "ISF_EntryLayerToolMT")
addTool("ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfigLegacy.getEntryLayerTool",      "ISF_EntryLayerTool")
addTool("ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfigLegacy.getAFIIEntryLayerTool",  "ISF_AFIIEntryLayerTool")
addTool("ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfigLegacy.getAFIIEntryLayerToolMT",  "ISF_AFIIEntryLayerToolMT")
