# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# The earliest bunch crossing time for which interactions will be sent
# to the SCT Digitization code.
def SCT_FirstXing():
    return -50
# The latest bunch crossing time for which interactions will be sent
# to the SCT Digitization code.
def SCT_LastXing():
    return 25

def getSCT_SurfaceChargesGenerator(name="SCT_SurfaceChargesGenerator", **kwargs):
    ## Set up services used by SCT_SurfaceChargesGenerator
    ## TODO remove all this stuff and see if PixelDigitization works without it.
    # Setup the DCS folders and Svc used in the sctSiliconConditionsSvc
    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested('/SCT/DCS/CHANSTAT'):
        conddb.addFolder("DCS_OFL","/SCT/DCS/CHANSTAT")
    if not conddb.folderRequested('/SCT/DCS/MODTEMP'):
        conddb.addFolder("DCS_OFL","/SCT/DCS/MODTEMP")
    if not conddb.folderRequested('/SCT/DCS/HV'):
        conddb.addFolder("DCS_OFL","/SCT/DCS/HV")
    ## SCT_DCSConditionsSvc - used by SCT_SurfaceChargesGenerator
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr(ServiceMgr, "InDetSCT_DCSConditionsSvc"):
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsSvc
        InDetSCT_DCSConditionsSvc = SCT_DCSConditionsSvc(name = "InDetSCT_DCSConditionsSvc")
        ServiceMgr += InDetSCT_DCSConditionsSvc
    ## SCT_SiPropertiesSvc - used by SCT_SurfaceChargesGenerator
    if not hasattr(ServiceMgr, "SCT_SiPropertiesSvc"):
        # Lorentz Angle Service
        from SiLorentzAngleSvc.LorentzAngleSvcSetup import lorentzAngleSvc
        # Silicon conditions service (set up by LorentzAngleSvcSetup)
        sctSiliconConditionsSvc = ServiceMgr.SCT_SiliconConditionsSvc
        # Silicon properties service
        from SiPropertiesSvc.SiPropertiesSvcConf import SiPropertiesSvc;
        sctSiPropertiesSvc = SiPropertiesSvc(name = "SCT_SiPropertiesSvc",
                                             DetectorName="SCT",
                                             SiConditionsServices = sctSiliconConditionsSvc)
        ServiceMgr += sctSiPropertiesSvc
    ## Charge trapping service - used by SCT_SurfaceChargesGenerator
    if not hasattr(ServiceMgr, "InDetSCT_RadDamageSummarySvc"):
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_RadDamageSummarySvc
        InDetSCT_RadDamageSummarySvc = SCT_RadDamageSummarySvc(name = "InDetSCT_RadDamageSummarySvc")
        ServiceMgr += InDetSCT_RadDamageSummarySvc
    ## END OF JUNK
    from SCT_Digitization.SCT_DigitizationConf import SCT_SurfaceChargesGenerator
    return SCT_SurfaceChargesGenerator(name, **kwargs)

