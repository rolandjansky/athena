# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Configure GeoModel tools to build minimal geometry fo reconstruction
#

from AthenaCommon.DetFlags      import DetFlags

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()

if ( DetFlags.detdescr.LAr_on() ):
    GeoModelSvc.DetectorTools["LArDetectorToolNV"].GeometryConfig = "RECO"

if ( DetFlags.detdescr.Tile_on() ):
    GeoModelSvc.DetectorTools["TileDetectorTool"].GeometryConfig = "RECO"

#
# Build Muon dead materials using AGDD2Geo
#

from AtlasGeoModel import Agdd2Geo
