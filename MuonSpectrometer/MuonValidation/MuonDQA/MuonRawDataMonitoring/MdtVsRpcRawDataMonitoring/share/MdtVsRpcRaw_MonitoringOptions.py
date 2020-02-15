#printfunc ('\n******************************************************************************************************\n')
#printfunc ("   ******** Welcome to the Offline ESD MuonRawDataMonitoring/MstVsRpcRawDataMonitoring package. ******** "  )
#printfunc ("   ******** Documentation may be found at:******** "  )
#printfunc ("   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MuonOfflineDQA ***** "  )
#printfunc ('\n******************************************************************************************************\n')

if not 'MuonDQAFlags' in dir():
    printfunc ("MuonDQAFlags.py: MuonDQAFlags not yet imported - I import them now")
    from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags
    
MdtRpcRawMonMan = AthenaMonManager(name="MdtVsRpcRawMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),
                                   Environment         = DQMonFlags.monManEnvironment(),
                                   OutputLevel         = muonOutputLevel) 
############# MdtVsRpcRawDataValAlg #############
from MdtVsRpcRawDataMonitoring.MdtVsRpcRawDataMonitoringConf import MdtVsRpcRawDataValAlg 
MdtVsRpcRawDataValAlg = MdtVsRpcRawDataValAlg(name='MdtVsRpcRawDataValAlg',
                                              DoMdtvsRpcEsd = True,
                                              MdtvsRpcChamberHist = False, ## True,
                                              MdtvsRpcSectorHist = False, ## True
                                              MdtvsRpcReduceRpcNbins = 8,
                                              MdtvsRpcReduceMdtNbins = 8,
                                              MdtvsRpcReduceMdtTDCNbins = 10,
                                              # temporary fix RpcClusterization not used
                                              # ClusterContainer="rpcClusters",
                                              ClusterContainer="rpcClusters",
                                              RpcPrepDataContainer="RPC_Measurements",
                                              MdtPrepDataContainer="MDT_DriftCircles",
                                              LastEvent = MuonDQAFlags.EvtMax)
#ToolSvc += MdtVsRpcRawDataValAlg
MdtRpcRawMonMan.AthenaMonTools += [ MdtVsRpcRawDataValAlg ]
topSequence += MdtRpcRawMonMan
printfunc (MdtRpcRawMonMan)
##################################################################
