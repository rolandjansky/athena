# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConfiguredFactory

from AthenaCommon.CfgGetter import addTool

addTool("SiLorentzAngleTool.SiLorentzAngleToolConfig.getSCTLorentzAngleTool", "SCTLorentzAngleTool")
addTool("SiLorentzAngleTool.SiLorentzAngleToolConfig.getPixelLorentzAngleTool", "PixelLorentzAngleTool")
