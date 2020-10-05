#======================================================================
# Offline Athena Muon Monitoring configuration
#======================================================================
# TOP JobOptions to run on MuonDQAMonitoring
#======================================================================  
#printfunc ('\n****************************************************************************************\n')
#printfunc ("   ******** Welcome to the Offline MuonDQAMonitoring package      ***** "  )
#printfunc ("   ******** Documentation may be found at:******** "  )
#printfunc ("   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MuonOfflineDQA ***** "  )
#printfunc ('\n****************************************************************************************\n')

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from MuonRecExample.MuonRecFlags import muonRecFlags
from RecExConfig.RecFlags import rec as recFlags
 
if not 'MuonDQADetFlags' in dir():
    printfunc ("MuonDQADetFlags.py: MuonDQADetFlags not yet imported - I import them now")
    from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags

MuonDQADetFlags.doMuonMonitoring = True

## --- sudetector specific flags
if muonRecFlags.doCSCs():
    MuonDQADetFlags.doCSCMon = True
    MuonDQADetFlags.doCSCClusMon = True
else:
    MuonDQADetFlags.doCSCMon = False
    MuonDQADetFlags.doCSCClusMon = False

if muonRecFlags.doMDTs():
    MuonDQADetFlags.doMDTMon = True
else:
    MuonDQADetFlags.doMDTMon = False

if muonRecFlags.doRPCs():    
    MuonDQADetFlags.doRPCL1Mon = True
    MuonDQADetFlags.doRPCMon = True
else:
    MuonDQADetFlags.doRPCL1Mon = False
    MuonDQADetFlags.doRPCMon = False
    
if muonRecFlags.doMDTs() and muonRecFlags.doRPCs():
    MuonDQADetFlags.doMDTRPCMon = True
else:
    MuonDQADetFlags.doMDTRPCMon = False
    
if muonRecFlags.doMDTs() and muonRecFlags.doTGCs():
    MuonDQADetFlags.doMDTTGCL1Mon = True
else:
    MuonDQADetFlags.doMDTTGCL1Mon = False

if muonRecFlags.doTGCs():
    MuonDQADetFlags.doTGCMon = True
    MuonDQADetFlags.doTGCL1Mon = True
else:
    MuonDQADetFlags.doTGCMon = False
    MuonDQADetFlags.doTGCL1Mon = False
    
# --- Muon Segm Monitoring
MuonDQADetFlags.doMuonSegmMon = False
MuonDQADetFlags.doCSCSegmMon = False

## --- Muon StandAlone Trk Monitoring and MuonCombined options
MuonDQADetFlags.doMuonTrackMon = True

## --- MuonTrk Trigger Aware Monitoring options
MuonDQADetFlags.MuonTrkMonDoTrigger = False 

# --- Muon Alignment Monitoring
MuonDQADetFlags.doMuonTrkAlignMon = False

# --- MuonPhysics Monitoring options
MuonDQADetFlags.doMuonPhysMon = False

from RecExConfig.RecFlags import rec
MuonDQADetFlags.doMuTrkMon = rec.doInDet()

# --- HighLvl Muon Monitoring options
#MuonDQADetFlags.doMuonTrkPhysMon = rec.doMuon()

#MuonDQADetFlags.doMuonTrkPhysMon = True
#
if recFlags.doInDet() and recFlags.doMuon():
    MuonDQADetFlags.doMuonTrkPhysMon = True
    MuonDQADetFlags.doMuonCbTrkAlignMon = False
else:
    MuonDQADetFlags.doMuonTrkPhysMon = False
    MuonDQADetFlags.doMuonCbTrkAlignMon = False

#----------------------#
# Muon raw monitoring  #
#----------------------#
MuonRawMon=False
MuonESDMon=False
if DQMonFlags.monManEnvironment() == 'online':
    MuonRawMon=True
    MuonESDMon=True
elif DQMonFlags.monManEnvironment() == 'tier0Raw':
    MuonRawMon=True
    MuonESDMon=False
elif DQMonFlags.monManEnvironment() == 'tier0ESD':
    MuonRawMon=False
    MuonESDMon=True
elif DQMonFlags.monManEnvironment() == 'tier0': 
    MuonRawMon=True
    MuonESDMon=True

