# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# LAr GeoModel initialization
#
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.DetFlags      import DetFlags

if ( DetFlags.detdescr.LAr_on() ):
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    GeoModelSvc = GeoModelSvc()

    from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
    GeoModelSvc.DetectorTools += [ LArDetectorToolNV() ]

    # as long as not created anywhere else in GeoModel :
    from CaloDetMgrDetDescrCnv import CaloDetMgrDDCnv

    # apply possible alignments
    if ( jobproperties.Global.DetGeo() == "atlas" or
         jobproperties.Global.DetGeo() == "commis" ):
        from LArConditionsCommon import LArAlignable
        GeoModelSvc.DetectorTools[ "LArDetectorToolNV" ].ApplyAlignments = True
