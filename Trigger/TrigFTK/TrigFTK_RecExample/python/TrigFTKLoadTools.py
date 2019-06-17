# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""                                                                                                 load common instances of FTK tools/services
jiri.masik@manchester.ac.uk
"""                                                                                                                                                                                                      

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from FTK_RecExample.FTKJobProperties import FTKFlags

if FTKFlags.doSmearing:
    print " FTKFlags.doSmearing=True => Creating FTKFastDataProviderSvc as TrigFTK_DataProviderSvc"
    from TrigFTK_RecExample.TrigFTK_DataProviderSvc_Config import TrigFTKFastDataProviderSvc
    theFTK_DataProviderSvc = TrigFTKFastDataProviderSvc("TrigFTK_DataProviderSvc")
    svcMgr += theFTK_DataProviderSvc
else:
    print " Creating TrigFTK_DataProviderSvc"
    from TrigFTK_RecExample.TrigFTK_DataProviderSvc_Config import TrigFTK_DataProviderSvc
    theFTK_DataProviderSvc = TrigFTK_DataProviderSvc("TrigFTK_DataProviderSvc")
    svcMgr += theFTK_DataProviderSvc


