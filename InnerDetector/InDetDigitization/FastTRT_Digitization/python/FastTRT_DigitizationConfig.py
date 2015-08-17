# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# The earliest bunch crossing time for which interactions will be sent
# to the FastTRT Digitization code.
def FastTRT_FirstXing():
    return -50

# The latest bunch crossing time for which interactions will be sent
# to the FastTRT Digitization code.
def FastTRT_LastXing():
    return 50

def getFastTRTRange(name="FastTRTRange", **kwargs):
    #this is the time of the xing in ns
    kwargs.setdefault('FirstXing', FastTRT_FirstXing() )
    kwargs.setdefault('LastXing', FastTRT_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["TRTUncompressedHitCollection#TRTUncompressedHits"] )
    from AthenaCommon import CfgMgr
    return CfgMgr.PileUpXingFolder(name, **kwargs)

def BasicTRTFastDigitizationTool(name, **kwargs):

    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested('/TRT/Calib/ToTCalib'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/ToTCalib","/TRT/Calib/ToTCalib")
    if not conddb.folderRequested('/TRT/Calib/HTCalib'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/HTCalib","/TRT/Calib/HTCalib")
    if not conddb.folderRequested('/TRT/Calib/errors'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/errors","/TRT/Calib/errors")
    #choose random number service
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc() )
    streamName = kwargs.setdefault("RandomStreamName", "FastTRTDigitization")
    # set rndm seeds
    if not digitizationFlags.rndmSeedList.checkForExistingSeed(streamName):
        digitizationFlags.rndmSeedList.addSeed(streamName, 45123, 94345 )

    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", FastTRT_FirstXing())
        kwargs.setdefault("LastXing",  FastTRT_LastXing())

    from AthenaCommon import CfgMgr
    return CfgMgr.TRTFastDigitizationTool(name,**kwargs)

def TRTFastDigitizationTool(name="TRTFastDigitizationTool",**kwargs):
    kwargs.setdefault("trtDriftCircleContainerName", "TRT_DriftCircles")
    kwargs.setdefault("prdMultiTruthCollectionName", "PRD_MultiTruthTRT")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return BasicTRTFastDigitizationTool(name,**kwargs)

def TRTFastDigitizationToolHS(name="TRTFastDigitizationToolHS",**kwargs):
    kwargs.setdefault("trtDriftCircleContainerName", "TRT_DriftCircles")
    kwargs.setdefault("prdMultiTruthCollectionName", "PRD_MultiTruthTRT")
    kwargs.setdefault("HardScatterSplittingMode", 1)
    return BasicTRTFastDigitizationTool(name,**kwargs)

def TRTFastDigitizationToolPU(name="TRTFastDigitizationToolPU",**kwargs):
    kwargs.setdefault("trtDriftCircleContainerName", "TRT_PU_DriftCircles")
    kwargs.setdefault("prdMultiTruthCollectionName", "PRD_MultiTruthTRT_PU")
    kwargs.setdefault("HardScatterSplittingMode", 2)
    return BasicTRTFastDigitizationTool(name,**kwargs)

def TRTFastDigitizationToolSplitNoMergePU(name="TRTFastDigitizationToolSplitNoMergePU",**kwargs):
    kwargs.setdefault("trtHitCollectionName", "PileupTRTUncompressedHits")
    kwargs.setdefault("trtDriftCircleContainerName", "TRT_PU_DriftCircles")
    kwargs.setdefault("prdMultiTruthCollectionName", "PRD_MultiTruthTRT_PU")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return BasicTRTFastDigitizationTool(name,**kwargs)
