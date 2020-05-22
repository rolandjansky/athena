# add LumiBlockMetaDataTool to ToolSvc and configure
from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )

# add ToolSvc.LumiBlockMetaDataTool to MetaDataSvc
from AthenaServices.AthenaServicesConf import MetaDataSvc
svcMgr += MetaDataSvc( "MetaDataSvc" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool ]

# Configure the goodrunslist selector tool
from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool() 
GoodRunsListSelectorTool.GoodRunsListVec = [ 'grl_topmix_mu.xml', 'grl_topmix_e.xml' ]
GoodRunsListSelectorTool.BlackRunsListVec = [ 'blacklist.xml' ]

## This Athena job consists of algorithms that loop over events;
## here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()
job += AthSequencer("ModSequence1")

## GRL selector, dummy ntuple dumper
from GoodRunsListsUser.GoodRunsListsUserConf import *
job.ModSequence1 += GRLTriggerSelectorAlg('GRLTriggerAlg1')
job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray = ['grl_topmix_mu']
job.ModSequence1.GRLTriggerAlg1.BlackRunsListArray = ['blacklist']
job.ModSequence1.GRLTriggerAlg1.TriggerSelectionRegistration = 'EF_mu20'

job.ModSequence1 += DummyDumperAlg('DummyDumperAlg1')
# job.ModSequence1.DummyDumperAlg1.RootFileName = 'selection1.root'
THistSvc.Output =  {"new DATAFILE='selection1.root' TYP='ROOT' OPT='NEW'"};
job.ModSequence1.DummyDumperAlg1.GRLNameVec = [ 'LumiBlocks_GoodDQ0', 'IncompleteLumiBlocks_GoodDQ0' ]
