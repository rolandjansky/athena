# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CfgGetter import addService, addTool

addService("ActsGeometry.ActsGeometryConfigLegacy.getActsTrackingGeometrySvc",  "ActsTrackingGeometrySvc")
addTool("ActsGeometry.ActsGeometryConfigLegacy.getActsTrackingGeometryTool", "ActsTrackingGeometryTool")
addTool("ActsGeometry.ActsGeometryConfigLegacy.getActsExtrapolationTool", "ActsExtrapolationTool")
