# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#####################################
##     Test PileUpTool methods     ##
#####################################

def getTestPileUpTool(name="TestPileUpTool", **kwargs):
    from AthenaCommon.Constants import VERBOSE
    kwargs.setdefault('OutputLevel', VERBOSE )
    kwargs.setdefault('FirstXing', -300 )
    kwargs.setdefault('LastXing', +300 )
    return CfgMgr.TestPileUpTool(name, **kwargs)

def getTestFilterPileUpTool(name="TestFilterPileUpTool", **kwargs):
    from AthenaCommon.Constants import INFO
    kwargs.setdefault('OutputLevel', INFO )
    kwargs.setdefault('DoFiltering', True )
    return getTestPileUpTool(name, **kwargs)

#####################################
## PileUpToolsList filling methods ##
#####################################

def getStandardTruthPileUpTools():
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.pileup.Truth_on():
        from Digitization.DigitizationFlags import digitizationFlags
        if 'NewMerge' in digitizationFlags.experimentalDigi():
            if not athenaCommonFlags.DoFullChain():
                PileUpToolsList += [ "NewMergeMcEventCollTool_Signal" ]
            if digitizationFlags.doLowPtMinBias.get_Value():
                PileUpToolsList += [ "NewMergeMcEventCollTool_MinBias" ]
            if digitizationFlags.doHighPtMinBias.get_Value():
                PileUpToolsList += [ "NewMergeMcEventCollTool_HighPtMinBias" ]
            if digitizationFlags.doCavern.get_Value():
                PileUpToolsList += [ "NewMergeMcEventCollTool_Cavern" ]
            if digitizationFlags.doBeamGas.get_Value() or digitizationFlags.doBeamHalo.get_Value():
                PileUpToolsList += [ "NewMergeMcEventCollTool_HaloGas" ]
        else:
            PileUpToolsList += [ "MergeMcEventCollTool" ]
        PileUpToolsList += [ "MergeTruthJetsTool" ]
        if DetFlags.writeRDOPool.Muon_on(): #possibly this should be digitize.Muon_on()
            PileUpToolsList += [ "MergeTrackRecordCollTool" ]
        if DetFlags.writeRDOPool.Calo_on(): #possibly this should be digitize.Calo_on()
            PileUpToolsList += [ "MergeCalibHitsTool" ]
    return PileUpToolsList

def getStandardSignalOnlyTruthPileUpTools():
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.pileup.Truth_on():
        from Digitization.DigitizationFlags import digitizationFlags
        if 'NewMerge' in digitizationFlags.experimentalDigi():
            if not athenaCommonFlags.DoFullChain():
                PileUpToolsList += [ "NewMergeMcEventCollTool_Signal" ]
        else:
            PileUpToolsList += [ "SignalOnlyMcEventCollTool" ]
        PileUpToolsList += [ "MergeTruthJetsTool" ]
        if not athenaCommonFlags.DoFullChain() and DetFlags.writeRDOPool.Muon_on(): #possibly this should be digitize.Muon_on()
            PileUpToolsList += [ "MergeTrackRecordCollTool" ]
        if DetFlags.writeRDOPool.Calo_on(): #possibly this should be digitize.Calo_on()
            PileUpToolsList += [ "MergeCalibHitsTool" ]
    return PileUpToolsList

