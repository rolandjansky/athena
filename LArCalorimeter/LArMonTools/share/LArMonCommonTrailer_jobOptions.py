#---------------------------------------------------------- 
if not online: 
    from GaudiSvc.GaudiSvcConf import THistSvc
    if os.path.exists(RootHistOutputFileName): 
        os.remove(RootHistOutputFileName)
    svcMgr += THistSvc()
    svcMgr.THistSvc.Output = ["AllMon DATAFILE='"+RootHistOutputFileName+"' OPT='New'"] 
    if Type == 'Delay' and DelayNtuple:
        theApp.Dlls += [ "RootHistCnv" ]
        theApp.HistogramPersistency = "ROOT"
        from GaudiSvc.GaudiSvcConf import NTupleSvc
        svcMgr += NTupleSvc()
        svcMgr.NTupleSvc.Output = "FILE1 DATAFILE='"+DelayOutputFile+"' OPT='NEW'"


# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL ) 
svcMgr.MessageSvc.OutputLevel  = 4
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

if not online:
    svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = 5)
    from AthenaCommon.AppMgr import theAuditorSvc
    from AthenaCommon.ConfigurableDb import getConfigurable
    theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = FATAL)
    theAuditorSvc += getConfigurable("ChronoAuditor")()
    theAuditorSvc += getConfigurable("NameAuditor")()
    theApp.EvtMax = EvtNo

 
include ("RDBAccessSvc/RDBAccessSvcPdb_jobOptions.py") 
RDBAccessSvc = Service( "RDBAccessSvc" ) 
RDBAccessSvc.HostName     = "atlas_dd" 
