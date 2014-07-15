# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# This contains Forward Detectors 
#
from AthenaCommon.DetFlags       import DetFlags
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()

# LUCID
if ( DetFlags.detdescr.Lucid_on() ):
    from LUCID_GeoModel.LUCID_GeoModelConf import LUCID_DetectorTool
    GeoModelSvc.DetectorTools += [ LUCID_DetectorTool() ]

# ALFA
if ( DetFlags.detdescr.ALFA_on() ):
    from ALFA_GeoModel.ALFA_GeoModel_joboption import getALFA_DetectorTool
    #from ALFA_GeoModel.ALFA_GeoModelConf import ALFA_DetectorTool
    GeoModelSvc.DetectorTools += [ getALFA_DetectorTool() ]

# ForwardRegion
if ( DetFlags.detdescr.FwdRegion_on() ):
    from ForwardRegionGeoModel.ForwardRegionGeoModelConf import ForwardRegionGeoModelTool
    GeoModelSvc.DetectorTools += [ ForwardRegionGeoModelTool() ]

# ZDC
if ( DetFlags.detdescr.ZDC_on() ):
    from ZDC_GeoM.ZDC_GeoMConf import ZDC_DetTool
    GeoModelSvc.DetectorTools += [ ZDC_DetTool() ]

# AFP
if ( DetFlags.detdescr.AFP_on() ):
    from AFP_GeoModel.AFP_GeoModelConf import AFP_GeoModelTool
    GeoModelSvc.DetectorTools += [ AFP_GeoModelTool() ]
