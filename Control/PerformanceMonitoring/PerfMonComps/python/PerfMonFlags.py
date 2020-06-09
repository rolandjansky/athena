# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: PerfMonFlags.py
# @purpose: a container of flags for Performance Monitoring
# @author: Sebastien Binet <binet@cern.ch>

"""  A container of flags for Performance Monitoring   

"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.17 $"
__doc__     = "A container of flags for Performance Monitoring"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class doMonitoring(JobProperty):
    """ Flag to activate the performance monitoring framework and service(s)
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    def _do_action(self):
        # install the service
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr, 'PerfMonSvc'):
            import AthenaCommon.Constants as Lvl
            from PerfMonComps.JobOptCfg import PerfMonSvc
            svcMgr += PerfMonSvc( "PerfMonSvc", OutputLevel = Lvl.INFO )
            pass
        
        ## activate persistency monitoring too
        jobproperties.PerfMonFlags.doPersistencyMonitoring = True
        #jobproperties.PerfMonFlags.doMallocMonitoring = True
        jobproperties.PerfMonFlags.doDsoMonitoring = True
        #svcMgr.PerfMonSvc.MonLvl = 1

        return

    def _undo_action(self):
        # uninstall the service
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr

        if hasattr(svcMgr, 'PerfMonSvc'):
            del svcMgr.PerfMonSvc
        return

#
class doPersistencyMonitoring(JobProperty):
    """ Flag to monitor persistency performances
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    def _do_action(self):
        if not jobproperties.PerfMonFlags.doMonitoring():
            jobproperties.PerfMonFlags.doMonitoring = True
            pass
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        #svcMgr.PerfMonSvc.MonLvl = -1
        return
# 
class doDetailedMonitoring(JobProperty):
    """ Flag to activate the detailed monitoring framework and service(s)
        It of course activates the doMonitoring flag
        (this is an alias for 'doFullMon')
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
    def _do_action(self):
        jobproperties.PerfMonFlags.doFullMon = True
        return
# 
class doFastMon(JobProperty):
    """ Flag to activate PerfMon in a lightweight mode (only event level
        monitoring, not gathering data for each component)
        It of course activates the doMonitoring flag
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
    def _do_action(self):
        # ensure flag consistency
        jobproperties.PerfMonFlags.doFullMon = False
        jobproperties.PerfMonFlags.doMonitoring = True
        # setup values
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr.PerfMonSvc.ProfiledAlgs = []
        svcMgr.PerfMonSvc.IoContainers = []
        #svcMgr.PerfMonSvc.MonLvl = 0
        return
# 
class doFullMon(JobProperty):
    """ Flag to activate the detailed monitoring framework and service(s)
        It of course activates the doMonitoring flag
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
    def _do_action(self):
        # ensure flag consistency
        jobproperties.PerfMonFlags.doFastMon = False
        jobproperties.PerfMonFlags.doMonitoring = True
        # setup values
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        #svcMgr.PerfMonSvc.MonLvl = -1
        # enable DSO monitoring
        jobproperties.PerfMonFlags.doDsoMonitoring = True
        # activate persistency monitoring too
        jobproperties.PerfMonFlags.doPersistencyMonitoring = True
        # and malloc monitoring
        jobproperties.PerfMonFlags.doMallocMonitoring = True
        # and dataproxy-size monitoring (for evtstore)
        jobproperties.PerfMonFlags.doDataProxySizeMonitoring = False
        return

