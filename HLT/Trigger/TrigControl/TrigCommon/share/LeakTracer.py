# Job option fragment to enable the leak tracer
# doLeakCheck/memChkMode are set by athenaHLT

# from Control/AthenaCommon/share/Preparation.py
if doLeakCheck is not False:
   from AthenaCommon.AppMgr import theApp, ServiceMgr
   
   from Hephaestus.Auditor import HephaestusAuditor
   theApp.AuditAlgorithms = True
   ServiceMgr.AuditorSvc += HephaestusAuditor( mode = memChkMode, auditOn = doLeakCheck )

del doLeakCheck
del memChkMode
