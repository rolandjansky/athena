#======================================================================
# JobOptions for MuonTrackMonitoring
#======================================================================

muTrackMonMan = AthenaMonManager(name="MuonTrackMonManager",
                                 FileKey             = DQMonFlags.monManFileKey(),
                                 Environment         = DQMonFlags.monManEnvironment(),
                                 OutputLevel         = muonOutputLevel)

from RecExConfig.RecFlags import rec as recFlags
from MuonRecExample import MuonRecTools

from MuonTrackMonitoring.MuonTrackMonitoringConf import MuonGenericTracksMon

#No trigger config
MuonGenericTracksMon_NoTrig = MuonGenericTracksMon(name = "MuonGenericTracksMon_NoTrig",
                                                   OutputLevel = WARNING)
ToolSvc += MuonGenericTracksMon_NoTrig

muTrackMonMan.AthenaMonTools += [ MuonGenericTracksMon_NoTrig ]
MuonGenericTracksMon_NoTrig.EnableLumi = True
topSequence += muTrackMonMan
print muTrackMonMan
