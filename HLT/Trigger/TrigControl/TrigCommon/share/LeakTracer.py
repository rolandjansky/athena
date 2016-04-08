# Job option fragment to enable the leak tracer
# doLeakCheck is set by athenaMT/PT

# from Control/AthenaCommon/share/Preparation.py
if doLeakCheck != False:
   from AthenaCommon.AppMgr import theApp, ServiceMgr
   
   from Hephaestus.Auditor import HephaestusAuditor
   theApp.AuditAlgorithms = True
   ServiceMgr.AuditorSvc += HephaestusAuditor( mode = memChkMode ,auditOn = doLeakCheck )

del doLeakCheck
del memChkMode
