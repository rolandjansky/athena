if hasattr(topSequence, 'StreamRDO'):
    outStream = topSequence.StreamRDO
else:
    ## temporary back-compatibility
    outStreams = AlgSequence( "Streams" )
    if hasattr(outStreams, 'StreamRDO'):
        outStream = outStreams.StreamRDO
    else:
        raise AttributeError("AthenaPoolOutputStream not found")
      
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.CfgGetter import getService, getPublicTool

ServiceMgr.PixelCalibSvc.DisableDB     = True

if DetFlags.digitize.SCT_on():
    outStream.ItemList+=["SiHitCollection#SCT_Hits"]
    if not hasattr( ToolSvc, 'SCT_FrontEnd'):
        from SCT_Digitization.SCT_DigitizationConf import SCT_FrontEnd
        ToolSvc += SCT_FrontEnd("SCT_FrontEnd")
    getPublicTool("SCT_DigitizationTool").FrontEnd.UseCalibData = False
    getPublicTool("SCT_DigitizationTool").FrontEnd.MaxStripsPerSide = 1280

    if not digitizationFlags.doXingByXingPileUp():
        if DetFlags.pileup.SCT_on():
            #changing the range of the SCT
            from AthenaCommon.AppMgr import ServiceMgr
            from PileUpComps.PileUpCompsConf import PileUpXingFolder
            sctxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['SiliconRange']
            sctxingFolder.FirstXing = -50
            sctxingFolder.LastXing = 25
