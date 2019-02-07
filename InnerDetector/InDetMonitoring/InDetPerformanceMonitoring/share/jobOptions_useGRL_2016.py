from GaudiSvc.GaudiSvcConf import THistSvc

# add LumiBlockMetaDataTool to ToolSvc and configure
from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )
LumiBlockMetaDataTool.calcLumi = True # False by default

# add ToolSvc.LumiBlockMetaDataTool to MetaDataSvc
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import MetaDataSvc
svcMgr += MetaDataSvc( "MetaDataSvc" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool ]

# Configure the goodrunslist selector tool
from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool()

#GoodRunsListSelectorTool.GoodRunsListVec = [ '/project/atlas/users/pbutti/Maps_v2/run/GRLs/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good.xml' ]
#GoodRunsListSelectorTool.GoodRunsListVec = [ '$TestArea/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml' ]
GoodRunsListSelectorTool.GoodRunsListVec = [ '$TestArea/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml' ]
#GoodRunsListSelectorTool.GoodRunsListVec = [ './grls/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml' ]



## This Athena job consists of algorithms that loop over events;
## here, the (default) top sequence is used:
#from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
#job = AlgSequence()
#seq = AthSequencer("AthFilterSeq")

## AthFilterSeq is always executed before the top sequence, and is configured such that
## any follow-up sequence (eg. top sequence) is not executed in case GRLTriggerAlg1 does
## not pass the event
## In short, the sequence AthFilterSeq makes sure that all algs in the job sequence
## are skipped when an event gets rejects
from GoodRunsListsUser.GoodRunsListsUserConf import *
seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
seq.GRLTriggerAlg1.GoodRunsListArray = ['PHYS_StandardGRL_All_Good_25ns']        ## pick up correct name from inside xml file!
#seq.GRLTriggerAlg1.TriggerSelectionRegistration = 'L1_MBTS_1' ## set this to your favorite trigger, eg. L1_MBTS_1_1

## Add the ntuple dumper to the top sequence, as usual
## DummyDumperAlg1 is run in the top sequence, but is not executed in case GRLTriggerAlg1 rejects the event.
job += DummyDumperAlg('DummyDumperAlg1')
# job.DummyDumperAlg1.RootFileName = 'selection1.root'
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["new DATAFILE='selection1.root' TYP='ROOT' OPT='RECREATE'"];
job.DummyDumperAlg1.GRLNameVec = [ 'LumiBlocks_GoodDQ0', 'IncompleteLumiBlocks_GoodDQ0' ]
