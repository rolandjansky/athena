# add LumiBlockMetaDataTool to ToolSvc and configure
from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )
LumiBlockMetaDataTool.OutputLevel = INFO

# add ToolSvc.LumiBlockMetaDataTool to MetaDataSvc
from AthenaServices.AthenaServicesConf import MetaDataSvc
svcMgr += MetaDataSvc( "MetaDataSvc" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool ]

# Configure the goodrunslist selector tool
from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool() 
GoodRunsListSelectorTool.OutputLevel = INFO
GoodRunsListSelectorTool.GoodRunsListVec = [ 'grl_topmix_mu.xml' ] # or grl_topmix_e.xml 
GoodRunsListSelectorTool.PassThrough = False