def getStandardInTimeOnlyTruthPileUpTools():
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.pileup.Truth_on():
        from Digitization.DigitizationFlags import digitizationFlags
        if 'NewMerge' in digitizationFlags.experimentalDigi():
            if not athenaCommonFlags.DoFullChain():
                PileUpToolsList += [ "NewMergeMcEventCollTool_Signal" ]
            if digitizationFlags.doLowPtMinBias.get_Value():
                PileUpToolsList += [ "InTimeOnlyNewMergeMcEventCollTool_MinBias" ]
            if digitizationFlags.doHighPtMinBias.get_Value():
                PileUpToolsList += [ "InTimeOnlyNewMergeMcEventCollTool_HighPtMinBias" ]
            if digitizationFlags.doCavern.get_Value():
                PileUpToolsList += [ "InTimeOnlyNewMergeMcEventCollTool_Cavern" ]
            if digitizationFlags.doBeamGas.get_Value() or digitizationFlags.doBeamHalo.get_Value():
                PileUpToolsList += [ "InTimeOnlyNewMergeMcEventCollTool_HaloGas" ]
        else:
            PileUpToolsList += [ "InTimeOnlyMcEventCollTool" ]
        PileUpToolsList += [ "MergeTruthJetsTool" ]
        if not athenaCommonFlags.DoFullChain() and DetFlags.writeRDOPool.Muon_on(): #possibly this should be digitize.Muon_on()
            PileUpToolsList += [ "MergeTrackRecordCollTool" ]
        if DetFlags.writeRDOPool.Calo_on(): #possibly this should be digitize.Calo_on()
            PileUpToolsList += [ "MergeCalibHitsTool" ]
    return PileUpToolsList

def getStandardForwardPileUpTools():
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.digitize.Lucid_on():
        PileUpToolsList += [ "LUCID_PileUpTool" ]
    if DetFlags.digitize.ZDC_on():
        PileUpToolsList += [ "ZDC_PileUpTool" ]
    if DetFlags.digitize.ALFA_on():
        PileUpToolsList += [ "ALFA_PileUpTool" ]
    if hasattr(DetFlags.digitize, 'AFP_on') and DetFlags.digitize.AFP_on():
        PileUpToolsList += [ "AFP_PileUpTool" ]
    return PileUpToolsList

def getStandardInDetPileUpTools():
    from AthenaCommon.DetFlags import DetFlags
    from Digitization.DigitizationFlags import digitizationFlags
    PileUpToolsList = []
    if DetFlags.digitize.BCM_on():
        PileUpToolsList += [ "BCM_DigitizationTool" ]
    if DetFlags.digitize.pixel_on():
        if 'doFastPixelDigi' in digitizationFlags.experimentalDigi():
            PileUpToolsList += [ "PixelFastDigitizationTool" ]
        elif 'doLightPixelDigi' in digitizationFlags.experimentalDigi():
            PileUpToolsList += [ "PixelLightDigitizationTool" ]
        elif 'doSmearedPixelDigi' in digitizationFlags.experimentalDigi():
            PileUpToolsList += [ "PixelSmearedDigitizationTool" ]
        elif digitizationFlags.doRadiationDamage():
            PileUpToolsList += [ "PixelRadDamDigitizationTool" ]
        else:
            PileUpToolsList += [ "PixelDigitizationTool" ]
    if DetFlags.digitize.SCT_on():
        if 'doFastSCT_Digi' in digitizationFlags.experimentalDigi():
            PileUpToolsList += [ "SCT_FastDigitizationTool" ]
        else:
            PileUpToolsList += [ "SCT_DigitizationTool" ]
    if DetFlags.digitize.TRT_on():
        if 'doFastTRT_Digi' in digitizationFlags.experimentalDigi():
            PileUpToolsList += [ "TRTFastDigitizationTool" ]
        else:
            PileUpToolsList += [ "TRTDigitizationTool" ]
    return PileUpToolsList

def getGeantinoTruthInDetPileUpTools():
    from AthenaCommon.DetFlags import DetFlags
    from Digitization.DigitizationFlags import digitizationFlags
    PileUpToolsList = []
    unsupportedKeys = ['doFastPixelDigi', 'doLightPixelDigi', 'doSmearedPixelDigi', 'doFastSCT_Digi', 'doFastTRT_Digi']
    if not set(unsupportedKeys).isdisjoint(set(digitizationFlags.experimentalDigi())):
        print "DigiAlgConfig.py ERROR The following digitizationFlags.experimentalDigi settings are not supported when digiSteeringConf is set to", digitizationFlags.digitSteeringConf.get_Value(), ": ", str(unsupportedKeys), " and will be ignored."
    if DetFlags.digitize.BCM_on():
        PileUpToolsList += [ "BCM_DigitizationTool" ]
    if DetFlags.digitize.pixel_on():
        PileUpToolsList += [ "PixelGeantinoTruthDigitizationTool" ]
    if DetFlags.digitize.SCT_on():
        PileUpToolsList += [ "SCT_GeantinoTruthDigitizationTool" ]
    if DetFlags.digitize.TRT_on():
        PileUpToolsList += [ "TRTGeantinoTruthDigitizationTool" ]
    return PileUpToolsList

