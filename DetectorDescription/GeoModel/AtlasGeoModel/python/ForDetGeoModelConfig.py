# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

# TODO This isn't used yet anywhere and the new style ConfigFlag
# equivalents of the DetFlags used here have yet to be created.

def ForDetGeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc,geoModelSvc = GeoModelCfg( flags )
    from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
    acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    # LUCID
    if True:#( DetFlags.detdescr.Lucid_on() ):
        from LUCID_GeoModel.LUCID_GeoModelConf import LUCID_DetectorTool
        geoModelSvc.DetectorTools += [ LUCID_DetectorTool() ]
    # ALFA
    if True:#( DetFlags.detdescr.ALFA_on() ):
        from ALFA_GeoModel.ALFA_GeoModel_joboption import getALFA_DetectorTool
        #from ALFA_GeoModel.ALFA_GeoModelConf import ALFA_DetectorTool
        theALFA_DetectorTool=ALFA_DetectorTool(name)
        theALFA_DetectorTool.MetrologyType=3
        theALFA_DetectorTool.B7L1U_MDGeometryType = 2
        theALFA_DetectorTool.B7L1U_ODGeometryType = 2
        theALFA_DetectorTool.B7L1L_MDGeometryType = 2
        theALFA_DetectorTool.B7L1L_ODGeometryType = 2
        theALFA_DetectorTool.A7L1U_MDGeometryType = 2
        theALFA_DetectorTool.A7L1U_ODGeometryType = 2
        theALFA_DetectorTool.A7L1L_MDGeometryType = 2
        theALFA_DetectorTool.A7L1L_ODGeometryType = 2
        theALFA_DetectorTool.A7R1U_MDGeometryType = 2
        theALFA_DetectorTool.A7R1U_ODGeometryType = 2
        theALFA_DetectorTool.A7R1L_MDGeometryType = 2
        theALFA_DetectorTool.A7R1L_ODGeometryType = 2
        theALFA_DetectorTool.B7R1U_MDGeometryType = 2
        theALFA_DetectorTool.B7R1U_ODGeometryType = 2
        theALFA_DetectorTool.B7R1L_MDGeometryType = 2
        theALFA_DetectorTool.B7R1L_ODGeometryType = 2
        geoModelSvc.DetectorTools += [ thALFA_DetectorTool ]
        acc.merge(addFoldersSplitOnline(flags,'FWD','/FWD/Onl/ALFA/position_calibration','/FWD/ALFA/position_calibration'))
    # ForwardRegion
    if True:#( DetFlags.detdescr.FwdRegion_on() ):
        from ForwardRegionGeoModel.ForwardRegionGeoModelConf import ForwardRegionGeoModelTool
        geoModelSvc.DetectorTools += [ ForwardRegionGeoModelTool() ]
    # ZDC
    if True:#( DetFlags.detdescr.ZDC_on() ):
        from ZDC_GeoM.ZDC_GeoMConf import ZDC_DetTool
        geoModelSvc.DetectorTools += [ ZDC_DetTool() ]
    # AFP
    if True:#( DetFlags.detdescr.AFP_on() ):
        from AFP_GeoModel.AFP_GeoModelConf import AFP_GeoModelTool
        geoModelSvc.DetectorTools += [ AFP_GeoModelTool() ]
    acc.addService(geoModelSvc)
    return acc
