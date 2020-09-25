#======================================================================
# Offline Athena Muon Monitoring configuration
#======================================================================
# TOP JobOptions to run MuonDQAMonitoring at Tier0
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

#--------------------------------------------------------------  
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if "OutputLevel" in dir():
   svcMgr.MessageSvc.OutputLevel = OutputLevel
else:
   svcMgr.MessageSvc.OutputLevel = WARNING
#--------------------------------------------------------------  

if not 'DQMonFlags' in dir():
   print("MuonDQAMonitoring_Tier0.py: DQMonFlags not yet imported - I import them now")
   from AthenaMonitoring.DQMonFlags import DQMonFlags

if DQMonFlags.monManEnvironment() == 'tier0Raw':
   MuonRawMon=True
   MuonESDMon=False
elif DQMonFlags.monManEnvironment() == 'tier0ESD':
   MuonRawMon=False
   MuonESDMon=True
else: # 'tier0'
   MuonRawMon=True
   MuonESDMon=True
 
if DQMonFlags.doMonitoring:
# ******************************
# Muon Monitoring configuration
# ******************************
    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    monMan = AthenaMonManager(name="MuonMonManager",
                              FileKey             = DQMonFlags.monManFileKey,
                              ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup,
                              DataType            = DQMonFlags.monManDataType,
                              Environment         = DQMonFlags.monManEnvironment,
                              ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup,
                              Run                 = DQMonFlags.monManRun,
                              LumiBlock           = DQMonFlags.monManLumiBlock)
    topSequence += monMan
#--------------------------------------------------------------
    ### MuonRaw Monitoring ###
    if DQMonFlags.doMuonRawMon:
        if MuonRawMon:
            include("MuonRawDataMonitoring/MuonRawBS_MonitoringOptions.py")
        elif MuonESDMon:
            include("MuonRawDataMonitoring/MuonRawESD_MonitoringOptions.py")
#--------------------------------------------------------------
    ### MuonSegm Monitoring ###
    if DQMonFlags.doMuonSegmMon and MuonESDMon:
        include("MuonSegmMonitoring/MuonSegmDQA_options.py")
#--------------------------------------------------------------	     
    ### MuonTrk Monitoring ###
    if DQMonFlags.doMuonTrackMon and MuonESDMon:
        include("MuonTrackMonitoring/MuonTrackDQA_options.py")
#--------------------------------------------------------------
    ### MuonPhys Monitoring ###
    if DQMonFlags.doMuonPhysMon and MuonESDMon:
        include("MuonPhysicsMonitoring/MuonPhysicsDQA_options.py") 
#--------------------------------------------------------------
    ### MuonEventInfo Monitoring ###
    if MuonDQAFlags.doMuonEvtMon:
        include("MuonDQAMonitoring/MuonEventInfoMonitoring_options.py")
#--------------------------------------------------------------
    print monMan;