def getFastInDetPileUpTools():
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.digitize.BCM_on():
        PileUpToolsList += [ "BCM_DigitizationTool" ]
    if DetFlags.digitize.pixel_on():
        PileUpToolsList += [ "PixelFastDigitizationTool" ]
    if DetFlags.digitize.SCT_on():
        PileUpToolsList += [ "SCT_FastDigitizationTool" ]
    if DetFlags.digitize.TRT_on():
        PileUpToolsList += [ "TRTFastDigitizationTool" ]
    return PileUpToolsList

def getSplitInDetPileUpTools():
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.digitize.BCM_on():
        PileUpToolsList += [ "BCM_DigitizationTool" ]
    if DetFlags.digitize.pixel_on():
        PileUpToolsList += [ "PixelDigitizationToolHS" ]
        PileUpToolsList += [ "PixelDigitizationToolPU" ]
    if DetFlags.digitize.SCT_on():
        PileUpToolsList += [ "SCT_DigitizationToolHS" ]
        PileUpToolsList += [ "SCT_DigitizationToolPU" ]
    if DetFlags.digitize.TRT_on():
        PileUpToolsList += [ "TRTDigitizationToolHS" ]
        PileUpToolsList += [ "TRTDigitizationToolPU" ]
    return PileUpToolsList

def getSplitSFInDetPileUpTools():
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.digitize.BCM_on():
        PileUpToolsList += [ "BCM_DigitizationTool" ]
    if DetFlags.digitize.pixel_on():
        PileUpToolsList += [ "PixelDigitizationToolHS" ]
        PileUpToolsList += [ "PixelFastDigitizationToolPU" ]
    if DetFlags.digitize.SCT_on():
        PileUpToolsList += [ "SCT_DigitizationToolHS" ]
        PileUpToolsList += [ "SCT_FastDigitizationToolPU" ]
    if DetFlags.digitize.TRT_on():
        PileUpToolsList += [ "TRTDigitizationToolHS" ]
        PileUpToolsList += [ "TRTFastDigitizationToolPU" ]
    return PileUpToolsList

def getSplitNoMergeInDetPileUpTools():
    """ This is for testing the fast chain with full digitization in split PU mode without
merging pileup
    """
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.digitize.BCM_on():
        PileUpToolsList += [ "BCM_DigitizationTool" ]
    if DetFlags.digitize.pixel_on():
        PileUpToolsList += [ "PixelDigitizationTool" ]
        PileUpToolsList += [ "PixelDigitizationToolSplitNoMergePU" ]
    if DetFlags.digitize.SCT_on():
        PileUpToolsList += [ "SCT_DigitizationTool" ]
        PileUpToolsList += [ "SCT_DigitizationToolSplitNoMergePU" ]
    if DetFlags.digitize.TRT_on():
        PileUpToolsList += [ "TRTDigitizationTool" ]
        PileUpToolsList += [ "TRTDigitizationToolSplitNoMergePU" ]
    return PileUpToolsList

def getSplitNoMergeSFInDetPileUpTools():
    """ This is for testing the fast chain with full digitization in split PU mode without
merging pileup
    """
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.digitize.BCM_on():
        PileUpToolsList += [ "BCM_DigitizationTool" ]
    if DetFlags.digitize.pixel_on():
        PileUpToolsList += [ "PixelDigitizationTool" ]
        PileUpToolsList += [ "PixelFastDigitizationToolSplitNoMergePU" ]
    if DetFlags.digitize.SCT_on():
        PileUpToolsList += [ "SCT_DigitizationTool" ]
        PileUpToolsList += [ "SCT_FastDigitizationToolSplitNoMergePU" ]
    if DetFlags.digitize.TRT_on():
        PileUpToolsList += [ "TRTDigitizationTool" ]
        PileUpToolsList += [ "TRTFastDigitizationToolSplitNoMergePU" ]
    return PileUpToolsList

