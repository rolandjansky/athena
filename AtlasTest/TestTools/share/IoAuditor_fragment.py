#
# write out a summary of the time spent
#
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]

svcMgr.ChronoStatSvc.ChronoDestinationCout = True

svcMgr.ChronoStatSvc.PrintUserTime     = True
svcMgr.ChronoStatSvc.PrintSystemTime   = True
svcMgr.ChronoStatSvc.PrintEllapsedTime = True

svcMgr.ChronoStatSvc.AsciiStatsOutputFile = "chronoStats.ascii"

svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

from AthenaCommon.AppMgr import theApp
theApp.AuditAlgorithms = True
