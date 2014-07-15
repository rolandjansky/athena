# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# TileCal GeoModel initialization
#
from AthenaCommon.DetFlags      import DetFlags

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()

if ( DetFlags.detdescr.Tile_on() ):   
   from TileGeoModel.TileGeoModelConf import TileDetectorTool
   GeoModelSvc.DetectorTools += [ TileDetectorTool() ]
   GeoModelSvc.DetectorTools[ "TileDetectorTool" ].TestBeam = True
   

