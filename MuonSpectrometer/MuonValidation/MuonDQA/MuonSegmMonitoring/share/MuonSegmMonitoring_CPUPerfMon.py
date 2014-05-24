#--------------------------------------------------------------
# useful CPU PerfMon debugging/info tools
#--------------------------------------------------------------
theApp.Dlls += [ "GaudiAlg","GaudiAud" ]
#-------------------------------------------------------------- 
#Detail time measurement and auditors write out a summary of the time spent
#--------------------------------------------------------------  
from AthenaCommon.ConfigurableDb import getConfigurable
#-------------------------------------------------------------- 
# write out summary of the memory usage
#-------------------------------------------------------------- 
if MuonDQAFlags.doAuditor:
   theApp.AuditAlgorithms=True
   theApp.AuditServices=True
   theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
   #number of events to be skip to detect memory leak
   svcMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 5 #   number of events to be skip to detect memory leak
   if recFlags.doNameAuditor():
       theAuditorSvc += getConfigurable("NameAuditor")()
   theAuditorSvc += getConfigurable("ChronoAuditor")()
   svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

if MuonDQAFlags.doFinalCPU:
  svcMgr.ChronoStatSvc.ChronoDestinationCout = True
  svcMgr.ChronoStatSvc.PrintUserTime     = True
  svcMgr.ChronoStatSvc.PrintSystemTime   = True
  svcMgr.ChronoStatSvc.PrintEllapsedTime = True
  svcMgr.ChronoStatSvc.AsciiStatsOutputFile = "MuonSegmMonitoring_chronoStats.ascii"
#---------------------------------------------------------------------------  
# printout summary of messages
#---------------------------------------------------------------------------  
theAuditorSvc.Auditors += [ "AlgErrorAuditor" ]
# show summary of WAR NING ER ROR and FATAL messages
#---------------------------------------------------------------------------  
svcMgr.MessageSvc.showStats=True
svcMgr.MessageSvc.statLevel=WARNING
svcMgr.MessageSvc.enableSuppression = True
#--------------------------------------------------------------  
## switch on LSFTimeKeeperSvc 
#---------------------------------------------------------------------------  
if MuonDQAFlags.doTimeLimit:
   include ("LSFTimeKeeper/LSFTimeKeeperOptions.py")
   LSFTimeKeeperSvc= Service("LSFTimeKeeperSvc")
   LSFTimeKeeperSvc.OutputLevel=DEBUG
#--------------------------------------------------------------  
# Load Performance monitoring Service
# -------------------------------------------------------------
if MuonDQAFlags.doMuonDQAPerfMonitoring:
    from PerfMonComps.PerfMonFlags import jobproperties
    jobproperties.PerfMonFlags.doMonitoring = True
    jobproperties.PerfMonFlags.doPersistencyMonitoring = True
    jobproperties.PerfMonFlags.OutputFile = "muonsegmmon.perfmon.pmon.gz"
    from PerfMonComps.JobOptCfg import PerfMonSvc
    svcMgr += PerfMonSvc( "PerfMonSvc", OutputLevel = INFO )
 
#--------------------------------------------------------------
# Dump configuration to file
#--------------------------------------------------------------
import os
if MuonDQAFlags.doDumpFile:
   f = open("MuonSegmMonitoring_config.txt",'w')
   f.write( str(ServiceMgr) + os.linesep )
   f.write( str(ToolSvc) + os.linesep )
   f.write( str(topSequence) + os.linesep )
   f.close()
