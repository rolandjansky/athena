# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from Digitization.DigitizationFlags import digitizationFlags
        
# The earliest bunch crossing time for which interactions will be sent
# to the Pixel Digitization code.
def Pixel_FirstXing():
    FirstXing = -50
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.estimatedLuminosity()> 0.5e33:
        FirstXing = -25
    return FirstXing

# The latest bunch crossing time for which interactions will be sent
# to the Pixel Digitization code.
def Pixel_LastXing():
    LastXing = 100
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.estimatedLuminosity()> 0.5e33:
        if jobproperties.Beam.bunchSpacing.get_Value() > 50 :
            LastXing = 75
        else :
            LastXing = 25
    return LastXing

###############################################################################

def RadDamageUtil(name="RadDamageUtil", **kwargs):
    kwargs.setdefault("defaultRamo", 1)
    kwargs.setdefault("betaElectrons", 4.5e-16)
    kwargs.setdefault("betaHoles", 6.0e-16)
    kwargs.setdefault("saveDebugMaps", False)
    return CfgMgr.RadDamageUtil(name, **kwargs)

def EfieldInterpolator(name="EfieldInterpolator", **kwargs):
    kwargs.setdefault("initialized", False)
    kwargs.setdefault("useSpline", True)
    kwargs.setdefault("sensorDepth", 200)
    return CfgMgr.EfieldInterpolator(name, **kwargs)

def EnergyDepositionTool(name="EnergyDepositionTool", **kwargs):
    kwargs.setdefault("DeltaRayCut", 80.7687)  
    kwargs.setdefault("nCols", 5)
    kwargs.setdefault("LoopLimit", 100000)
    kwargs.setdefault("doBichsel", hasattr(digitizationFlags, "doBichselSimulation") and digitizationFlags.doBichselSimulation())
    kwargs.setdefault("doBichselBetaGammaCut", 0.7)   # dEdx not quite consistent below this
    kwargs.setdefault("doDeltaRay", False)            # needs validation
    kwargs.setdefault("doPU", False)
    return CfgMgr.EnergyDepositionTool(name, **kwargs)

def SensorSimPlanarTool(name="SensorSimPlanarTool", **kwargs):
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("SiPropertiesTool", ToolSvc.PixelSiPropertiesTool)
    kwargs.setdefault("LorentzAngleTool", ToolSvc.PixelLorentzAngleTool)
    kwargs.setdefault("doRadDamage", digitizationFlags.doPixelPlanarRadiationDamage.get_Value())
    kwargs.setdefault("doRadDamageTemplate", digitizationFlags.doPixelPlanarRadiationDamageTemplate.get_Value())
    return CfgMgr.SensorSimPlanarTool(name, **kwargs)

def SensorSim3DTool(name="SensorSim3DTool", **kwargs):
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("SiPropertiesTool", ToolSvc.PixelSiPropertiesTool)
    kwargs.setdefault("doRadDamage", digitizationFlags.doPixel3DRadiationDamage.get_Value())
    kwargs.setdefault("doRadDamageTemplate", digitizationFlags.doPixel3DRadiationDamageTemplate.get_Value())
    return CfgMgr.SensorSim3DTool(name, **kwargs)

def SensorSimTool(name="SensorSimTool", **kwargs):
    return CfgMgr.SensorSimTool(name, **kwargs)

def FrontEndSimTool(name="FrontEndSimTool", **kwargs):
    from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
    pixelConditionsSummaryToolSetup = PixelConditionsSummaryTool("PixelConditionsSummaryTool",
                                                                 UseByteStreamFEI4=False,
                                                                 UseByteStreamFEI3=False)
    kwargs.setdefault("PixelConditionsSummaryTool", pixelConditionsSummaryToolSetup)
    return CfgMgr.FrontEndSimTool(name, **kwargs)

def BarrelRD53SimTool(name="BarrelRD53SimTool", **kwargs):
    kwargs.setdefault("BarrelEC", 0)
    kwargs.setdefault("DoNoise", digitizationFlags.doInDetNoise.get_Value())
    return CfgMgr.RD53SimTool(name, **kwargs)

