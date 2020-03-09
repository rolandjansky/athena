# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 
# Job options configuration file for PerfMonMTSvc

from PerfMonComps.PerfMonCompsConf import PerfMonMTSvc as _PerfMonMTSvc

class PerfMonMTSvc ( _PerfMonMTSvc  ):

    def __init__(self, name = "PerfMonMTSvc", **kwargs):

        kwargs['name'] = name
        super( PerfMonMTSvc, self ).__init__( **kwargs )

        return

    def setDefaults(cls, handle):

        from AthenaCommon import CfgMgr

        from AthenaCommon.AppMgr import theApp
        theApp.AuditAlgorithms = True
        theApp.AuditTools      = True
        theApp.AuditServices   = True

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr

        if hasattr(handle, "getFullJobOptName") :
            handleName = handle.getFullJobOptName()
            if not handleName in theApp.CreateSvc:
                # Be the very first service to be initialized
                theApp.CreateSvc = [ handleName ] + theApp.CreateSvc
        
        if not hasattr(svcMgr, 'AuditorSvc'):
            from GaudiSvc.GaudiSvcConf import AuditorSvc
            svcMgr += AuditorSvc()
       
    pass
