#########################################################
#
# SimulationJobOptions/postOptions.Calo_InTimeOnlyPileUp.py
# John Chapman
#
# Only do pile-up in BC0 for the LAr and Tile sub-detectors.
#
# This job option should be added via the postInclude
# command line argument. 
#
#########################################################

if DetFlags.digitize.LAr_on():
    if DetFlags.pileup.LAr_on():
        #changing the range of the LAr EM
        from AthenaCommon.AppMgr import ServiceMgr
        from PileUpComps.PileUpCompsConf import PileUpXingFolder
        laremxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['LArRangeEM']
        laremxingFolder.FirstXing = 0
        laremxingFolder.LastXing = 0
 
        #changing the range of the LAr HEC
        from AthenaCommon.AppMgr import ServiceMgr
        from PileUpComps.PileUpCompsConf import PileUpXingFolder
        larhecxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['LArRangeHEC']
        larhecxingFolder.FirstXing = 0
        larhecxingFolder.LastXing = 0
        
        #changing the range of the LAr FCAL
        from AthenaCommon.AppMgr import ServiceMgr
        from PileUpComps.PileUpCompsConf import PileUpXingFolder
        larfcalxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['LArRangeFCAL']
        larfcalxingFolder.FirstXing = 0
        larfcalxingFolder.LastXing = 0

if DetFlags.digitize.Tile_on():
    if DetFlags.pileup.Tile_on():
        #changing the range of the Tile
        from AthenaCommon.AppMgr import ServiceMgr
        from PileUpComps.PileUpCompsConf import PileUpXingFolder
        tilexingFolder = ServiceMgr.PileUpMergeSvc.Intervals['TileRange']
        tilexingFolder.FirstXing = 0
        tilexingFolder.LastXing = 0