## add an AthenaMonManager algorithm to the list of algorithms to be ran
from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
        
muonOutputLevel=INFO
if DQMonFlags.doMuonRawMon():
    try:
        if MuonRawMon:
            if MuonDQADetFlags.doRPCL1Mon():
                include ("RpcRawDataMonitoring/RpcRawBS_MonitoringOptions.py") 
            if MuonDQADetFlags.doCSCMon() or MuonDQADetFlags.doCSCClusMon():
                include ("CscRawDataMonitoring/CscRawBS_MonitoringOptions.py")
        if MuonESDMon:
            # add configuration of muon cablings 
            import MuonCnvExample.MuonCablingConfig
            if MuonDQADetFlags.doMDTMon():
                include ("MdtRawDataMonitoring/MdtRaw_MonitoringOptions.py")
            if MuonDQADetFlags.doRPCMon():
                include ("RpcRawDataMonitoring/RpcRawESD_MonitoringOptions.py")
            if MuonDQADetFlags.doTGCMon() or MuonDQADetFlags.doTGCL1Mon:
                include ("TgcRawDataMonitoring/TgcRaw_MonitoringOptions.py")
            if MuonDQADetFlags.doCSCMon():
                include ("CscRawDataMonitoring/CscRawESD_MonitoringOptions.py")
            if MuonDQADetFlags.doMDTRPCMon():
                include ("MdtVsRpcRawDataMonitoring/MdtVsRpcRaw_MonitoringOptions.py")
            if MuonDQADetFlags.doMDTTGCL1Mon():
                include ("MdtVsTgcRawDataMonitoring/MdtVsTgcRaw_MonitoringOptions.py")
    except Exception:
        treatException("DataQualitySteering_jobOptions.py: exception when setting up Muon raw monitoring")
#-------------------------#
# Muon segment monitoring #
#-------------------------#
if DQMonFlags.doMuonSegmentMon():
    try:
        if MuonDQADetFlags.doMuonSegmMon():
            include ("MuonSegmMonitoring/MuonSegmDQA_options.py")
    except Exception:
        treatException("DataQualitySteering_jobOptions.py: exception when setting up Muon segment monitoring")
    try:
        if MuonDQADetFlags.doCSCSegmMon():
            include ("MuonSegmMonitoring/MuonCSCSegmDQA_options.py")    
    except Exception:
        treatException("DataQualitySteering_jobOptions.py: exception when setting up Muon CSC segment monitoring")

#------------- ---------#
# Muon track monitoring #
#------------- ---------#
# switch between legacy and new code
Run3_test = False
if DQMonFlags.doMuonTrackMon():
    try:
        if MuonESDMon and Run3_test:
            from MuonTrackMonitoring.MuonTrackMonitorAlgorithm import MuonTrackConfig
            topSequence += MuonTrackConfig(DQMonFlags,isOld=True)

        # Legacy monitoring
        elif MuonESDMon:
            if DQMonFlags.useTrigger(): ## monitoring tool cannot have a dependence on TrigDecisionTool if DQMonFlags.useTrigger==False (ATLASRECTS-3549)
                if MuonDQADetFlags.doMuonTrackMon():
                    include ("MuonTrackMonitoring/MuonTrackDQA_options.py")
                if MuonDQADetFlags.MuonTrkMonDoTrigger():
                    include ("MuonTrackMonitoring/MuonTrigTrackDQA_options.py")
    except Exception:
        treatException("DataQualitySteering_jobOptions.py: exception when setting up Muon track monitoring")

#-------------------------#
# Muon physics monitoring #
#-------------------------#
if DQMonFlags.doMuonPhysicsMon():
    try:
        if MuonDQADetFlags.doMuonPhysMon():
            include ("MuonPhysicsMonitoring/MuonPhysicsDQA_options.py")
    except Exception:
        treatException("DataQualitySteering_jobOptions.py: exception when setting up Muon physics monitoring")
    try:            
        if MuonDQADetFlags.doMuTrkMon():
            include ("MuonPhysicsMonitoring/MuonTrackPhysicsDQA_options.py")
    except Exception:
        treatException("DataQualitySteering_jobOptions.py: exception when setting up MuonTrk physics monitoring")
