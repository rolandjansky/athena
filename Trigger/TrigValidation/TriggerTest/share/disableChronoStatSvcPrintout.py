#-------------------------------------------------------------
# Disable overly verbose and problematic ChronoStatSvc print-out
#-------------------------------------------------------------
if 'ChronoAuditor/ChronoAuditor' in svcMgr.AuditorSvc.Auditors:
    svcMgr.AuditorSvc.Auditors.remove("ChronoAuditor/ChronoAuditor")

if hasattr(svcMgr, 'ChronoStatSvc'):
    svcMgr.ChronoStatSvc.ChronoPrintOutTable = False
    svcMgr.ChronoStatSvc.StatPrintOutTable = False
    svcMgr.ChronoStatSvc.PrintUserTime = False
