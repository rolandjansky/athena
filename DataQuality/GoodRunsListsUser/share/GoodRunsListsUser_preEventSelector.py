###########################################################################################
## IMPORTANT: Configuring the goodrunslist selector as preevent selector tool
##            does currently not work in combination with the LumiBlockMetaDataTool
##            USE WITH CAUTION!
###########################################################################################

###########################################################################################
## Configure the goodrunslist selector tool
###########################################################################################

from GoodRunsLists.GoodRunsListsConf import *
GoodRunsTool = GoodRunsListSelectorTool('GoodRunsTool')
GoodRunsTool.GoodRunsListVec  = [ 'grl_topmix_mu.xml', 'grl_topmix_e.xml' ]
GoodRunsTool.BlackRunsListVec = [ 'blacklist.xml' ]
GoodRunsTool.RejectBlackRunsInEventSelector = True  ## False by default
GoodRunsTool.EventSelectorMode = True
#GoodRunsTool.OutputLevel = DEBUG

###########################################################################################
## Add GRL selector tool to pre-eventselector
###########################################################################################

## Klugde: eventselector currently needs private tool, will become public very soon!
## Public version is used by eg. LumiBlockMetaDataTool
## add the (private version of) grl selector tool to the eventselector
AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.PreSelectTools += [ GoodRunsTool ]
#AthenaEventLoopMgr.OutputLevel = WARNING

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr += AthenaEventLoopMgr

