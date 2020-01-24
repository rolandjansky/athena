#printfunc ('\n************************************************************************************************\n')
#printfunc ("   ******** Welcome to the Offline BS MuonRawDataMonitoring/RpcRawDataMonitoring package. ******** "  )
#printfunc ("   ******** Documentation may be found at:******** "  )
#printfunc ("   **** https://twiki.cern.ch/twiki/bin/view/Atlas/RPCOfflineMonitoringPlots ***** "  )
#printfunc ('\n************************************************************************************************\n')

if not 'MuonDQAFlags' in dir():
    printfunc ("MuonDQAFlags.py: MuonDQAFlags not yet imported - I import them now")
    from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags

############# RPCLV1RawDataValAlg #############
rpcLv1RawMonMan = AthenaMonManager(name="RpcLv1RawMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),
                                   Environment         = DQMonFlags.monManEnvironment(),
                                   OutputLevel         = muonOutputLevel)

OutputMessageLevel = WARNING
from RpcRawDataMonitoring.RpcRawDataMonitoringConf import RpcLv1RawDataValAlg
rpcLV1RawDataValAlg = RpcLv1RawDataValAlg(name='rpcLV1RawDataValAlg'             ,
                                          RpcLv1File        = False              ,
                                          RpcLv1Hist        = False              ,
                                          RpcLv1Prof        = False              ,
                                          RpcLv1ReduceNbins = 2                  ,
					  OutputLevel       = OutputMessageLevel ,
                                          LastEvent         = MuonDQAFlags.EvtMax,
					  doCoolDB          = False               )
#ToolSvc += rpcLV1RawDataValAlg
rpcLv1RawMonMan.AthenaMonTools += [ rpcLV1RawDataValAlg ]
topSequence += rpcLv1RawMonMan
printfunc (rpcLv1RawMonMan)


############# RpcLv1RawDataSectorLogic #############
rpcLv1SLRawMonMan = AthenaMonManager(name="RpcLv1SLRawMonManager",
                                     FileKey             = DQMonFlags.monManFileKey(),
                                     Environment         = DQMonFlags.monManEnvironment(),
                                     OutputLevel         = muonOutputLevel)
from RpcRawDataMonitoring.RpcRawDataMonitoringConf import RpcLv1RawDataSectorLogic
rpcLV1RawDataSectorLogic = RpcLv1RawDataSectorLogic(name='rpcLV1RawDataSectorLogic',OutputLevel = OutputMessageLevel)
if globalflags.DataSource() != 'data':
    rpcLV1RawDataSectorLogic.isMC = True
#ToolSvc += rpcLV1RawDataSectorLogic
rpcLv1SLRawMonMan.AthenaMonTools += [ rpcLV1RawDataSectorLogic ]
topSequence += rpcLv1SLRawMonMan
printfunc (rpcLv1SLRawMonMan)
