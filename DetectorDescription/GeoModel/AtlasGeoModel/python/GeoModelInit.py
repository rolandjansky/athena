# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @file: AtlasGeoModel/python/GeoModelInit.py
## @brief: Encapsulate GeoModel configuration
 
def _setupGeoModel():
    from AthenaCommon.JobProperties import jobproperties
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
 
    try:
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
        if not (hasattr(svcMgr,'DetDescrCnvSvc') and TrkDetFlags.ISF_FatrasCustomGeometry()):
            import DetDescrCnvSvc.DetStoreConfig
            svcMgr.DetDescrCnvSvc.IdDictFromRDB = True
    except ImportError:
        import DetDescrCnvSvc.DetStoreConfig  # noqa: F401
        svcMgr.DetDescrCnvSvc.IdDictFromRDB = True

    # Conditions DB setup and TagInfo
    from IOVDbSvc.CondDB import conddb    # noqa: F401
    import EventInfoMgt.EventInfoMgtInit  # noqa: F401
 
    if ( jobproperties.Global.DataSource() == "geant3" ):
        from AtlasGeoModel import GeoModelInitDC1  # noqa: F401

    elif ( jobproperties.Global.DetGeo() == "ctbh8" ):
        from AtlasGeoModel import GeoModelInitCTB  # noqa: F401
 
    elif ( jobproperties.Global.DetGeo() == "ctbh6" ):
        from AtlasGeoModel import GeoModelInitH6   # noqa: F401
 
    else:
        from AtlasGeoModel import GeoModelInitStandard  # noqa: F401

    pass

## setup GeoModel at module import
_setupGeoModel()

## clean-up: avoid running multiple times this method
del _setupGeoModel
