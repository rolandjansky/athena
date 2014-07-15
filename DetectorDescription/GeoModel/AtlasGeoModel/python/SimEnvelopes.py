# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# This contains envelopes used by Simulation
#
from AthenaCommon.DetFlags       import DetFlags
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()

# ForDet
if ( DetFlags.detdescr.Lucid_on() or
     DetFlags.detdescr.ALFA_on() or
     DetFlags.detdescr.FwdRegion_on() or
     DetFlags.detdescr.AFP_on() or
     DetFlags.detdescr.ZDC_on() ):
    from GeoModelEnvelopes.GeoModelEnvelopesConf import ForDetEnvelopeTool
    GeoModelSvc.DetectorTools += [ ForDetEnvelopeTool() ]

