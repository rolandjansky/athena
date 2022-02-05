# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
conddb.addOverride("/MDT/RTBLOB","MDTRT_Sim-R3SYM-01")
conddb.addOverride("/MDT/T0BLOB","MDTT0_Sim-R3SYM-01")
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
RpcRdoToRpcPrepData = RpcRdoToRpcPrepData()
RpcRdoToRpcPrepData.PrintPrepData = False
topSequence+=RpcRdoToRpcPrepData

from MuonSimHitToPrdTest.MuonSimHitToPrdTestConf import  RPC_SimHitToPrdCBNTAlgo
RPC_SimHitToPrdCBNTAlgo = RPC_SimHitToPrdCBNTAlgo()
topSequence += RPC_SimHitToPrdCBNTAlgo
RPC_SimHitToPrdCBNTAlgo.OutputLevel=INFO
RPC_SimHitToPrdCBNTAlgo.doRPCRDO=True
RPC_SimHitToPrdCBNTAlgo.doRPCPrep=True
theApp.HistogramPersistency = "ROOT"
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.NTupleSvc.Output = [ "FILE DATAFILE='RPCntuple.root' OPT='NEW'" ]


ServiceMgr.MessageSvc.defaultLimit=99999999
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"