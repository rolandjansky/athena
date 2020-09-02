# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file: JobOptCfg.py
# @purpose: a customized Configurable class for the PerfMonSvc
# @author: Sebastien Binet <binet@cern.ch>

## import the automatically generated Configurables
from __future__ import print_function
from PerfMonComps.PerfMonCompsConf import PerfMonSvc as _PerfMonSvc
import six

class PerfMonSvc( _PerfMonSvc ):

    __slots__ = ( )      # enforce no new properties

    def __init__(self, name = "PerfMonSvc", **kwargs ):

        # have to call base init
        kwargs['name'] = name
        super( PerfMonSvc, self ).__init__( **kwargs )

        return

    def setDefaults( cls, handle ):

        ## get a handle on the service manager
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr

        ## don't allow the service to be used in MT>1 jobs
        from AthenaCommon.ConcurrencyFlags import jobproperties as jp
        if jp.ConcurrencyFlags.NumThreads() > 1:
            print(" >> PerfMonSvc should NOT be used in MT>1 jobs, self removing...")
            if hasattr(svcMgr, 'PerfMonSvc'):
                del svcMgr.PerfMonSvc
            return

        ## import the PerfMonFlags
        from PerfMonComps.PerfMonFlags import jobproperties

        ## continue, only if we are required to:
        if not jobproperties.PerfMonFlags.doMonitoring():
            return

        ## continue, only if it is our Configurable
        if not isinstance(handle, PerfMonSvc):
            return

        from AthenaCommon import CfgMgr
        ## get a handle on the application manager
        from AthenaCommon.AppMgr import theApp
        if not jobproperties.PerfMonFlags.doFastMon() or jobproperties.PerfMonFlags.doSemiDetailedMonitoring():
            theApp.AuditAlgorithms = True
            theApp.AuditTools      = True
            theApp.AuditServices   = True

        ## get output levels
        import AthenaCommon.Constants as Lvl
        
        ## make sure the application manager explicitly creates the service
        if hasattr(handle, "getFullJobOptName") :
            handleName = handle.getFullJobOptName()
            if not handleName in theApp.CreateSvc:
                # we want to be the very first service to be initialized
                # so we can monitor what happens during initialize
                theApp.CreateSvc = [ handleName ] + theApp.CreateSvc

        fname = handle.getDefaultProperty('OutFileName')
        ## provide a default tuple file name from the jobproperties
        if not hasattr(handle, "OutFileName") and \
           hasattr(handle, "getDefaultProperty") :
            fname = jobproperties.PerfMonFlags.OutputFile()
        if fname.endswith('.pmon.gz'):
            fname = fname.replace('.pmon.gz','')
        elif fname.endswith('.root'):
            fname = fname.replace('.root','')
        handle.OutFileName = fname
        
        ## configure the auditor service if not done already...
        if not hasattr(svcMgr, 'AuditorSvc'):
            from GaudiSvc.GaudiSvcConf import AuditorSvc
            svcMgr += AuditorSvc()
        
        ## configure the chronostat service
        if not hasattr(svcMgr, 'ChronoStatSvc'):
            from GaudiSvc.GaudiSvcConf import ChronoStatSvc
            svcMgr += ChronoStatSvc( OutputLevel = Lvl.WARNING,
                                     ChronoPrintOutTable = False )

        ## communicate the whole list of Algorithms if no default provided
        ## by user: we ask for all the ConfigurableAlgorithm's instances
        componentNames = []
        if not hasattr(handle, "ProfiledAlgs") or \
           (hasattr(handle, "ProfiledAlgs") and len(handle.ProfiledAlgs) == 0):
            from AthenaCommon.Configurable import (Configurable,
                                                   ConfigurableAlgorithm,
                                                   ConfigurableAlgTool,
                                                   ConfigurableService)
            for k,v in Configurable.allConfigurables.items():
                if isinstance( v, ( ConfigurableAlgorithm,
                                    ConfigurableAlgTool,
                                    ConfigurableService ) ) \
                   and not k in ('PerfMonSvc',
                                 'AthenaEventLoopMgr',):
                    componentNames.append (v.getJobOptName())
                    pass
            pass
        else:
            componentNames = [k for k in handle.ProfiledAlgs[:]]

        ## try to fetch the list of containers to be written out from the
        ## registered OutputStreams (and if we are configured to do such a
        ## thing)
        ioContainers = []
        if (jobproperties.PerfMonFlags.doPersistencyMonitoring() and
            not jobproperties.PerfMonFlags.doFastMon()) :
                    
            ## make sure the AthenaPoolCnvSvc is correctly configured
            try:   svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True
            except AttributeError:
                # ok... maybe not a 'writeAthenaPool' job...
                pass

            ## add our ioContainers list if none has been provided
            if not hasattr(handle, "IoContainers") or \
               ( hasattr(handle, "IoContainers") and \
                 len(handle.IoContainers) == 0 ):
                # first let us ask the Streams...
                from AthenaCommon.Configurable import Configurable
                if 'Streams' in Configurable.configurables:
                    outStreams = Configurable.configurables['Streams']
                    for o in outStreams:
                        if not hasattr(o, 'ItemList'): continue
                        ioContainers += [ i for i in o.ItemList
                                          if not i.endswith("#*") ]
                    pass
                # then, what about objKeyStore ?
                from AthenaCommon.KeyStore import CfgKeyStore
                keystore = CfgKeyStore.instances.get('KeyStore')
                if keystore:
                    ioLabels = [ "streamRDO","streamESD",
                                 "streamAOD","streamTAG",
                                 "inputBackNav","inputFile" ]
                    for l in ioLabels:
                        try:
                            ioContainers.extend(keystore[l].list())
                        except AttributeError:
                            for k,v in keystore[l].items():
                                ioContainers += [ "%s#%s" % (k,c) for c in v ]
                    pass
                ## collect everything
                handle.IoContainers = ioContainers
                pass
            pass

        ## communicate the granularity of monitoring from the job-flags
        if jobproperties.PerfMonFlags.doFullMon():
            handle.MonLvl =  -1
        elif jobproperties.PerfMonFlags.doFastMon():
            handle.MonLvl =  0
        elif jobproperties.PerfMonFlags.doMonitoring():
            handle.MonLvl =  1
        else:
            handle.MonLvl = -1
            pass

        if jobproperties.PerfMonFlags.doSemiDetailedMonitoring():
            if jobproperties.PerfMonFlags.doSemiDetailedMonitoringFullPrint():
                handle.SemiDetailedMonitoring=2
            else:
                handle.SemiDetailedMonitoring=1
            #Collect the start-time (in jiffies) of the configuration process
            try:
                j=open('/proc/self/stat').readline().split()[21]
            except Exception as err:
                #Not worth stopping the job over missing diagnostics.
                #We should notice the missing info in any case.
                j=""
                print("PMonSD WARNING: Could not determine job start time:",err)
                pass
            handle.JobStartJiffies=j
        handle.ExtraPrintouts=jobproperties.PerfMonFlags.doExtraPrintouts()

        ## communicate the double-counting correction from the job-flags
        val = jobproperties.PerfMonFlags.enableDoubleCountingCorrection()
        if val:
            handle.EnableDoubleCountingCorrection = True
        else:
            handle.EnableDoubleCountingCorrection = False
        
        pmon_flags = {}
        import AthenaCommon.JobProperties as acjp
        for n,v in six.iteritems (jobproperties.PerfMonFlags.__dict__):
            if isinstance(v, acjp.JobProperty):
                pmon_flags[n] = v()
                
        ## create PyPerfMon.Svc object
        from .PyPerfMon import Svc
        svc = Svc(
            name=handle.getName(),
            properties = {
                'outFileName'  : "%s" % handle.OutFileName,
                'components'   : componentNames,
                'ioContainers' : ioContainers,
                'pmon_flags' : pmon_flags,
            })

        ## store the py-perfmonsvc object into a sqlite-pickle...
        import PyUtils.dbsqlite as dbs
        pmon_props = dbs.open(handle.OutFileName+'.props.pkl', 'w')
        pmon_props['pmon'] = svc
        pmon_props.close()
        return

    pass # class PerfMonSvc
