# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# The earliest bunch crossing time for which interactions will be sent
# to the Fast Pixel Digitization code.
def FastPixel_FirstXing():
    FirstXing = 0
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.estimatedLuminosity()> 0.5e33:
        FirstXing = 0
    return FirstXing
# The latest bunch crossing time for which interactions will be sent
# to the Fast Pixel Digitization code.
def FastPixel_LastXing():
    LastXing = 0
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.estimatedLuminosity()> 0.5e33:
        if jobproperties.Beam.bunchSpacing.get_Value() > 50 :
            LastXing = 0
        else :
            LastXing = 0
    return LastXing
# The earliest bunch crossing time for which interactions will be sent
# to the Fast SCT Digitization code.
def FastSCT_FirstXing():
    return 0
# The latest bunch crossing time for which interactions will be sent
# to the Fast SCT Digitization code.
def FastSCT_LastXing():
    return 0

def FastClusterMakerTool(name="FastClusterMakerTool", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    #FIXME: at some point we should move away from being dependent on the experimentalDigi flags.
    if 'doFastSCT_Digi' in digitizationFlags.experimentalDigi() and not 'doFastPixelDigi' in digitizationFlags.experimentalDigi():
        kwargs.setdefault("UsePixelCalibCondDB", False)
        kwargs.setdefault("PixelCalibSvc","");
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

    # SiLorentzAngleTool for PixelFastDigitizationTool
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
        from SiLorentzAngleSvc.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
        pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()
    kwargs.setdefault("LorentzAngleTool", ToolSvc.PixelLorentzAngleTool)

    from AthenaCommon import CfgMgr
    return CfgMgr.PixelFastDigitizationTool(name,**kwargs)

def commonSCT_FastDigitizationConfig(name,**kwargs):

    # Setup the DCS folders and tool used in the sctSiliconConditionsTool
    from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
    sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
    sct_DCSConditionsToolSetup.setup()

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

    # Set up SCT_SiliconConditionsTool
    from SCT_ConditionsTools.SCT_SiliconConditionsToolSetup import SCT_SiliconConditionsToolSetup
    sct_SiliconConditionsToolSetup = SCT_SiliconConditionsToolSetup()
    sct_SiliconConditionsToolSetup.setDcsTool(sct_DCSConditionsToolSetup.getTool())
    sct_SiliconConditionsToolSetup.setup()
    # Set up SCT_SiPropertiesTool
    from SiPropertiesTool.SCT_SiPropertiesToolSetup import SCT_SiPropertiesToolSetup
    sct_SiPropertiesToolSetup = SCT_SiPropertiesToolSetup()
    sct_SiPropertiesToolSetup.setSiliconTool(sct_SiliconConditionsToolSetup.getTool())
    sct_SiPropertiesToolSetup.setup()

    # SiLorentzAngleTool for SCT_FastDigitizationTool
    from AthenaCommon.AppMgr import ToolSvc
    from SiLorentzAngleSvc.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
    sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()
    kwargs.setdefault("LorentzAngleTool", sctLorentzAngleToolSetup.SCTLorentzAngleTool)

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
    kwargs.setdefault("PixelClusterAmbiguitiesMapName", "PixelClusterAmbiguitiesMapPU")
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
