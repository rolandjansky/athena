#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory


def InDetServiceMaterialCfg(flags):
    from AtlasGeoModel.GeometryDBConfig import InDetGeometryDBSvcCfg
    db = InDetGeometryDBSvcCfg(flags)

    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)
    geoModelSvc = acc.getPrimary()
    geoModelSvc.DetectorTools += [ CompFactory.InDetServMatTool(GeometryDBSvc=db.getPrimary()) ]
    acc.merge(db)
    return acc
