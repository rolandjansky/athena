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
if DetFlags.digitize.pixel_on():
    outStream.ItemList+=["SiHitCollection#PixelHits"]
    #disable some Pixel stuff
    pixelTool = ServiceMgr.GeoModelSvc.DetectorTools['PixelDetectorTool']
    pixelTool.Alignable = False
    from Digitization.DigitizationFlags import digitizationFlags
    pixeldigi = None
    from PyJobTransforms.trfUtils import releaseIsOlderThan
    if releaseIsOlderThan(19,2):
        if not digitizationFlags.doXingByXingPileUp() and hasattr(job, 'PixelDigitization'):
            if hasattr( job.PixelDigitization, 'DigitizationTool'):
                pixeldigi = job.PixelDigitization.DigitizationTool
            else:
                pixeldigi = job.PixelDigitization
        elif hasattr( ToolSvc, 'PixelDigitizationTool'):
            pixeldigi = ToolSvc.PixelDigitizationTool
        else:
            for alg in job:
                if hasattr(alg, 'PileUpTools'):
                    pixeldigi = alg.PileUpTools[ "PixelLightDigitizationTool" ]
                    break
                pass
    else:
        pixeldigi = getPublicTool("PixelLightDigitizationTool")
    if None == pixeldigi:
        raise AttributeError("PixelDigitization(Tool) not found.")

if DetFlags.digitize.SCT_on():
    outStream.ItemList+=["SiHitCollection#SCT_Hits"]
    if not hasattr( ToolSvc, 'SCT_FrontEnd'):
        from SCT_Digitization.SCT_DigitizationConf import SCT_FrontEnd
        ToolSvc += SCT_FrontEnd("SCT_FrontEnd")
    theSCT_FrontEnd = ToolSvc.SCT_FrontEnd
    theSCT_FrontEnd.MaxStripsPerSide = 1280
    theSCT_FrontEnd.UseCalibData = False

    if not digitizationFlags.doXingByXingPileUp():
        if DetFlags.pileup.SCT_on():
            #changing the range of the SCT
            from AthenaCommon.AppMgr import ServiceMgr
            from PileUpComps.PileUpCompsConf import PileUpXingFolder
            sctxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['SiliconRange']
            sctxingFolder.FirstXing = -50
            sctxingFolder.LastXing = 25
