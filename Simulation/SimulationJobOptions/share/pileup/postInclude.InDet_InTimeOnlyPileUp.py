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
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

bcm = None
if DetFlags.digitize.BCM_on():
    if DetFlags.pileup.BCM_on():
        if digitizationFlags.doXingByXingPileUp():
            for alg in topSequence:
                if 'PileUpToolsAlg' in alg.name():
                    bcm = alg.PileUpTools["BCM_DigitizationTool"]
                    break
        else:
            #changing the range of the BCM
            from AthenaCommon.AppMgr import ServiceMgr
            from PileUpComps.PileUpCompsConf import PileUpXingFolder
            bcm = ServiceMgr.PileUpMergeSvc.Intervals['BCM_Range']
        if bcm is not None:
            bcm.FirstXing = 0
            bcm.LastXing = 0

pixel = None
if DetFlags.digitize.pixel_on():
    if DetFlags.pileup.pixel_on():
        if digitizationFlags.doXingByXingPileUp():
            for alg in topSequence:
                if 'PileUpToolsAlg' in alg.name():
                    pixel = alg.PileUpTools["PixelDigitizationTool"]
                    break
        else:
            #changing the range of the Pixels
            from AthenaCommon.AppMgr import ServiceMgr
            from PileUpComps.PileUpCompsConf import PileUpXingFolder
            pixel = ServiceMgr.PileUpMergeSvc.Intervals['PixelRange']
        if pixel is not None:
            pixel.FirstXing = 0
            pixel.LastXing = 0

sct = None
if DetFlags.digitize.SCT_on():
    if DetFlags.pileup.SCT_on():
        if digitizationFlags.doXingByXingPileUp():
            for alg in topSequence:
                if 'PileUpToolsAlg' in alg.name():
                    sct = alg.PileUpTools["SCT_DigitizationTool"]
                    break
        else:
            #changing the range of the SCT
            from AthenaCommon.AppMgr import ServiceMgr
            from PileUpComps.PileUpCompsConf import PileUpXingFolder
            sct = ServiceMgr.PileUpMergeSvc.Intervals['SiliconRange']
        if sct is not None:
            sct.FirstXing = 0
            sct.LastXing = 0

trt = None
if DetFlags.digitize.TRT_on():
    if DetFlags.pileup.TRT_on():
        if digitizationFlags.doXingByXingPileUp():
            for alg in topSequence:
                if 'PileUpToolsAlg' in alg.name():
                    trt = alg.PileUpTools["TRTDigitizationTool"]
                    break
        else:
            #changing the range of the TRT
            from AthenaCommon.AppMgr import ServiceMgr
            from PileUpComps.PileUpCompsConf import PileUpXingFolder
            trt = ServiceMgr.PileUpMergeSvc.Intervals['TRTRange']
        if trt is not None:
            trt.FirstXing = 0
            trt.LastXing = 0
