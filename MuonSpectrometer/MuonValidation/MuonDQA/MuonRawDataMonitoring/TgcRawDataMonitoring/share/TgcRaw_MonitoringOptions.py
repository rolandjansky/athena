print '\n*************************************************************************************************\n'
print "   ******** Welcome to the Offline ESD MuonRawDataMonitoring/TgcRawDataMonitoring package. ******** "  
print "   ******** Documentation may be found at:******** "  
print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/TGCOfflineMonitoringPlots ***** "  
print '\n*************************************************************************************************\n'
if not 'MuonDQAFlags' in dir():
    print "MuonDQAFlags.py: MuonDQAFlags not yet imported - I import them now"
    from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags

tgcRawMonMan = AthenaMonManager(name="TgcRawMonManager",
                                FileKey             = DQMonFlags.monManFileKey(),
                                Environment         = DQMonFlags.monManEnvironment(),
                                OutputLevel         = muonOutputLevel)
tgcLv1RawMonMan = AthenaMonManager(name="TgcLv1RawMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),
                                   Environment         = DQMonFlags.monManEnvironment(),
                                   OutputLevel         = muonOutputLevel)
############## TgcRawDataValAlg #############
from TgcRawDataMonitoring.TgcRawDataMonitoringConf import TgcRawDataValAlg
tgcRawDataValAlg = TgcRawDataValAlg(name='tgcRawDataValAlg',
                                    TgcPrepDataContainer="TGC_Measurements")
ToolSvc += tgcRawDataValAlg
tgcRawMonMan.AthenaMonTools += [ tgcRawDataValAlg ]
topSequence += tgcRawMonMan
print tgcRawMonMan
############## TgcLv1RawDataValAlg ##########
from TgcRawDataMonitoring.TgcRawDataMonitoringConf import TgcLv1RawDataValAlg
tgcLv1RawDataValAlg = TgcLv1RawDataValAlg(name='tgcLv1RawDataValAlg',
                                          TgcPrepDataContainer="TGC_Measurements")

if DQMonFlags.useTrigger():
    ToolSvc += tgcLv1RawDataValAlg
    tgcLv1RawMonMan.AthenaMonTools += [ tgcLv1RawDataValAlg  ]
    topSequence += tgcLv1RawMonMan
    print tgcLv1RawMonMan
