# @file: KitValidation/KitValidation_jobOptions.py
# @author: Alessandro De Salvo
# $Id: KitValidation_jobOptions.py,v 1.0 2008/05/22 00:46:00 desalvzp Exp $

###############################
# Load PerfMonSvc
###############################
try:
    from PerfMonComps.PerfMonFlags import jobproperties
    jobproperties.PerfMonFlags.doMonitoring = True
    jobproperties.PerfMonFlags.doFastMon = False
    include( "PerfMonComps/PerfMonSvc_jobOptions.py" )

except Exception:
    treatException("Could not load PerfMon" )

###############################
# Enable library debug
###############################
#try:
#    theApp.ReflexPluginDebugLevel = 1000
#except Exception:
#    treatException("Could not set the Reflex Plugin debug level" )
