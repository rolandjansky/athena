# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file: AtlasGeoModel/python/GeoModelInit.py
## @purpose: encapsulate GeoModel configuration
 
def _setupGeoModel():
    from AthenaCommon.JobProperties import jobproperties
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
 
    try:
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
        if not (hasattr(svcMgr,'DetDescrCnvSvc') and TrkDetFlags.ISF_FatrasCustomGeometry()):
            import DetDescrCnvSvc.DetStoreConfig
            svcMgr.DetDescrCnvSvc.IdDictFromRDB = True
    except ImportError:
        import DetDescrCnvSvc.DetStoreConfig
        svcMgr.DetDescrCnvSvc.IdDictFromRDB = True

    # Conditions DB setup and TagInfo
    from IOVDbSvc.CondDB import conddb
    import EventInfoMgt.EventInfoMgtInit
 
    if ( jobproperties.Global.DataSource() == "geant3" ):
        from AtlasGeoModel import GeoModelInitDC1
 
    elif ( jobproperties.Global.DetGeo() == "ctbh8" ):
        from AtlasGeoModel import GeoModelInitCTB
 
    elif ( jobproperties.Global.DetGeo() == "ctbh6" ):
        from AtlasGeoModel import GeoModelInitH6
 
    else:
        from AtlasGeoModel import GeoModelInitStandard

    pass

## setup GeoModel at module import
_setupGeoModel()

## clean-up: avoid running multiple times this method
del _setupGeoModel