def EndcapRD53SimTool(name="EndcapRD53SimTool", **kwargs):
    kwargs.setdefault("BarrelEC", 2)
    kwargs.setdefault("DoNoise", digitizationFlags.doInDetNoise.get_Value())
    return CfgMgr.RD53SimTool(name, **kwargs)

def BarrelFEI4SimTool(name="BarrelFEI4SimTool", **kwargs):
    kwargs.setdefault("BarrelEC", 0)
    kwargs.setdefault("DoNoise", digitizationFlags.doInDetNoise.get_Value())
    from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
    pixelConditionsSummaryToolSetup = PixelConditionsSummaryTool("PixelConditionsSummaryTool",
                                                                 UseByteStreamFEI4=False,
                                                                 UseByteStreamFEI3=False)
    kwargs.setdefault("PixelConditionsSummaryTool", pixelConditionsSummaryToolSetup)
    return CfgMgr.FEI4SimTool(name, **kwargs)

def DBMFEI4SimTool(name="DBMFEI4SimTool", **kwargs):
    kwargs.setdefault("BarrelEC", 4)
    kwargs.setdefault("DoNoise", digitizationFlags.doInDetNoise.get_Value())
    from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
    pixelConditionsSummaryToolSetup = PixelConditionsSummaryTool("PixelConditionsSummaryTool",
                                                                 UseByteStreamFEI4=False,
                                                                 UseByteStreamFEI3=False)
    kwargs.setdefault("PixelConditionsSummaryTool", pixelConditionsSummaryToolSetup)
    return CfgMgr.FEI4SimTool(name, **kwargs)

def BarrelFEI3SimTool(name="BarrelFEI3SimTool", **kwargs):
    kwargs.setdefault("BarrelEC", 0)
    from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
    pixelConditionsSummaryToolSetup = PixelConditionsSummaryTool("PixelConditionsSummaryTool",
                                                                 UseByteStreamFEI4=False,
                                                                 UseByteStreamFEI3=False)
    kwargs.setdefault("PixelConditionsSummaryTool", pixelConditionsSummaryToolSetup)
    return CfgMgr.FEI3SimTool(name, **kwargs)

def EndcapFEI3SimTool(name="EndcapFEI3SimTool", **kwargs):
    kwargs.setdefault("BarrelEC", 2)
    from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
    pixelConditionsSummaryToolSetup = PixelConditionsSummaryTool("PixelConditionsSummaryTool",
                                                                 UseByteStreamFEI4=False,
                                                                 UseByteStreamFEI3=False)
    kwargs.setdefault("PixelConditionsSummaryTool", pixelConditionsSummaryToolSetup)
    return CfgMgr.FEI3SimTool(name, **kwargs)

def IdMapping():
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags

    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"

    if not geoFlags.isIBL():
        IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping.dat"
    else:
        # Planar IBL
        if (geoFlags.IBLLayout() == "planar"):
            if geoFlags.isDBM():
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat"
            else:
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL.dat"
        # Hybrid IBL plus DBM
        elif (geoFlags.IBLLayout() == "3D"):
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"

    return IdMappingDat

def PixelConfigCondAlg_MC():

    #################
    # Module status #
    #################
    from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg
    alg = PixelConfigCondAlg(name="PixelConfigCondAlg")

    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType == "cosmics" :
        alg.BarrelTimeJitter=[25.0,25.0,25.0,25.0]
        alg.EndcapTimeJitter=[25.0,25.0,25.0]
        alg.DBMTimeJitter=[25.0,25.0,25.0]
        alg.BarrelNumberOfBCID=[8,8,8,8]
        alg.EndcapNumberOfBCID=[8,8,8]
        alg.DBMNumberOfBCID=[8,8,8]
        alg.BarrelTimeOffset=[100.0,100.0,100.0,100.0]
        alg.EndcapTimeOffset=[100.0,100.0,100.0]
        alg.DBMTimeOffset=[100.0,100.0,100.0]

    alg.CablingMapFileName=IdMapping()

    return alg

