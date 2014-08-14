# $Id: MuonTrkPhysDQA_options.py 611517 2014-08-13 05:08:12Z rrosten $
#======================================================================
# Offline Athena Muon Monitoring configuration
#======================================================================
# TOP JobOptions to run on MuonDQAMonitoring
#======================================================================
#print '\n****************************************************************************************\n'
#print "   ******** Welcome to the Offline MuonDQAMonitoring package      ***** "
#print "   ******** Documentation may be found at:******** "
#print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MuonOfflineDQA ***** "
#print '\n****************************************************************************************\n'

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
muMonMan = AthenaMonManager(name                ="MuonMonManager",
						  FileKey             = DQMonFlags.monManFileKey(),
						  Environment         = DQMonFlags.monManEnvironment(),
						  OutputLevel         = muonOutputLevel)


#======================================================================
from MuonTrkPhysMonitoring.MuonTrkPhysMonitoringConf import MuonTrkPhysMonitoring 

MuonTrkPhysMon_NoTrig = MuonTrkPhysMonitoring(name = "MuonTrkPhysMon_NoTrig",
                                              UseTriggerVector     = False,
                                              MuonTriggerChainName = "NoMuonTriggerSelection",
                                              OutputLevel          = DEBUG,
                                              MuonTrackCollections = "Muons",
                                              InnerTrackContainer  = "InDetTrackParticles",
                                              isoConeMax           = 0.3,
                                              isoConeMin           = 0.05)


#======================================================================
from AthenaCommon.AppMgr import ToolSvc

ToolSvc += MuonTrkPhysMon_NoTrig
muMonMan.AthenaMonTools += [ MuonTrkPhysMon_NoTrig ]


#======================================================================
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += muMonMan


#======================================================================
print muMonMan




