def getSplitNoMergeFSInDetPileUpTools():
    """ This is for testing the fast chain with full digitization in split PU mode without
merging pileup
    """
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.digitize.BCM_on():
        PileUpToolsList += [ "BCM_DigitizationTool" ]
    if DetFlags.digitize.pixel_on():
        PileUpToolsList += [ "PixelFastDigitizationTool" ]
        PileUpToolsList += [ "PixelDigitizationToolSplitNoMergePU" ]
    if DetFlags.digitize.SCT_on():
        PileUpToolsList += [ "SCT_FastDigitizationTool" ]
        PileUpToolsList += [ "SCT_DigitizationToolSplitNoMergePU" ]
    if DetFlags.digitize.TRT_on():
        PileUpToolsList += [ "TRTFastDigitizationTool" ]
        PileUpToolsList += [ "TRTDigitizationToolSplitNoMergePU" ]
    return PileUpToolsList

def getSplitNoMergeFFInDetPileUpTools():
    """ This is for testing the fast chain with full digitization in split PU mode without
merging pileup
    """
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.digitize.BCM_on():
        PileUpToolsList += [ "BCM_DigitizationTool" ]
    if DetFlags.digitize.pixel_on():
        PileUpToolsList += [ "PixelFastDigitizationTool" ]
        PileUpToolsList += [ "PixelFastDigitizationToolSplitNoMergePU" ]
    if DetFlags.digitize.SCT_on():
        PileUpToolsList += [ "SCT_FastDigitizationTool" ]
        PileUpToolsList += [ "SCT_FastDigitizationToolSplitNoMergePU" ]
    if DetFlags.digitize.TRT_on():
        PileUpToolsList += [ "TRTFastDigitizationTool" ]
        PileUpToolsList += [ "TRTFastDigitizationToolSplitNoMergePU" ]
    return PileUpToolsList

def getStandardCaloPileUpTools():
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.digitize.LAr_on():
        PileUpToolsList += [ "LArPileUpTool" ]
    if hasattr(DetFlags.digitize, 'HGTD_on') and DetFlags.digitize.HGTD_on():
        PileUpToolsList += [ "HGTDPileUpTool" ]
    if DetFlags.digitize.Tile_on():
        PileUpToolsList += [ "TileHitVecToCntTool" ]
    return PileUpToolsList

def getStandardMuonPileUpTools():
    from AthenaCommon.DetFlags import DetFlags
    PileUpToolsList = []
    if DetFlags.digitize.CSC_on():
        PileUpToolsList += [ "CscDigitizationTool" ]
    if DetFlags.digitize.MDT_on():
        PileUpToolsList += [ "MdtDigitizationTool" ]
    if DetFlags.digitize.RPC_on():
        PileUpToolsList += [ "RpcDigitizationTool" ]
    if DetFlags.digitize.TGC_on():
        PileUpToolsList += [ "TgcDigitizationTool" ]
    if DetFlags.digitize.sTGC_on():
        PileUpToolsList += [ "sTgcDigitizationTool" ]
    if DetFlags.digitize.Micromegas_on():
        PileUpToolsList += [ "MM_DigitizationTool" ]
    return PileUpToolsList

def getStandardPileUpToolsList():
    PileUpToolsList = []
    ## Truth information
    PileUpToolsList += getStandardTruthPileUpTools()
    ## Forward Detector Digitization
    PileUpToolsList += getStandardForwardPileUpTools()
    ## Inner Detector Digitization
    PileUpToolsList += getStandardInDetPileUpTools()
    ## Calo Digitization
    PileUpToolsList += getStandardCaloPileUpTools()
    ## Muon System Digitization
    PileUpToolsList += getStandardMuonPileUpTools()
    ## RecoTimingObj
    PileUpToolsList += [ "MergeRecoTimingObjTool" ]
    return PileUpToolsList

