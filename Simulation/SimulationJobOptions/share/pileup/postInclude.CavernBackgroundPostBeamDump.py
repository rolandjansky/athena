#########################################################
#
# SimulationJobOptions/postInclude.CavernBackgroundPostBeamDump.py
# Valeria Bartsch
# John Chapman
#
# Allow the central bunch crossing to be fixed at an empty
# BCID.
#
# Increase the time-window for pile-up to [-800,+800]ns for
# all MuonSpectrometer sub-detectors.
#
# This job option should be added via the postInclude
# command line argument.
#
#########################################################

from AthenaCommon.AppMgr import ServiceMgr
if hasattr(ServiceMgr, 'FixedArrayBM'):
    ServiceMgr.FixedArrayBM.AllowEmptyT0BunchCrossing = True
else:
    raise AttributeError("This fragment should only be used in jobs using FixeArrayBM!")

if DetFlags.digitize.CSC_on():
    if DetFlags.pileup.CSC_on():
        #changing the range of the CSC
        from AthenaCommon.AppMgr import ServiceMgr
        from PileUpComps.PileUpCompsConf import PileUpXingFolder
        cscxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['CscRange']
        cscxingFolder.FirstXing = -800
        cscxingFolder.LastXing = 800

if DetFlags.digitize.MDT_on():
    if DetFlags.pileup.MDT_on():
        #changing the range of the MDT
        from AthenaCommon.AppMgr import ServiceMgr
        from PileUpComps.PileUpCompsConf import PileUpXingFolder
        MDTxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['MdtRange']
        MDTxingFolder.FirstXing = -800
        MDTxingFolder.LastXing = 800

if DetFlags.digitize.RPC_on():
    if DetFlags.pileup.RPC_on():
        #changing the range of the RPC
        from AthenaCommon.AppMgr import ServiceMgr
        from PileUpComps.PileUpCompsConf import PileUpXingFolder
        rpcxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['RpcRange']
        rpcxingFolder.FirstXing = -800
        rpcxingFolder.LastXing = 800

if DetFlags.digitize.TGC_on():
    if DetFlags.pileup.TGC_on():
        #changing the range of the TGC
        from AthenaCommon.AppMgr import ServiceMgr
        from PileUpComps.PileUpCompsConf import PileUpXingFolder
        tgcxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['TgcRange']
        tgcxingFolder.FirstXing = -800
        tgcxingFolder.LastXing = 800

#svcMgr.MessageSvc.OutputLevel=VERBOSE
