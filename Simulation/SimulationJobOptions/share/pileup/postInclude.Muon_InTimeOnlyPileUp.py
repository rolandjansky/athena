#########################################################
#
# SimulationJobOptions/postOptions.InDet_InTimeOnlyPileUp.py
# John Chapman
#
# Only do pile-up in BC0 for the CSC, MDT, RPC and TGC
# sub-detectors.
#
# This job option should be added via the postInclude
# command line argument. 
#
#########################################################

if DetFlags.digitize.CSC_on():
    if DetFlags.pileup.CSC_on():
        #changing the range of the CSC
        from AthenaCommon.AppMgr import ServiceMgr
        from PileUpComps.PileUpCompsConf import PileUpXingFolder
        cscxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['CscRange']
        cscxingFolder.FirstXing = 0
        cscxingFolder.LastXing = 0
 
if DetFlags.digitize.MDT_on():
    if DetFlags.pileup.MDT_on():
        #changing the range of the MDT
        from AthenaCommon.AppMgr import ServiceMgr
        from PileUpComps.PileUpCompsConf import PileUpXingFolder
        MDTxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['MdtRange']
        MDTxingFolder.FirstXing = 0
        MDTxingFolder.LastXing = 0
        
if DetFlags.digitize.RPC_on():
    if DetFlags.pileup.RPC_on():
        #changing the range of the RPC
        from AthenaCommon.AppMgr import ServiceMgr
        from PileUpComps.PileUpCompsConf import PileUpXingFolder
        rpcxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['RpcRange']
        rpcxingFolder.FirstXing = 0
        rpcxingFolder.LastXing = 0

if DetFlags.digitize.TGC_on():
    if DetFlags.pileup.TGC_on():
        #changing the range of the TGC
        from AthenaCommon.AppMgr import ServiceMgr
        from PileUpComps.PileUpCompsConf import PileUpXingFolder
        tgcxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['TgcRange']
        tgcxingFolder.FirstXing = 0
        tgcxingFolder.LastXing = 0
