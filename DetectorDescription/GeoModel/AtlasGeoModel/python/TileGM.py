# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# TileCal GeoModel initialization
#

from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.DetFlags      import DetFlags

if ( DetFlags.detdescr.Tile_on() ):
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    GeoModelSvc = GeoModelSvc()

    from TileGeoModel.TileGeoModelConf import TileDetectorTool
    GeoModelSvc.DetectorTools += [ TileDetectorTool() ]

    if ( jobproperties.Global.DetGeo() == "ctbh8" or jobproperties.Global.DetGeo() == "ctbh6" ):
       GeoModelSvc.DetectorTools[ "TileDetectorTool" ].TestBeam = True 

