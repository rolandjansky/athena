from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
theFastShowerCellBuilderTool=FastShowerCellBuilderTool()

theFastShowerCellBuilderTool.OutputLevel=DEBUG
from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()
svcMgr.MessageSvc.debugLimit   = 100000000 
svcMgr.MessageSvc.infoLimit   = 100000000 
