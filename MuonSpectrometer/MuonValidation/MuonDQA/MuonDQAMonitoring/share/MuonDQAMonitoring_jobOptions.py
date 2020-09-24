#======================================================================
# Offline Athena Muon Monitoring configuration
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

if not 'MuonDQAFlags' in dir():
    print("MuonDQAFlags.py: MuonDQAFlags not yet imported - I import them now")
    from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags

#--------------------------------------------------------------  
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if "OutputLevel" in dir():
    svcMgr.MessageSvc.OutputLevel = OutputLevel
else:
    svcMgr.MessageSvc.OutputLevel = WARNING
#--------------------------------------------------------------  

MuonDQAFlags.doMuonMonitoring = True
 
if not 'DQMonFlags' in dir():
    print("DataQualityMon_RecExCommon_Flags_jobOptions.py: DQMonFlags not yet imported - I import them now")
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    monEnv="tier0ESD"

if DQMonFlags.monManEnvironment() == 'tier0Raw':
    MuonRawMon=True
    MuonESDMon=False
elif DQMonFlags.monManEnvironment() == 'tier0ESD':
    MuonRawMon=False
    MuonESDMon=True
else: # 'tier0'
    MuonRawMon=True
    MuonESDMon=True
 
if DQMonFlags.doMonitoring() and MuonDQAFlags.doMuonMonitoring:
# ******************************
# Muon Monitoring configuration
# ******************************
    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    monMan = AthenaMonManager(name="MuonMonManager",
                              FileKey             = "GLOBAL",
                              ManualDataTypeSetup = True,
                              DataType            = "userDefined",# use this for collision data for now
                              Environment         = "user",
                              ManualRunLBSetup    = True,
                              Run                 = 1,
                              LumiBlock           = 1)
    topSequence += monMan
#--------------------------------------------------------------
    ###MuonHit Monitoring
    if MuonDQAFlags.doMuonHitMon:
        if MuonRawMon:
            include("MuonRawDataMonitoring/MuonRawDataMonitoring_options.py")
        #if recFlags.readESD and MuonESDMon and (DQMonFlags.monManEnvironment=='tier0ESD'):
        #    include("MuonRawDataMonitoring/MuonRawDataMonitoringESD_options.py")	    
        ##Temporary is commented out
	#if recFlags.readESD and MuonESDMon and (DQMonFlags.monManEnvironment=='tier0ESD'):
	###if recFlags.readESD and MuonESDMon:
            ###include("MuonRawDataMonitoring/MuonRawDataMonitoringESD_options.py")
#--------------------------------------------------------------
    ###MuonSegm Monitoring
    if MuonDQAFlags.doMuonSegmMon:
        include("MuonSegmMonitoring/MuonSegmMonitoring_options.py")
        if muonRecFlags.doMoore and not muonRecFlags.doMuonboy:
            ToolSvc.MuonSegmValidation.WhichSegmCollections = [1,0]
        if muonRecFlags.doMuonboy and not muonRecFlags.doMoore:
            ToolSvc.MuonSegmValidation.WhichSegmCollections = [0,1]
#--------------------------------------------------------------	     
    ###MuonTrk Monitoring
    if MuonDQAFlags.doMuonTrackMon:
        include("MuonTrackMonitoring/MuonTrackMonitoring_options.py")
        if muonRecFlags.doMuonboy and not muonRecFlags.doMoore:
            ToolSvc.MuonGenericTracksMon.WhichTrackCollections = [0,1]
            ToolSvc.MuonSelectedTracksMon.WhichTrackCollections = [0,1] 	
        if muonRecFlags.doMoore and not muonRecFlags.doMuonboy:
            ToolSvc.MuonGenericTracksMon.WhichTrackCollections = [1,0]
            ToolSvc.MuonSelectedTracksMon.WhichTrackCollections = [1,0]
#--------------------------------------------------------------	     
    ###MuonTrk Alignment Monitoring
    if MuonDQAFlags.doMuonTrkAlignMon:
        include("MuonAlignMonitoring/MuonAlignDQA_options.py")
#--------------------------------------------------------------
    ###MuonPhys Monitoring
    if MuonDQAFlags.doMuonPhysMon:
        include("MuonPhysicsMonitoring/MuonPhysicsMonitoring_options.py") 
#--------------------------------------------------------------
    ###MuonEventInfo Monitoring
    if MuonDQAFlags.doMuonEvtMon:
        include("MuonDQAMonitoring/MuonEventInfoMonitoring_options.py")
#--------------------------------------------------------------
    print monMan;