#
class doMonitoringMT(JobProperty):
    """Flag to active the MT-safe monitoring framework and service(s)
       It of course deactives serial monitoring
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
    def _do_action(seld):
        # Deactive serial monitoring
        jobproperties.PerfMonFlags.doMonitoring = False
        # Setup PerfMonMTSvc
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr, 'PerfMonMTSvc'):
            from PerfMonComps.MTJobOptCfg import PerfMonMTSvc
            svcMgr += PerfMonMTSvc("PerfMonMTSvc")
        # Setup PerfMonAlg
        from AthenaCommon.AlgSequence import AthSequencer
        topSequence = AthSequencer("AthAlgSeq")
        if not hasattr(topSequence, "PerfMonMTAlg"):
            from PerfMonComps.PerfMonCompsConf import PerfMonMTAlg
            topSequence += PerfMonMTAlg("PerfMonMTAlg")
        return

    def _undo_action(self):
        # Uninstall the service and the algorithm
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaCommon.AlgSequence import AthSequencer
        topSequence = AthSequencer("AthAlgSeq")

        if hasattr(svcMgr, 'PerfMonMTSvc'):
            del svcMgr.PerfMonMTSvc
        if hasattr(topSequence, 'PerfMonMTAlg'):
            del topSequence.PerfMonMTAlg
        return
#
class doFastMonMT(JobProperty):
    """ Flag to active fast MT-safe monitoring framework and service(s)
        It also activates the doMonitoringMT flag
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
    def _do_action(self):
        jobproperties.PerfMonFlags.doMonitoringMT = True
        return

#
class doFullMonMT(JobProperty):
    """ Flag to activate full MT-safe monitoring framework and service(s)
        It also activate the doMonitoringMT flag
        Note that due to locks this functionality might negatively impact
        concurrency
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
    def  _do_action(self):
        jobproperties.PerfMonFlags.doMonitoringMT = True
        return

# 
class doSemiDetailedMonitoring(JobProperty):
    """ Flag to activate the semi-detailed monitoring framework and service(s)
        It also activates the doMonitoring flag
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
    def _do_action(self):
        jobproperties.PerfMonFlags.doMonitoring = True
        return
# 
class doSemiDetailedMonitoringFullPrint(JobProperty):
    """ Flag to activate semi-detailed monitoring with full uncollapsed printout
        It also activates the doSemiDetailedMonitoring flag
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
    def _do_action(self):
        jobproperties.PerfMonFlags.doSemiDetailedMonitoring = True
        return
# 
class doExtraPrintouts(JobProperty):
    """ Flag to activate extra INFO level printouts before and after each component is audited. Kind of like a NameAuditor but including performance metrics
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
#
class doHephaestusMon(JobProperty):
    """ Flag to activate the HephaestusMonTool monitoring tool
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
#
class doPerEvtLeakCheck(JobProperty):
    """ Flag to enable the per-event leak check of Hephaestus.
        Note that if activated, this may generate big output ASCII files (well
        of course this should not happen b/c your job does not leak, does it ?)
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
#
class doPushBackCheck(JobProperty):
    """ Flag to enable scanning StoreGateSvc for containers whose elements have
        been push_back'ed without any call to reserve or resize beforehand
        (and thus wastes CPU and memory whenever re-allocs happen)
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
#

class doPostProcessing(JobProperty):
    """ Flag to activate or not the in-athena post-processing (ie: running
    perfmon.py as part of the finalization of Athena)
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
#

class doDsoMonitoring(JobProperty):
    """ Flag to activate the monitoring of the loading of shared libraries.
    When activated, it will printout the vmem usage after any DSO has been
    loaded by the application
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
#

class doMallocMonitoring(JobProperty):
    """ Flag to activate the monitoring of the malloc hooks.
    When activated, and if glibc-malloc is used, it will printout various
    informations about malloc/free usage of the current athena job.
    """
    statusOn = True
    allowedTypes = ['bool']
    StoreValue = False
#
class doDataProxySizeMonitoring(JobProperty):
    """ Flag to activate the monitoring of the dataproxy sizes for evtstore.
    When activated, it will record heap-size informations about all DataProxy
    payloads of the evtstore.
    """
    statusOn = True
    allowedTypes = ['bool']
    StoreValue = False

    def _do_action(self):
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaCommon.AppMgr import theApp

        if hasattr(svcMgr, 'PyStorePayloadMon'):
            return

        import PerfMonComps.PyComps  as pmcpc
        svc = pmcpc.PyStorePayloadMon()
        svcMgr += svc
        theApp.CreateSvc += [svc.getFullJobOptName()]
        return
    
    def _undo_action(self):
        # uninstall the service
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr

        if hasattr(svcMgr, 'PyStorePayloadMon'):
            del svcMgr.PyStorePayloadMon
        return
#
class enableDoubleCountingCorrection(JobProperty):
    """ Flag to enable (or not) the on-the-fly double-counting of resources correction.
    default is 'False'
    """
    statusOn = True
    allowedTypes = ['bool']
    StoreValue = False
    pass

#
class OutMonLevel(JobProperty):
    """ Flag to modify PerfMon verbosity during various stages of the job.
    There are 2 levels: verbose(9), info(20)
    examples:
     jp.PerfMonFlags.OutMonLevel = 20 # info
     jp.PerfMonFlags.OutMonLevel =  9 # verbose
    """
    import AthenaCommon.Logging as L
    statusOn = True
    allowedTypes = ['int']
    StoredValue  = L.logging.INFO
    
# 
class OutputFile(JobProperty):
    """ Flag to override the default output file name of the perfmon tuple
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "ntuple.pmon.gz"

