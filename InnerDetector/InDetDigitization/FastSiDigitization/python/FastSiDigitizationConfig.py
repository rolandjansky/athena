# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

    #################################
    # Config pixel conditions setup #
    #################################
    from IOVDbSvc.CondDB import conddb
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    #################
    # Module status #
    #################
    useNewChargeFormat  = False
    useNewDeadmapFormat = False

    if not hasattr(condSeq, "PixelConfigCondAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg

        IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
        rodIDForSingleLink40=0
        # ITk:
        if geoFlags.isSLHC():
            IdMappingDat = "ITk_Atlas_IdMapping.dat"
            if "BrlIncl4.0_ref" == commonGeoFlags.GeoType():
                IdMappingDat = "ITk_Atlas_IdMapping_InclBrl4.dat"
            elif "IBrlExt4.0ref" == commonGeoFlags.GeoType():
                IdMappingDat = "ITk_Atlas_IdMapping_IExtBrl4.dat"
            elif "BrlExt4.0_ref" == commonGeoFlags.GeoType():
                IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl4.dat"
            elif "BrlExt3.2_ref" == commonGeoFlags.GeoType():
                IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl32.dat"
        elif (geoFlags.isIBL() == False):
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping.dat"
        else:
            # Planar IBL
            if (geoFlags.IBLLayout() == "planar"):
                if (geoFlags.isDBM() == True):
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat"
                else:
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL.dat"
            # Hybrid IBL plus DBM
            elif (geoFlags.IBLLayout() == "3D"):
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
        
        condSeq += PixelConfigCondAlg(name="PixelConfigCondAlg", 
                                      ReadDeadMapKey = "",
                                      UseCablingConditions=False,
                                      CablingMapFileName=IdMappingDat)

    if useNewDeadmapFormat:
        if not hasattr(condSeq, "PixelDeadMapCondAlg"):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDeadMapCondAlg
            condSeq += PixelDeadMapCondAlg(name="PixelDeadMapCondAlg",ReadKey="")

    #FIXME: at some point we should move away from being dependent on the experimentalDigi flags.
    if 'doFastSCT_Digi' in digitizationFlags.experimentalDigi() and not 'doFastPixelDigi' in digitizationFlags.experimentalDigi():
        # Set empty Folder
        if not useNewChargeFormat:
            if not hasattr(condSeq, 'PixelChargeCalibCondAlg'):
                from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeCalibCondAlg
                condSeq += PixelChargeCalibCondAlg(name="PixelChargeCalibCondAlg", ReadKey="")
        else:
            if not hasattr(condSeq, 'PixelChargeLUTCalibCondAlg'):
                from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeLUTCalibCondAlg
                condSeq += PixelChargeLUTCalibCondAlg(name="PixelChargeLUTCalibCondAlg", ReadKey="")

    else:
        #####################
        # Calibration Setup #
        #####################
        if not useNewChargeFormat:
            if not conddb.folderRequested("/PIXEL/PixCalib"):
                conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/PixCalib", "/PIXEL/PixCalib", className="CondAttrListCollection")
            if not hasattr(condSeq, 'PixelChargeCalibCondAlg'):
                from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeCalibCondAlg
                condSeq += PixelChargeCalibCondAlg(name="PixelChargeCalibCondAlg", ReadKey="/PIXEL/PixCalib")
        else:
            if not conddb.folderRequested("/PIXEL/ChargeCalibration"):
                conddb.addFolder("PIXEL_OFL", "/PIXEL/ChargeCalibration", className="CondAttrListCollection")
            if not hasattr(condSeq, 'PixelChargeLUTCalibCondAlg'):
                from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeLUTCalibCondAlg
                condSeq += PixelChargeLUTCalibCondAlg(name="PixelChargeLUTCalibCondAlg", ReadKey="/PIXEL/ChargeCalibration")

        #####################
        # Cabling map Setup #
        #####################
        if (conddb.dbmc=="OFLP200" and geoFlags.isIBL()==True) and not conddb.folderRequested("/PIXEL/HitDiscCnfg"):
            conddb.addFolderSplitMC("PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg", className="AthenaAttributeList")

            if not hasattr(condSeq, 'PixelHitDiscCnfgAlg'):
                from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelHitDiscCnfgAlg
                condSeq += PixelHitDiscCnfgAlg(name="PixelHitDiscCnfgAlg")

        if not conddb.folderRequested("/PIXEL/ReadoutSpeed"):
            conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed", className="AthenaAttributeList")

        if not hasattr(condSeq, 'PixelReadoutSpeedAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelReadoutSpeedAlg
            condSeq += PixelReadoutSpeedAlg(name="PixelReadoutSpeedAlg")

        if not hasattr(condSeq, 'PixelCablingCondAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelCablingCondAlg
            condSeq += PixelCablingCondAlg(name="PixelCablingCondAlg",
                                           MappingFile=IdMappingDat,
                                           RodIDForSingleLink40=rodIDForSingleLink40)

        if not conddb.folderRequested('/PIXEL/PixdEdx'):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/PixdEdx", className="AthenaAttributeList")

        if not conddb.folderRequested("/PIXEL/PixReco"):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/PixReco", className="DetCondCFloat")

        if not conddb.folderRequested("/Indet/PixelDist"):
            conddb.addFolder("INDET", "/Indet/PixelDist", className="DetCondCFloat")

        if not hasattr(condSeq, 'PixelOfflineCalibCondAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelOfflineCalibCondAlg
            condSeq += PixelOfflineCalibCondAlg(name="PixelOfflineCalibCondAlg", ReadKey="/PIXEL/PixReco")
            PixelOfflineCalibCondAlg.InputSource = 2

        if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
            from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
            pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()

        if not hasattr(condSeq, 'PixelDistortionAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDistortionAlg
            condSeq += PixelDistortionAlg(name="PixelDistortionAlg")

        if not hasattr(condSeq, 'PixeldEdxAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixeldEdxAlg
            condSeq += PixeldEdxAlg(name="PixeldEdxAlg")
            PixeldEdxAlg.ReadFromCOOL = True

        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, "PixelRecoDbTool"):
            from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
            ToolSvc += PixelRecoDbTool()
        ToolSvc.PixelRecoDbTool.InputSource = 1

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
        from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
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
    from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
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
