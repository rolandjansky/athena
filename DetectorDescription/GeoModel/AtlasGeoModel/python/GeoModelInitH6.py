# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import theApp
from G4AtlasApps.SimFlags import SimFlags

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
ServiceMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]
GeoModelSvc=GeoModelSvc()

if (SimFlags.SimLayout.get_Value()=="tb_LArH6_2002"):
    from LArGeoH62002Algs.LArGeoH62002AlgsConf import LArDetectorToolH62002
    GeoModelSvc.DetectorTools += [ LArDetectorToolH62002() ]

elif (SimFlags.SimLayout.get_Value()=="tb_LArH6EC_2002"):
    from LArGeoTBEC.LArGeoTBECConf import LArDetectorToolTBEC
    GeoModelSvc.DetectorTools += [ LArDetectorToolTBEC() ]

elif (SimFlags.SimLayout.get_Value()=="tb_LArH6_2003"):
    from LArGeoH62003Algs.LArGeoH62003AlgsConf import LArDetectorToolH62003
    GeoModelSvc.DetectorTools += [ LArDetectorToolH62003() ]

elif (SimFlags.SimLayout.get_Value()=="tb_LArH6_2004"):
    from LArGeoH62004Algs.LArGeoH62004AlgsConf import LArDetectorToolH62004
    LArDetTool = LArDetectorToolH62004()
    LArDetTool.XCryoPos = SimFlags.LArTB_H1CryoXPos.get_Value()
    LArDetTool.YTablePos = SimFlags.LArTB_H1TableYPos.get_Value()
    LArDetTool.isRun1 = SimFlags.LArTB_H6Run1.get_Value()
    LArDetTool.isEmec = SimFlags.LArTB_H6Emec.get_Value()
    LArDetTool.isHec = SimFlags.LArTB_H6Hec.get_Value()
    LArDetTool.isFcal = SimFlags.LArTB_H6Fcal.get_Value()
    LArDetTool.isColdnose = SimFlags.LArTB_H6Coldnose.get_Value()
    GeoModelSvc.DetectorTools += [ LArDetTool ]


# as long as not created anywhere else in GeoModel :
from CaloDetMgrDetDescrCnv import CaloDetMgrDDCnv
