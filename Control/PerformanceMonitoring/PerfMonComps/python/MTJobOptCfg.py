# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 
# Job options configuration file for PerfMonMTSvc

from PerfMonComps.PerfMonCompsConf import PerfMonMTSvc as _PerfMonMTSvc

class PerfMonMTSvc ( _PerfMonMTSvc  ):

    def __init__(self, name = "PerfMonMTSvc", **kwargs):

        kwargs['name'] = name
        super( PerfMonMTSvc, self ).__init__( **kwargs )

        return

    def setDefaults(cls, handle):

        ## Import the PerfMonFlags
        from PerfMonComps.PerfMonFlags import jobproperties

        ## Continue, only if we are required to:
        if not jobproperties.PerfMonFlags.doMonitoringMT():
            return

        ## Continue, only if it is our Configurable
        if not isinstance(handle, PerfMonMTSvc):
            return

        from AthenaCommon import CfgMgr

        ## Enable the auditors
        from AthenaCommon.AppMgr import theApp
        theApp.AuditAlgorithms = True
        theApp.AuditTools      = True
        theApp.AuditServices   = True

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr

        if hasattr(handle, "getFullJobOptName") :
            handleName = handle.getFullJobOptName()
            if handleName not in theApp.CreateSvc:
                # Be the very first service to be initialized
                theApp.CreateSvc = [ handleName ] + theApp.CreateSvc

        ## Set the job start time
        import os,psutil
        handle.wallTimeOffset = psutil.Process(os.getpid()).create_time() * 1000

        ## Set the monitoring check points
        from AthenaCommon.ConcurrencyFlags import jobproperties as jp
        handle.numberOfThreads = max(1,jp.ConcurrencyFlags.NumThreads())
        handle.numberOfSlots = max(1,jp.ConcurrencyFlags.NumConcurrentEvents())

        ## Make sure the auditor service is there
        if not hasattr(svcMgr, 'AuditorSvc'):
            from GaudiSvc.GaudiSvcConf import AuditorSvc
            svcMgr += AuditorSvc()

        ## Turn on component-level monitoring if asked by the user
        if jobproperties.PerfMonFlags.doFullMonMT():
            handle.doComponentLevelMonitoring = True

        return

    pass # class PerfMonMTSvc
