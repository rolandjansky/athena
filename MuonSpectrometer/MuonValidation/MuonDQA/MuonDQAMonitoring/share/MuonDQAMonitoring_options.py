#======================================================================
# TOP JobOptions to run on MuonDQAMonitoring
#======================================================================
print '\n****************************************************************************************\n'
print "   ******** Welcome to the Offline MuonDQAMonitoring package      ***** "  
print "   ******** Documentation may be found at:******** "  
print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MuonOfflineDQA ***** "  
print '\n****************************************************************************************\n'

#--------------------------------------------------------------  
# Configurables JobOpts
#--------------------------------------------------------------  
## get a handle on the top sequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## add an AthenaMonManager algorithm to the list of algorithms to be ran
## AthenaMonManager is the Algorithm that manages many classes inheriting
## from ManagedMonitorToolBase
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "MuonMonManager" )

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
 
if "OutputLevel" in dir():
   svcMgr.MessageSvc.OutputLevel = OutputLevel
else:
   svcMgr.MessageSvc.OutputLevel = WARNING
 
# ******************************
# Muon Monitoring configuration
# ******************************
monMan = topSequence.MuonMonManager 
monMan.FileKey             = "GLOBAL" #"muoncosmics" ## FileKey must match that given to THistSvc

## Set global monitoring parameters: see the AthenaMonManager class
## in the Control/AthenaMonitoring package
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "user"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1

#MuonMonManager.ManualRunLBSetup    = True
#MuonMonManager.Run                 = 1
#MuonMonManager.LumiBlock           = 1
#

include ("MuonDQAMonitoring/MuonEventInfoMonitoring_options.py")