def BasicPixelDigitizationTool(name="PixelDigitizationTool", **kwargs):
    from AthenaCommon.AppMgr import ServiceMgr
    from AthenaCommon.AppMgr import ToolSvc
    from IOVDbSvc.CondDB import conddb
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")

    if not hasattr(condSeq, 'PixelConfigCondAlg'):
        condSeq += PixelConfigCondAlg_MC()

    if digitizationFlags.doPixelPlanarRadiationDamage.get_Value() or digitizationFlags.doPixel3DRadiationDamage.get_Value():
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelRadSimFluenceMapAlg
        condSeq += PixelRadSimFluenceMapAlg()

    ############################################################################################
    # Set up Conditions DB
    ############################################################################################
    if not conddb.folderRequested("/PIXEL/PixelModuleFeMask"):
        conddb.addFolder("PIXEL_OFL", "/PIXEL/PixelModuleFeMask", className="CondAttrListCollection")

    if not hasattr(condSeq, "PixelDeadMapCondAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDeadMapCondAlg
        condSeq += PixelDeadMapCondAlg(name="PixelDeadMapCondAlg")

    if not hasattr(condSeq, "PixelDCSCondStateAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStateAlg
        condSeq += PixelDCSCondStateAlg(name="PixelDCSCondStateAlg",
                                        ReadKeyState = '')

    if not hasattr(condSeq, "PixelDCSCondStatusAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStatusAlg
        condSeq += PixelDCSCondStatusAlg(name="PixelDCSCondStatusAlg",
                                         ReadKeyStatus = '')

    if not conddb.folderRequested("/PIXEL/DCS/HV"):
        conddb.addFolder("DCS_OFL", "/PIXEL/DCS/HV", className="CondAttrListCollection")

    if not hasattr(condSeq,"PixelDCSCondHVAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondHVAlg
        condSeq += PixelDCSCondHVAlg(name="PixelDCSCondHVAlg")

    if not conddb.folderRequested("/PIXEL/DCS/TEMPERATURE"):
        conddb.addFolder("DCS_OFL", "/PIXEL/DCS/TEMPERATURE", className="CondAttrListCollection")

    if not hasattr(condSeq,"PixelDCSCondTempAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondTempAlg
        condSeq += PixelDCSCondTempAlg(name="PixelDCSCondTempAlg")

    #####################
    # Calibration Setup #
    #####################
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    if commonGeoFlags.Run()=="RUN3" and 'UseOldIBLCond' not in digitizationFlags.experimentalDigi():
        if not conddb.folderRequested("/PIXEL/ChargeCalibration"):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/ChargeCalibration", className="CondAttrListCollection")
        if not hasattr(condSeq, 'PixelChargeLUTCalibCondAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeLUTCalibCondAlg
            condSeq += PixelChargeLUTCalibCondAlg(name="PixelChargeLUTCalibCondAlg", ReadKey="/PIXEL/ChargeCalibration")
    else:
        if not conddb.folderRequested("/PIXEL/PixCalib"):
            conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/PixCalib", "/PIXEL/PixCalib", className="CondAttrListCollection")
        if not hasattr(condSeq, 'PixelChargeCalibCondAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeCalibCondAlg
            condSeq += PixelChargeCalibCondAlg(name="PixelChargeCalibCondAlg", ReadKey="/PIXEL/PixCalib" if commonGeoFlags.Run() == "RUN2" else "")

    #####################
    # Cabling map Setup #
    #####################
    if (globalflags.DataSource=='data' and conddb.dbdata == 'CONDBR2'):  # for data overlay
        if geoFlags.isIBL() and not conddb.folderRequested("/PIXEL/HitDiscCnfg"):
            conddb.addFolderSplitMC("PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg", className="AthenaAttributeList")

        if geoFlags.isIBL() and not hasattr(condSeq, 'PixelHitDiscCnfgAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelHitDiscCnfgAlg
            condSeq += PixelHitDiscCnfgAlg(name="PixelHitDiscCnfgAlg")

        if not conddb.folderRequested("/PIXEL/ReadoutSpeed"):
            conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed", className="AthenaAttributeList")

        if not hasattr(condSeq, 'PixelReadoutSpeedAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelReadoutSpeedAlg
            condSeq += PixelReadoutSpeedAlg(name="PixelReadoutSpeedAlg")

        if not conddb.folderRequested("/PIXEL/CablingMap"):
            conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap", className="AthenaAttributeList")

        if not hasattr(condSeq, 'PixelCablingCondAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelCablingCondAlg
            condSeq += PixelCablingCondAlg(name="PixelCablingCondAlg", ReadKey = '')

    if not conddb.folderRequested("/Indet/PixelDist"):
        conddb.addFolder("INDET", "/Indet/PixelDist", className="DetCondCFloat")

    if not hasattr(condSeq, 'PixelDistortionAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDistortionAlg
        condSeq += PixelDistortionAlg(name="PixelDistortionAlg", ReadKey="/Indet/PixelDist")

    #######################
    # Setup Lorentz angle #
    #######################
    if not hasattr(condSeq, "PixelSiPropertiesCondAlg"):
        from SiPropertiesTool.SiPropertiesToolConf import PixelSiPropertiesCondAlg
        condSeq += PixelSiPropertiesCondAlg(name="PixelSiPropertiesCondAlg")

    if not hasattr(ToolSvc, "PixelSiPropertiesTool"):
        from SiPropertiesTool.SiPropertiesToolConf import SiPropertiesTool
        ToolSvc += SiPropertiesTool(name = "PixelSiPropertiesTool",
                                    DetectorName = "Pixel",
                                    ReadKey = "PixelSiliconPropertiesVector")

    if not hasattr(condSeq, "PixelSiLorentzAngleCondAlg"):
        from SiLorentzAngleTool.SiLorentzAngleToolConf import PixelSiLorentzAngleCondAlg
        condSeq += PixelSiLorentzAngleCondAlg(name = "PixelSiLorentzAngleCondAlg",
                                              SiPropertiesTool = ToolSvc.PixelSiPropertiesTool,
                                              UseMagFieldCache = True,
                                              UseMagFieldDcs = True)

    if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
        from SiLorentzAngleTool.SiLorentzAngleToolConf import SiLorentzAngleTool
        ToolSvc += SiLorentzAngleTool(name="PixelLorentzAngleTool", 
                                      DetectorName="Pixel", 
                                      DetEleCollKey="PixelDetectorElementCollection",
                                      UseMagFieldCache=True,
                                      SiLorentzAngleCondData="PixelSiLorentzAngleCondData")

############################################################################################
# Set up Tool/Service
############################################################################################

    #####################
    # Setup Cabling Svc #
    #####################
    from PixelReadoutGeometry.PixelReadoutGeometryConf import InDetDD__PixelReadoutManager
    PixelReadoutManager = InDetDD__PixelReadoutManager("PixelReadoutManager")
    ServiceMgr += PixelReadoutManager
    print (PixelReadoutManager)
    kwargs.setdefault("InputObjectName", "PixelHits")

    chargeTools = []
    chargeTools += ['SensorSimPlanarTool']
    chargeTools += ['SensorSim3DTool']
    feSimTools = []
    feSimTools += ['BarrelFEI4SimTool']
    feSimTools += ['DBMFEI4SimTool']
    feSimTools += ['BarrelFEI3SimTool']
    feSimTools += ['EndcapFEI3SimTool']
    kwargs.setdefault("ChargeTools", chargeTools)
    kwargs.setdefault("FrontEndSimTools", feSimTools)
    kwargs.setdefault("EnergyDepositionTool", "EnergyDepositionTool")
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", Pixel_FirstXing() )
        kwargs.setdefault("LastXing", Pixel_LastXing() )
    from AthenaCommon.DetFlags import DetFlags
    if not DetFlags.pileup.any_on():
        kwargs.setdefault("PileUpMergeSvc", '')
        kwargs.setdefault("OnlyUseContainerName", False)
    return CfgMgr.PixelDigitizationTool(name, **kwargs)

def PixelDigitizationTool(name="PixelDigitizationTool", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 0)
    if digitizationFlags.PileUpPresampling and 'LegacyOverlay' not in digitizationFlags.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("RDOCollName", overlayFlags.bkgPrefix() + "PixelRDOs")
        kwargs.setdefault("SDOCollName", overlayFlags.bkgPrefix() + "PixelSDO_Map")
    else:
        kwargs.setdefault("RDOCollName", "PixelRDOs")
        kwargs.setdefault("SDOCollName", "PixelSDO_Map")
    return BasicPixelDigitizationTool(name, **kwargs)

def PixelGeantinoTruthDigitizationTool(name="PixelGeantinoTruthDigitizationTool", **kwargs):
    kwargs.setdefault("ParticleBarcodeVeto", 0)
    return PixelDigitizationTool(name, **kwargs)

def PixelDigitizationToolHS(name="PixelDigitizationToolHS", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 1)
    return BasicPixelDigitizationTool(name, **kwargs)

def PixelDigitizationToolPU(name="PixelDigitizationToolPU", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 2)
    kwargs.setdefault("RDOCollName", "Pixel_PU_RDOs")
    kwargs.setdefault("SDOCollName", "Pixel_PU_SDO_Map")
    return BasicPixelDigitizationTool(name, **kwargs)

def PixelDigitizationToolSplitNoMergePU(name="PixelDigitizationToolSplitNoMergePU", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 0)
    kwargs.setdefault("InputObjectName", "PileupPixelHits")
    kwargs.setdefault("RDOCollName", "Pixel_PU_RDOs")
    kwargs.setdefault("SDOCollName", "Pixel_PU_SDO_Map")
    return BasicPixelDigitizationTool(name, **kwargs)

def PixelOverlayDigitizationTool(name="PixelOverlayDigitizationTool",**kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("OnlyUseContainerName", False)
        kwargs.setdefault("RDOCollName", overlayFlags.sigPrefix() + "PixelRDOs")
        kwargs.setdefault("SDOCollName", overlayFlags.sigPrefix() + "PixelSDO_Map")
    else:
        kwargs.setdefault("RDOCollName", overlayFlags.evtStore() + "+PixelRDOs")
        kwargs.setdefault("SDOCollName", overlayFlags.evtStore() + "+PixelSDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return BasicPixelDigitizationTool(name,**kwargs)

def getPixelRange(name="PixelRange" , **kwargs):
    kwargs.setdefault('FirstXing', Pixel_FirstXing() )
    kwargs.setdefault('LastXing',  Pixel_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["SiHitCollection#PixelHits"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)

def PixelDigitizationHS(name="PixelDigitizationHS",**kwargs):
    kwargs.setdefault("DigitizationTool", "PixelDigitizationToolHS")
    return CfgMgr.PixelDigitization(name,**kwargs)

def PixelDigitizationPU(name="PixelDigitizationPU",**kwargs):
    kwargs.setdefault("DigitizationTool", "PixelDigitizationToolPU")
    return CfgMgr.PixelDigitization(name,**kwargs)

def PixelOverlayDigitization(name="PixelOverlayDigitization",**kwargs):
    kwargs.setdefault("DigitizationTool", "PixelOverlayDigitizationTool")
    # Multi-threading settinggs
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    if is_hive:
        kwargs.setdefault('Cardinality', concurrencyProps.ConcurrencyFlags.NumThreads())
        # Set common overlay extra inputs
        kwargs.setdefault("ExtraInputs", [("McEventCollection", "TruthEvent")])
    return CfgMgr.PixelDigitization(name,**kwargs)
