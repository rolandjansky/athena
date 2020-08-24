printfunc ('\n*************************************************************************************************\n')
printfunc ("   ******** Welcome to the Offline ESD MuonRawDataMonitoring/TgcRawDataMonitoring package. ******** "  )
printfunc ("   ******** Documentation may be found at:******** "  )
printfunc ("   **** https://twiki.cern.ch/twiki/bin/view/Atlas/TGCOfflineMonitoringPlots ***** "  )
printfunc ('\n*************************************************************************************************\n')
if not 'MuonDQAFlags' in dir():
    printfunc ("MuonDQAFlags.py: MuonDQAFlags not yet imported - I import them now")
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
tgcRawDataValAlg = TgcRawDataValAlg(name='tgcRawDataValAlg')
#ToolSvc += tgcRawDataValAlg
tgcRawMonMan.AthenaMonTools += [ tgcRawDataValAlg ]
topSequence += tgcRawMonMan
printfunc (tgcRawMonMan)
############## TgcLv1RawDataValAlg ##########
from TgcRawDataMonitoring.TgcRawDataMonitoringConf import TgcLv1RawDataValAlg
tgcLv1RawDataValAlg = TgcLv1RawDataValAlg(name='tgcLv1RawDataValAlg')

if DQMonFlags.useTrigger():
    #ToolSvc += tgcLv1RawDataValAlg
    tgcLv1RawMonMan.AthenaMonTools += [ tgcLv1RawDataValAlg  ]
    topSequence += tgcLv1RawMonMan
    printfunc (tgcLv1RawMonMan)
