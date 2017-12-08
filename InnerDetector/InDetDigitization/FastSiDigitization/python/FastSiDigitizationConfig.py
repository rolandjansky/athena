# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# The earliest bunch crossing time for which interactions will be sent
# to the Fast Pixel Digitization code.
def FastPixel_FirstXing():
    FirstXing = -50
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.estimatedLuminosity()> 0.5e33:
        FirstXing = -25
    return FirstXing
# The latest bunch crossing time for which interactions will be sent
# to the Fast Pixel Digitization code.
def FastPixel_LastXing():
    LastXing = 100
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.estimatedLuminosity()> 0.5e33:
        if jobproperties.Beam.bunchSpacing.get_Value() > 50 :
            LastXing = 75
        else :
            LastXing = 25
    return LastXing
# The earliest bunch crossing time for which interactions will be sent
# to the Fast SCT Digitization code.
def FastSCT_FirstXing():
    return -50
# The latest bunch crossing time for which interactions will be sent
# to the Fast SCT Digitization code.
def FastSCT_LastXing():
    return 25

def FastClusterMakerTool(name="FastClusterMakerTool", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    #FIXME: at some point we should move away from being dependent on the experimentalDigi flags.
    if 'doFastSCT_Digi' in digitizationFlags.experimentalDigi() and not 'doFastPixelDigi' in digitizationFlags.experimentalDigi():
        kwargs.setdefault("UsePixelCalibCondDB", False)
        kwargs.setdefault("PixelCalibSvc","");
        kwargs.setdefault("PixelOfflineCalibSvc","");
    else:
        from AthenaCommon.Include import include
        include( "PixelConditionsServices/PixelDCSSvc_jobOptions.py" )
        include.block( "PixelConditionsServices/PixelDCSSvc_jobOptions.py" )
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, "PixelRecoDbTool"):
            from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
            ToolSvc += PixelRecoDbTool()
        ToolSvc.PixelRecoDbTool.InputSource = 1

        # setup PixelCalibDbTool in ToolSvc
        if not hasattr(ToolSvc, "PixelCalibDbTool"):
            from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTool
            ToolSvc += PixelCalibDbTool()

        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested('/PIXEL/PixCalib'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/PixCalib")
        if not conddb.folderRequested('/PIXEL/ReadoutSpeed'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/ReadoutSpeed")
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, "PixelCalibSvc"):
            from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc
            InDetPixelCalibSvc = PixelCalibSvc()
            ServiceMgr += InDetPixelCalibSvc
        if not hasattr(ServiceMgr, "PixelSiPropertiesSvc"):
            from SiLorentzAngleSvc.LorentzAngleSvcSetup import lorentzAngleSvc
            from SiPropertiesSvc.SiPropertiesSvcConf import SiPropertiesSvc;
            PixelSiPropertiesSvc = SiPropertiesSvc(name = "PixelSiPropertiesSvc",DetectorName="Pixel",SiConditionsServices = lorentzAngleSvc.pixelSiliconConditionsSvc)
            ServiceMgr += PixelSiPropertiesSvc
        if not hasattr(ServiceMgr, "PixelOfflineCalibSvc"):
            from PixelConditionsServices.PixelConditionsServicesConf import PixelOfflineCalibSvc
            ServiceMgr +=PixelOfflineCalibSvc()
            ServiceMgr.PixelOfflineCalibSvc.HDCFromCOOL = False

    from AthenaCommon import CfgMgr
    return CfgMgr.InDet__ClusterMakerTool(name,**kwargs)

def commonPixelFastDigitizationConfig(name,**kwargs):
    kwargs.setdefault("ClusterMaker", "FastClusterMakerTool")

    # Import Digitization job properties
    from Digitization.DigitizationFlags import digitizationFlags
    # set the random service, stream name
    streamName = kwargs.setdefault("RndmEngine", "FastPixelDigitization")
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc() )
    # set rndm seeds
    if not digitizationFlags.rndmSeedList.checkForExistingSeed(streamName):
        digitizationFlags.rndmSeedList.addSeed(streamName, 10513239, 492615104 )

    if digitizationFlags.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", FastPixel_FirstXing())
        kwargs.setdefault("LastXing",  FastPixel_LastXing() )

    from AthenaCommon import CfgMgr
    return CfgMgr.PixelFastDigitizationTool(name,**kwargs)

