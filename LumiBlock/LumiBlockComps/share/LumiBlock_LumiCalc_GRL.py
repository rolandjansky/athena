# add LumiBlockMetaDataTool to ToolSvc and configure
from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )
LumiBlockMetaDataTool.calcLumi = True # False by default
LumiBlockMetaDataTool.storeXMLFiles = True
LumiBlockMetaDataTool.applyDQCuts = True
LumiBlockMetaDataTool.OutputLevel = INFO

# add ToolSvc.LumiBlockMetaDataTool to MetaDataSvc
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import MetaDataSvc
svcMgr += MetaDataSvc( "MetaDataSvc" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool ]

# Configure the goodrunslist selector tool
from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool() 
GoodRunsListSelectorTool.OutputLevel = INFO
GoodRunsListSelectorTool.GoodRunsListVec = [ 'grl_topmix_mu.xml' ] # or grl_topmix_e.xml 
GoodRunsListSelectorTool.PassThrough = False

# add LumiCalcSvc to ServiceMgr and configure
from LumiBlockComps.LumiBlockCompsConf import LumiCalcSvc
LumiCalcSvc = LumiCalcSvc()
LumiCalcSvc.Triggers = ["EF_mu20"]
LumiCalcSvc.UseMC = True
LumiCalcSvc.LBCollNames = ["LumiBlocks", "IncompleteLumiBlocks"]
LumiCalcSvc.Verbose = False
svcMgr += LumiCalcSvc

