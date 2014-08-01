#########################################################
#
# SimulationJobOptions/postOptions.InDet_InTimeOnlyPileUp.py
# John Chapman
#
# Only do pile-up in BC0 for the BCM, Pixel, SCT and TRT
# sub-detectors.
#
# This job option should be added via the postInclude
# command line argument.
#
#########################################################
from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.CfgGetter import getPublicTool

if DetFlags.digitize.BCM_on():
    if DetFlags.pileup.BCM_on():
        if digitizationFlags.doXingByXingPileUp():
            bcm = getPublicTool("BCM_DigitizationTool")
        else:
            #changing the range of the BCM
            from AthenaCommon.AppMgr import ServiceMgr
            from PileUpComps.PileUpCompsConf import PileUpXingFolder
            bcm = ServiceMgr.PileUpMergeSvc.Intervals['BCM_Range']
        bcm.FirstXing = 0
        bcm.LastXing = 0

if DetFlags.digitize.pixel_on():
    if DetFlags.pileup.pixel_on():
        if digitizationFlags.doXingByXingPileUp():
            pixel = getPublicTool("PixelDigitizationTool")
        else:
            #changing the range of the Pixels
            from AthenaCommon.AppMgr import ServiceMgr
            from PileUpComps.PileUpCompsConf import PileUpXingFolder
            pixel = ServiceMgr.PileUpMergeSvc.Intervals['PixelRange']
        pixel.FirstXing = 0
        pixel.LastXing = 0

if DetFlags.digitize.SCT_on():
    if DetFlags.pileup.SCT_on():
        if digitizationFlags.doXingByXingPileUp():
            sct = getPublicTool("SCT_DigitizationTool")
        else:
            #changing the range of the SCT
            from AthenaCommon.AppMgr import ServiceMgr
            from PileUpComps.PileUpCompsConf import PileUpXingFolder
            sct = ServiceMgr.PileUpMergeSvc.Intervals['SiliconRange']
        sct.FirstXing = 0
        sct.LastXing = 0

if DetFlags.digitize.TRT_on():
    if DetFlags.pileup.TRT_on():
        if digitizationFlags.doXingByXingPileUp():
            trt = getPublicTool("TRTDigitizationTool")
        else:
            #changing the range of the TRT
            from AthenaCommon.AppMgr import ServiceMgr
            from PileUpComps.PileUpCompsConf import PileUpXingFolder
            trt = ServiceMgr.PileUpMergeSvc.Intervals['TRTRange']
        trt.FirstXing = 0
        trt.LastXing = 0