def getStandardSignalOnlyTruthPileUpToolsList():
    PileUpToolsList = []
    ## Truth information
    PileUpToolsList += getStandardSignalOnlyTruthPileUpTools()
    ## Forward Detector Digitization
    PileUpToolsList += getStandardForwardPileUpTools()
    ## Inner Detector Digitization
    PileUpToolsList += getStandardInDetPileUpTools()
    ## Calo Digitization
    PileUpToolsList += getStandardCaloPileUpTools()
    ## Muon System Digitization
    PileUpToolsList += getStandardMuonPileUpTools()
    ## RecoTimingObj
    PileUpToolsList += [ "MergeRecoTimingObjTool" ]
    return PileUpToolsList

def getStandardInTimeOnlyTruthPileUpToolsList():
    PileUpToolsList = []
    ## Truth information
    PileUpToolsList += getStandardInTimeOnlyTruthPileUpTools()
    ## Forward Detector Digitization
    PileUpToolsList += getStandardForwardPileUpTools()
    ## Inner Detector Digitization
    PileUpToolsList += getStandardInDetPileUpTools()
    ## Calo Digitization
    PileUpToolsList += getStandardCaloPileUpTools()
    ## Muon System Digitization
    PileUpToolsList += getStandardMuonPileUpTools()
    ## RecoTimingObj
    PileUpToolsList += [ "MergeRecoTimingObjTool" ]
    return PileUpToolsList

def getStandardInTimeOnlyGeantinoTruthPileUpToolsList():
    PileUpToolsList = []
    ## Truth information
    PileUpToolsList += getStandardInTimeOnlyTruthPileUpTools()
    ## Forward Detector Digitization
    PileUpToolsList += getStandardForwardPileUpTools()
    ## Inner Detector Digitization
    PileUpToolsList += getGeantinoTruthInDetPileUpTools()
    ## Calo Digitization
    PileUpToolsList += getStandardCaloPileUpTools()
    ## Muon System Digitization
    PileUpToolsList += getStandardMuonPileUpTools()
    ## RecoTimingObj
    PileUpToolsList += [ "MergeRecoTimingObjTool" ]
    return PileUpToolsList

def getFastPileUpToolsList():
    PileUpToolsList = []
    ## Truth information
    PileUpToolsList += getStandardTruthPileUpTools()
    ## Forward Detector Digitization
    PileUpToolsList += getStandardForwardPileUpTools()
    ## Inner Detector Digitization
    PileUpToolsList += getFastInDetPileUpTools()
    ## Calo Digitization
    PileUpToolsList += getStandardCaloPileUpTools()
    ## Muon System Digitization
    PileUpToolsList += getStandardMuonPileUpTools()
    ## RecoTimingObj
    PileUpToolsList += [ "MergeRecoTimingObjTool" ]
    return PileUpToolsList

def getSplitPileUpToolsList():
    PileUpToolsList = []
    ## Truth information
    PileUpToolsList += getStandardTruthPileUpTools()
    ## Forward Detector Digitization
    PileUpToolsList += getStandardForwardPileUpTools()
    ## Inner Detector Digitization - non-standard
    PileUpToolsList += getSplitInDetPileUpTools()
    ## Calo Digitization
    PileUpToolsList += getStandardCaloPileUpTools()
    ## Muon System Digitization
    PileUpToolsList += getStandardMuonPileUpTools()
    ## RecoTimingObj
    PileUpToolsList += [ "MergeRecoTimingObjTool" ]
    return PileUpToolsList

