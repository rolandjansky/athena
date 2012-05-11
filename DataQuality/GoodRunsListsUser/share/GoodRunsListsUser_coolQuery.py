###########################################################################################
## IMPORTANT: Configuring the goodrunslist selector tool to work from COOL
##            does not work in combination with the LumiBlockMetaDataTool
##            USE WITH CAUTION!
###########################################################################################

from AthenaCommon.AppMgr import ServiceMgr
from PoolSvc.PoolSvcConf import PoolSvc
ServiceMgr+=PoolSvc(SortReplicas=True)
from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
ServiceMgr+=DBReplicaSvc(UseCOOLSQLite=False)
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')
#globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-SIM-00-00-03')

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-03')
include("RecExCond/AllDet_detDescr.py")

include("DetectorStatus/DetStatusSvc_CondDB.py")
conddb.addOverride('/GLOBAL/DETSTATUS/LBSUMM','DetStatusLBSumm-TopMix1')

###########################################################################################
## Configure the goodrunslist selector tool to work from COOL
###########################################################################################

from GoodRunsLists.GoodRunsListsConf import GoodRunsListSelectorTool
from GoodRunsLists.TopMixConfig import TopMixConfig
GoodRunsTool = GoodRunsListSelectorTool('GoodRunsTool')
dqflagsconfig = TopMixConfig()
GoodRunsTool.DQFlagsQueryVec = dqflagsconfig.getparseddqflags()
GoodRunsTool.DQFlagsFromCOOL = True
GoodRunsTool.RunRangeExpressionCOOL = '1' # 'run>=108175 || run>=10000 && run<=100000' # ala TFormula
#GoodRunsTool.VerboseDetStatus = True
#GoodRunsTool.OutputLevel = DEBUG

###########################################################################################
## Add GRL selector tool to pre-eventselector
###########################################################################################

## Klugde: eventselector currently needs private tool, will become public very soon!
## Public version is used by eg. LumiBlockMetaDataTool
## add the (private version of) grl selector tool to the eventselector
AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.PreSelectTools += [ GoodRunsTool ]
AthenaEventLoopMgr.OutputLevel = WARNING