def commonSCT_FastDigitizationConfig(name,**kwargs):

    # Setup the DCS folders and Svc used in the sctSiliconConditionsSvc
    from IOVDbSvc.CondDB import conddb
    sctDCSStateFolder = '/SCT/DCS/CHANSTAT'
    sctDCSTempFolder = '/SCT/DCS/MODTEMP'
    sctDCSHVFolder = '/SCT/DCS/HV'
    if not conddb.folderRequested(sctDCSStateFolder):
        conddb.addFolder("DCS_OFL", sctDCSStateFolder, className="CondAttrListCollection")
    if not conddb.folderRequested(sctDCSTempFolder):
        conddb.addFolder("DCS_OFL", sctDCSTempFolder, className="CondAttrListCollection")
    if not conddb.folderRequested(sctDCSHVFolder):
        conddb.addFolder("DCS_OFL", sctDCSHVFolder, className="CondAttrListCollection")
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr(ServiceMgr, "InDetSCT_DCSConditionsSvc"):
        from AthenaCommon.AlgSequence import AthSequencer
        condSequence = AthSequencer("AthCondSeq")
        if not hasattr(condSequence, "SCT_DCSConditionsHVCondAlg"):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsHVCondAlg
            condSequence += SCT_DCSConditionsHVCondAlg(name = "SCT_DCSConditionsHVCondAlg",
                                                       ReadKey = sctDCSHVFolder)
        if not hasattr(condSequence, "SCT_DCSConditionsStatCondAlg"):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsStatCondAlg
            condSequence += SCT_DCSConditionsStatCondAlg(name = "SCT_DCSConditionsStatCondAlg",
                                                         ReadKeyHV = sctDCSHVFolder,
                                                         ReadKeyState = sctDCSStateFolder)
        if not hasattr(condSequence, "SCT_DCSConditionsTempCondAlg"):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsTempCondAlg
            condSequence += SCT_DCSConditionsTempCondAlg(name = "SCT_DCSConditionsTempCondAlg",
                                                         ReadKey = sctDCSTempFolder)
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsSvc
        InDetSCT_DCSConditionsSvc = SCT_DCSConditionsSvc(name = "InDetSCT_DCSConditionsSvc")
        ServiceMgr += InDetSCT_DCSConditionsSvc

    kwargs.setdefault("ClusterMaker", "FastClusterMakerTool")

    # Import Digitization job properties
    from Digitization.DigitizationFlags import digitizationFlags
    # set the random service, stream name
    streamName = kwargs.setdefault("RndmEngine", "FastSCT_Digitization")
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc() )
    # set rndm seeds
    if not digitizationFlags.rndmSeedList.checkForExistingSeed(streamName):
        digitizationFlags.rndmSeedList.addSeed(streamName, 49261510, 105132394 )

    if digitizationFlags.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", FastSCT_FirstXing())
        kwargs.setdefault("LastXing",  FastSCT_LastXing() )

    from AthenaCommon import CfgMgr
    return CfgMgr.SCT_FastDigitizationTool(name,**kwargs)

######################################################################################
def PixelFastDigitizationTool(name="PixelFastDigitizationTool", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return commonPixelFastDigitizationConfig(name, **kwargs)

######################################################################################
def PixelFastDigitizationToolHS(name="PixelFastDigitizationToolHS", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 1)
    return commonPixelFastDigitizationConfig(name, **kwargs)

######################################################################################
def PixelFastDigitizationToolPU(name="PixelFastDigitizationToolPU", **kwargs):
    kwargs.setdefault("PixelClusterContainerName", "Pixel_PU_Clusters")
    kwargs.setdefault("TruthNamePixel", "PRD_MultiTruthPixel_PU")
    kwargs.setdefault("HardScatterSplittingMode", 2)
    return commonPixelFastDigitizationConfig(name, **kwargs)

######################################################################################
def PixelFastDigitizationToolSplitNoMergePU(name="PixelFastDigitizationToolSplitNoMergePU", **kwargs):
    kwargs.setdefault("InputObjectName", "PileupPixelHits")
    kwargs.setdefault("PixelClusterContainerName", "PixelFast_PU_Clusters")
    kwargs.setdefault("TruthNamePixel", "PRD_MultiTruthPixel_PU")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return commonPixelFastDigitizationConfig(name, **kwargs)

######################################################################################
def SCT_FastDigitizationTool(name="SCT_FastDigitizationTool", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return commonSCT_FastDigitizationConfig(name,**kwargs)

######################################################################################
def SCT_FastDigitizationToolHS(name="SCT_FastDigitizationToolHS",**kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 1)
    return commonSCT_FastDigitizationConfig(name,**kwargs)

######################################################################################
def SCT_FastDigitizationToolPU(name="SCT_FastDigitizationToolPU",**kwargs):
    kwargs.setdefault("SCT_ClusterContainerName", "SCT_PU_Clusters")
    kwargs.setdefault("TruthNameSCT", "PRD_MultiTruthSCT_PU")
    kwargs.setdefault("HardScatterSplittingMode", 2)
    return commonSCT_FastDigitizationConfig(name,**kwargs)

######################################################################################
def SCT_FastDigitizationToolSplitNoMergePU(name="SCT_FastDigitizationToolSplitNoMergePU",**kwargs):
    kwargs.setdefault("InputObjectName", "PileupSCT_Hits")
    kwargs.setdefault("SCT_ClusterContainerName", "SCT_PU_Clusters")
    kwargs.setdefault("TruthNameSCT", "PRD_MultiTruthSCT_PU")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return commonSCT_FastDigitizationConfig(name,**kwargs)

######################################################################################
def getFastPixelRange(name="FastPixelRange" , **kwargs):
    kwargs.setdefault('FirstXing', FastPixel_FirstXing() )
    kwargs.setdefault('LastXing',  FastPixel_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["SiHitCollection#PixelHits"] )
    from AthenaCommon import CfgMgr
    return CfgMgr.PileUpXingFolder(name, **kwargs)

######################################################################################
def getFastSCTRange(name="FastSCTRange" , **kwargs):
    #this is the time of the xing in ns
    kwargs.setdefault('FirstXing', FastSCT_FirstXing() )
    kwargs.setdefault('LastXing',  FastSCT_LastXing()  )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["SiHitCollection#SCT_Hits"] )
    from AthenaCommon import CfgMgr
    return CfgMgr.PileUpXingFolder(name, **kwargs)