def getSplitSFPileUpToolsList():
    PileUpToolsList = []
    ## Truth information
    PileUpToolsList += getStandardTruthPileUpTools()
    ## Forward Detector Digitization
    PileUpToolsList += getStandardForwardPileUpTools()
    ## Inner Detector Digitization - non-standard
    PileUpToolsList += getSplitSFInDetPileUpTools()
    ## Calo Digitization
    PileUpToolsList += getStandardCaloPileUpTools()
    ## Muon System Digitization
    PileUpToolsList += getStandardMuonPileUpTools()
    ## RecoTimingObj
    PileUpToolsList += [ "MergeRecoTimingObjTool" ]
    return PileUpToolsList

def getSplitNoMergePileUpToolsList():
    PileUpToolsList = []
    ## Truth information
    PileUpToolsList += getStandardTruthPileUpTools()
    ## Forward Detector Digitization
    PileUpToolsList += getStandardForwardPileUpTools()
    ## Inner Detector Digitization - non-standard
    PileUpToolsList += getSplitNoMergeInDetPileUpTools()
    ## Calo Digitization
    PileUpToolsList += getStandardCaloPileUpTools()
    ## Muon System Digitization
    PileUpToolsList += getStandardMuonPileUpTools()
    ## RecoTimingObj
    PileUpToolsList += [ "MergeRecoTimingObjTool" ]
    return PileUpToolsList

def getSplitNoMergeSFPileUpToolsList():
    PileUpToolsList = []
    ## Truth information
    PileUpToolsList += getStandardTruthPileUpTools()
    ## Forward Detector Digitization
    PileUpToolsList += getStandardForwardPileUpTools()
    ## Inner Detector Digitization - non-standard
    PileUpToolsList += getSplitNoMergeSFInDetPileUpTools()
    ## Calo Digitization
    PileUpToolsList += getStandardCaloPileUpTools()
    ## Muon System Digitization
    PileUpToolsList += getStandardMuonPileUpTools()
    ## RecoTimingObj
    PileUpToolsList += [ "MergeRecoTimingObjTool" ]
    return PileUpToolsList

def getSplitNoMergeFSPileUpToolsList():
    PileUpToolsList = []
    ## Truth information
    PileUpToolsList += getStandardTruthPileUpTools()
    ## Forward Detector Digitization
    PileUpToolsList += getStandardForwardPileUpTools()
    ## Inner Detector Digitization - non-standard
    PileUpToolsList += getSplitNoMergeFSInDetPileUpTools()
    ## Calo Digitization
    PileUpToolsList += getStandardCaloPileUpTools()
    ## Muon System Digitization
    PileUpToolsList += getStandardMuonPileUpTools()
    ## RecoTimingObj
    PileUpToolsList += [ "MergeRecoTimingObjTool" ]
    return PileUpToolsList

def getSplitNoMergeFFPileUpToolsList():
    PileUpToolsList = []
    ## Truth information
    PileUpToolsList += getStandardTruthPileUpTools()
    ## Forward Detector Digitization
    PileUpToolsList += getStandardForwardPileUpTools()
    ## Inner Detector Digitization - non-standard
    PileUpToolsList += getSplitNoMergeFFInDetPileUpTools()
    ## Calo Digitization
    PileUpToolsList += getStandardCaloPileUpTools()
    ## Muon System Digitization
    PileUpToolsList += getStandardMuonPileUpTools()
    ## RecoTimingObj
    PileUpToolsList += [ "MergeRecoTimingObjTool" ]
    return PileUpToolsList

def getStandardPileUpToolsAlg(name="StandardPileUpToolsAlg", **kwargs):
    kwargs.setdefault('PileUpTools', getStandardPileUpToolsList() )
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.doXingByXingPileUp():
        return CfgMgr.PileUpToolsAlg(name, **kwargs)
    else:
        return CfgMgr.DigitizationAlg(name, **kwargs)

def getFastPileUpToolsAlg(name="FastPileUpToolsAlg", **kwargs):
    kwargs.setdefault('PileUpTools', getFastPileUpToolsList() )
    return getStandardPileUpToolsAlg(name, **kwargs)