def getSCT_FrontEnd(name="SCT_FrontEnd", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    #Setup noise treament in SCT_FrontEnd
    # If noise is turned off:
    if not digitizationFlags.doInDetNoise.get_Value():
        ###kwargs.setdefault("OnlyHitElements", True)
        print 'SCT_Digitization:::: Turned off Noise in SCT_FrontEnd'
        kwargs.setdefault("NoiseBarrel", 0.0)
        kwargs.setdefault("NoiseBarrel3", 0.0)
        kwargs.setdefault("NoiseInners", 0.0)
        kwargs.setdefault("NoiseMiddles", 0.0)
        kwargs.setdefault("NoiseShortMiddles", 0.0)
        kwargs.setdefault("NoiseOuters", 0.0)
        kwargs.setdefault("NOBarrel", 0.0)
        kwargs.setdefault("NOBarrel3", 0.0)
        kwargs.setdefault("NOInners", 0.0)
        kwargs.setdefault("NOMiddles", 0.0)
        kwargs.setdefault("NOShortMiddles", 0.0)
        kwargs.setdefault("NOOuters", 0.0)
        kwargs.setdefault("NoiseOn", False)
    # To set the mean noise values for the different module types
    # Default values set at 0 degrees, plus/minus ~5 enc per plus/minus degree
    else:
        kwargs.setdefault("NoiseBarrel", 1500.0)
        kwargs.setdefault("NoiseBarrel3", 1541.0)
        kwargs.setdefault("NoiseInners", 1090.0)
        kwargs.setdefault("NoiseMiddles", 1557.0)
        kwargs.setdefault("NoiseShortMiddles", 940.0)
        kwargs.setdefault("NoiseOuters", 1618.0)
        kwargs.setdefault("NOBarrel", 1.5e-5)
        kwargs.setdefault("NOBarrel3", 2.1e-5)
        kwargs.setdefault("NOInners", 5.0e-9)
        kwargs.setdefault("NOMiddles", 2.7e-5)
        kwargs.setdefault("NOShortMiddles", 2.0e-9)
        kwargs.setdefault("NOOuters", 3.5e-5)
        kwargs.setdefault("NoiseOn", True)
    # Use Calibration data from Conditions DB, still for testing purposes only
    kwargs.setdefault("UseCalibData", True)
    # Setup the ReadCalibChip folders and Svc
    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested('/SCT/DAQ/Calibration/ChipGain'):
        conddb.addFolderSplitMC("SCT","/SCT/DAQ/Calibration/ChipGain","/SCT/DAQ/Calibration/ChipGain")
    if not conddb.folderRequested('/SCT/DAQ/Calibration/ChipNoise'):
        conddb.addFolderSplitMC("SCT","/SCT/DAQ/Calibration/ChipNoise","/SCT/DAQ/Calibration/ChipNoise")
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr(ServiceMgr, "InDetSCT_ReadCalibChipDataSvc"):
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibChipDataSvc
        InDetSCT_ReadCalibChipDataSvc = SCT_ReadCalibChipDataSvc(name = "InDetSCT_ReadCalibChipDataSvc")
        ServiceMgr += InDetSCT_ReadCalibChipDataSvc
    # DataCompressionMode: 1 is level mode x1x (default), 2 is edge mode 01x, 3 is expanded any hit xxx
    from AthenaCommon.BeamFlags import jobproperties
    if (jobproperties.Beam.bunchSpacing() <= 25):
       kwargs.setdefault("DataCompressionMode", 2)
    elif (jobproperties.Beam.bunchSpacing() <= 50):
       kwargs.setdefault("DataCompressionMode", 1)
    else:
       kwargs.setdefault("DataCompressionMode", 3)
       kwargs.setdefault("NoiseExpandedMode", True)
    from SCT_Digitization.SCT_DigitizationConf import SCT_FrontEnd
    return SCT_FrontEnd(name, **kwargs)

def commonSCT_DigitizationConfig(name,**kwargs):

    from Digitization.DigitizationFlags import digitizationFlags
    # If noise is turned off:
    if not digitizationFlags.doInDetNoise.get_Value():
        kwargs.setdefault("OnlyHitElements", True)

    kwargs.setdefault("InputObjectName", "SCT_Hits")
    kwargs.setdefault("EnableHits", True)

    # Use of random disabled cells
    #kwargs.setdefault("RandomDisabledCells", True)

    # Set FixedTime for cosmics for use in SurfaceChargesGenerator
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType == "cosmics" :
        kwargs.setdefault("CosmicsRun", True)
        kwargs.setdefault("FixedTime", 10)

    # write out SCT1_RawData
    #kwargs.setdefault("WriteSCT1_RawData", False)

    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc() )

    if digitizationFlags.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", SCT_FirstXing())
        kwargs.setdefault("LastXing",  SCT_LastXing() )

    from AthenaCommon import CfgMgr
    return CfgMgr.SCT_DigitizationTool(name,**kwargs)
#    else:
#        from AthenaCommon import CfgMgr
#        return CfgMgr.SCT_Digitization(name, **kwargs)

def GenericSCT_DigitizationTool(name,**kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    digitizationFlags.rndmSeedList.addSeed("SCT_Digitization", 49261510, 105132394 )
    return commonSCT_DigitizationConfig(name,**kwargs)

######################################################################################
def SCT_DigitizationTool(name="SCT_DigitizationTool", **kwargs):
    kwargs.setdefault("OutputObjectName", "SCT_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return GenericSCT_DigitizationTool(name,**kwargs)

######################################################################################

def SCT_DigitizationToolHS(name="SCT_DigitizationToolHS",**kwargs):
    kwargs.setdefault("OutputObjectName", "SCT_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 1)
    return GenericSCT_DigitizationTool(name,**kwargs)

######################################################################################

def SCT_DigitizationToolPU(name="SCT_DigitizationToolPU",**kwargs):
    kwargs.setdefault("OutputObjectName", "SCT_PU_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_PU_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 2)
    return commonSCT_DigitizationConfig(name,**kwargs)

######################################################################################

def getSiliconRange(name="SiliconRange" , **kwargs):
    #this is the time of the xing in ns
    kwargs.setdefault('FirstXing', SCT_FirstXing() )
    kwargs.setdefault('LastXing',  SCT_LastXing()  )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["SiHitCollection#SCT_Hits"] )
    from AthenaCommon import CfgMgr
    return CfgMgr.PileUpXingFolder(name, **kwargs)

######################################################################################

def SCT_DigitizationHS(name="SCT_DigitizationHS",**kwargs):
    kwargs.setdefault("DigitizationTool", "SCT_DigitizationToolHS")
    from SCT_Digitization.SCT_DigitizationConf import SCT_Digitization
    return SCT_Digitization(name,**kwargs)

######################################################################################

def SCT_DigitizationPU(name="SCT_DigitizationPU",**kwargs):
    kwargs.setdefault("DigitizationTool", "SCT_DigitizationToolPU")
    from SCT_Digitization.SCT_DigitizationConf import SCT_Digitization
    return SCT_Digitization(name,**kwargs)