# Defines the container for the performance monitoring flags  
class PerfMonFlags(JobPropertyContainer):
    """ The global performance monitoring flag/job property container.
    """
    pass

# add the perfmon flags container to the top container 
jobproperties.add_Container(PerfMonFlags)


# We want always the following flags in the container  
list_jobproperties = [
    doMonitoring,
    doPersistencyMonitoring,
    doDetailedMonitoring,
    doFastMon,
    doFullMon,
    doMonitoringMT,
    doFastMonMT,
    doFullMonMT,
    doSemiDetailedMonitoring,
    doSemiDetailedMonitoringFullPrint,
    doExtraPrintouts,
    doHephaestusMon,
    doPerEvtLeakCheck,
    doPushBackCheck,
    doPostProcessing,
    doDsoMonitoring,
    doMallocMonitoring,
    doDataProxySizeMonitoring,
    enableDoubleCountingCorrection,
    OutMonLevel,
    OutputFile,
    ]

for i in list_jobproperties:
    jobproperties.PerfMonFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties

## -- helper function to decode athena-command-line options
def _decode_pmon_opts(opts):
    """helper function to decode athena-command-line options.

    @param opts is a list of options which can enable or disable a few
           jobproperties.PerfMonFlags fields

    one activates a perfmon flag by prepending '+' in front of the option name
    (or nothing prepended, '+' being the implied default)
    and de-activates it by prepending '-'.
    """
    #action = lambda x: None
    pmf = jobproperties.PerfMonFlags
    dispatch = {
        'perfmon':    pmf.doMonitoring,
        'fastmon':    pmf.doFastMon,
        'sdmon':      pmf.doSemiDetailedMonitoring,
        'sdmonfp':    pmf.doSemiDetailedMonitoringFullPrint,
        'fullmon':    pmf.doFullMon,
        'perfmonmt':  pmf.doMonitoringMT,
        'fastmonmt':  pmf.doFastMonMT,
        'fullmonmt':  pmf.doFullMonMT,
        'malloc-mon': pmf.doMallocMonitoring,
        'extraprint': pmf.doExtraPrintouts,
        'heph-mon':   pmf.doHephaestusMon,
        'dso-mon':    pmf.doDsoMonitoring,
        'pers-mon':   pmf.doPersistencyMonitoring,
        'dp-mon':     pmf.doDataProxySizeMonitoring,
        'post-proc':  pmf.doPostProcessing,
        }
    ## try:
    ##     import AthenaCommon.AthOptionsParser as aop
    ##     allowed = aop._allowed_values['pmon'][:]
    ##     dispat
    ## except ImportError:
    ##     allowed = 
    ##     pass

    for opt in opts:
        flag_name = opt[:]
        val = True
        if opt.startswith('-'):
            val = False
            flag_name = flag_name[1:]
        elif opt.startswith('+'):
            val = True
            flag_name = flag_name[1:]
        if not flag_name in dispatch:
            raise ValueError(
                '[%s] is not a valid PerfMonFlag (allowed: %r)' %
                (flag_name, dispatch.keys())
                )
                
        dispatch[flag_name].set_Value(val)
        
    
        