def getSplitPileUpToolsAlg(name="SplitPileUpToolsAlg", **kwargs):
    kwargs.setdefault('PileUpTools', getSplitPileUpToolsList() )
    return getStandardPileUpToolsAlg(name, **kwargs)

def getSplitSFPileUpToolsAlg(name="SplitSFPileUpToolsAlg", **kwargs):
    kwargs.setdefault('PileUpTools', getSplitSFPileUpToolsList() )
    return getStandardPileUpToolsAlg(name, **kwargs)

def getStandardSignalOnlyTruthPileUpToolsAlg(name="StandardSignalOnlyTruthPileUpToolsAlg", **kwargs):
    kwargs.setdefault('PileUpTools', getStandardSignalOnlyTruthPileUpToolsList() )
    return getStandardPileUpToolsAlg(name, **kwargs)

def getStandardInTimeOnlyTruthPileUpToolsAlg(name="StandardInTimeOnlyTruthPileUpToolsAlg", **kwargs):
    kwargs.setdefault('PileUpTools', getStandardInTimeOnlyTruthPileUpToolsList() )
    return getStandardPileUpToolsAlg(name, **kwargs)

def getStandardInTimeOnlyGeantinoTruthPileUpToolsAlg(name="StandardInTimeOnlyGeantinoTruthPileUpToolsAlg", **kwargs):
    kwargs.setdefault('PileUpTools', getStandardInTimeOnlyGeantinoTruthPileUpToolsList() )
    return getStandardPileUpToolsAlg(name, **kwargs)

def getSplitNoMergePileUpToolsAlg(name="SplitNoMergePileUpToolsAlg", **kwargs):
    kwargs.setdefault('PileUpTools', getSplitNoMergePileUpToolsList() )
    return getStandardPileUpToolsAlg(name, **kwargs)

def getSplitNoMergeSFPileUpToolsAlg(name="SplitNoMergeSFPileUpToolsAlg", **kwargs):
    kwargs.setdefault('PileUpTools', getSplitNoMergeSFPileUpToolsList() )
    return getStandardPileUpToolsAlg(name, **kwargs)

def getSplitNoMergeFSPileUpToolsAlg(name="SplitNoMergeFSPileUpToolsAlg", **kwargs):
    kwargs.setdefault('PileUpTools', getSplitNoMergeFSPileUpToolsList() )
    return getStandardPileUpToolsAlg(name, **kwargs)

def getSplitNoMergeFFPileUpToolsAlg(name="SplitNoMergeFFPileUpToolsAlg", **kwargs):
    kwargs.setdefault('PileUpTools', getSplitNoMergeFFPileUpToolsList() )
    return getStandardPileUpToolsAlg(name, **kwargs)

def getTestPileUpToolsAlg(name="TestPileUpToolsAlg", **kwargs):
    PileUpToolsList = [ "TestPileUpTool" ]
    kwargs.setdefault('PileUpTools', PileUpToolsList )
    from AthenaCommon.Constants import DEBUG
    kwargs.setdefault('OutputLevel', DEBUG )
    return getStandardPileUpToolsAlg(name, **kwargs)

def getTestFilterPileUpToolsAlg(name="TestFilterPileUpToolsAlg", **kwargs):
    PileUpToolsList = [ "TestFilterPileUpTool" ]
    kwargs.setdefault('PileUpTools', PileUpToolsList )
    from AthenaCommon.Constants import DEBUG
    kwargs.setdefault('OutputLevel', DEBUG )
    return getStandardPileUpToolsAlg(name, **kwargs)

def getTestTruthJetFilterPileUpToolsAlg(name="TestTruthJetFilterPileUpToolsAlg", **kwargs):
    PileUpToolsList = [ "MergeTruthJetsFilterTool" ]
    kwargs.setdefault('PileUpTools', PileUpToolsList )
    from AthenaCommon.Constants import DEBUG
    kwargs.setdefault('OutputLevel', DEBUG )
    return getStandardPileUpToolsAlg(name, **kwargs)
