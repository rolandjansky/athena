# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getForwardTransportModel(name="ForwardTransportModel", **kwargs):
    kwargs.setdefault("ForwardTransportSvcName", "ForwardTransportSvc")
    kwargs.setdefault("RegionNames" , ["FWDBeamLine"] )
    return CfgMgr.ForwardTransportModelTool(name, **kwargs)
