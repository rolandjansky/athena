# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
# add tools and algorithms to the ConfiguredFactory, see:
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConfiguredFactory
addTool("SCT_GeoModel.SCT_GeoModelOldConfig.getSCT_DetectorTool","SCT_DetectorTool")
