#**************************************************************
#
# TrigServicesEventLoopMgr configuration file
#
#==============================================================
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *

# make the HltEventLoopMgr service available
svcMgr.HltEventLoopMgr = theApp.service( "HltEventLoopMgr" )     # already instantiated
HltEventLoopMgr = svcMgr.HltEventLoopMgr

# configure here Level-1 CTP ROB identifier which is used in HLT
HltEventLoopMgr.Lvl1CTPROBid = 0x770001

# request that events with invalid or missing CTP ROBs are skipped by the HltEventLoopMgr
# (default: don't skip these events)
HltEventLoopMgr.Lvl1CTPROBcheck = TRUE

#
# name of the HLT Result object in StoreGate
#
HltEventLoopMgr.HltResultName = "HLTResult_HLT"

#
# properties for the HLT result size histogram
# --> set upper edge of histogram to maximum allowed number of words in HLT
#
HltEventLoopMgr.histHltResultSize=("HltResultSize",0,125000,100)

#
# Configuration of EDM size monitoring plots
#
try:
    from TrigEDMConfig.TriggerEDM import EDMDetails,getTypeAndKey,keyToLabel
    
    # TODO update to use import TriggerHLTList directly
    from TrigEDMConfig.TriggerEDM import TriggerL2List,TriggerEFList
    TriggerHLTList = list(set(TriggerL2List).union(TriggerEFList))
    
    l = []
    for item in TriggerHLTList:
        if 'BS' in item[1]:
            t,k = getTypeAndKey(item[0])
            ctype = t
            if EDMDetails[t].has_key('collection'):
                ctype = EDMDetails[t]['collection']
            l += [ctype+'__'+keyToLabel(k)]
    # add a bin for not accounted collections
    l += ['not__configured']

    HltEventLoopMgr.HltEDMCollectionNames = l
    number_of_collections=len( HltEventLoopMgr.HltEDMCollectionNames )
    HltEventLoopMgr.histHltEdmSizes=("HltEDMSizes",0., 15000., number_of_collections)
except ImportError,e:
    print " +----------------------------------------------------------------------+ "
    print " | No initial configuration for EDM monitoring plots will be available! | "
    print " +----------------------------------------------------------------------+ "
    print e
#==============================================================
#
# End of TrigServicesEventLoopMgr configuration file
#
#**************************************************************
