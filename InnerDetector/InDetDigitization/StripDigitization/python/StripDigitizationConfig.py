# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
# The earliest bunch crossing time for which interactions will be sent
# to the SCT Digitization code.
def StripFirstXing():
    return -50
# The latest bunch crossing time for which interactions will be sent
# to the SCT Digitization code.
def StripLastXing():
    return 25
# NOTE: related to 3BC mode?

######################################################################################
def getStripSurfaceChargesGenerator(name="StripSurfaceChargesGenerator", **kwargs):
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

    kwargs.setdefault("FixedTime", -999)
    kwargs.setdefault("SubtractTime", -999)
    kwargs.setdefault("SurfaceDriftTime", 10)
    kwargs.setdefault("NumberOfCharges", 1)
    kwargs.setdefault("SmallStepLength", 5)
    kwargs.setdefault("DepletionVoltage", 70)
    kwargs.setdefault("BiasVoltage", 150)
    from AthenaCommon.GlobalFlags import globalflags
    kwargs.setdefault("isOverlay", globalflags.isOverlay())

    from Digitization.DigitizationFlags import digitizationFlags
    if 'NewMerge' in digitizationFlags.experimentalDigi():
         kwargs.setdefault("UseMcEventCollectionHelper",True)
    else:
        kwargs.setdefault("UseMcEventCollectionHelper",False)
    if 'doDetailedSurfChargesGen' in digitizationFlags.experimentalDigi():
        kwargs.setdefault("ChargeDriftModel", 1)
        kwargs.setdefault("EFieldModel", 2)
        kwargs.setdefault("MagneticField", -2.0)
        kwargs.setdefault("SensorTemperature", 273.15)
        kwargs.setdefault("TransportTimeStep", 0.25)
        kwargs.setdefault("TransportTimeMax", 25.0)
        from StripDigitization.StripDigitizationConf import StripDetailedSurfaceChargesGenerator
        return StripDetailedSurfaceChargesGenerator(name, **kwargs)
    else:
        from StripDigitization.StripDigitizationConf import StripSurfaceChargesGenerator
        return StripSurfaceChargesGenerator(name, **kwargs)


######################################################################################
def getStripFrontEnd(name="StripFrontEnd", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    #Setup noise treament in SCT_FrontEnd
    # To set the mean noise values for the different module types
    # Default values set at 0 degrees, plus/minus ~5 enc per plus/minus degree
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
    # If noise is turned off:
    if not digitizationFlags.doInDetNoise.get_Value():
        ###kwargs.setdefault("OnlyHitElements", True)
        print 'SCT_Digitization:::: Turned off Noise in SCT_FrontEnd'
        kwargs.setdefault("NoiseOn", False)
        kwargs.setdefault("AnalogueNoiseOn", False)
        kwargs.setdefault("AddNoiseDiodes", False)
    else:
        kwargs.setdefault("NoiseOn", True)
        kwargs.setdefault("AnalogueNoiseOn", True)
    # In overlay MC, only analogue noise is on. Noise hits are not added.
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.isOverlay() and globalflags.DataSource == 'geant4':
        kwargs["NoiseOn"] = False
        kwargs["AnalogueNoiseOn"] = True
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
    if digitizationFlags.PileUpPremixing:
        kwargs.setdefault("DataCompressionMode", 3)
    elif globalflags.isOverlay() and globalflags.DataSource == 'geant4':
        kwargs.setdefault("DataCompressionMode", 2)
    elif (jobproperties.Beam.bunchSpacing() <= 50):
        kwargs.setdefault("DataCompressionMode", 1) 
    else: 
        kwargs.setdefault("DataCompressionMode", 3) 
    # DataReadOutMode: 0 is condensed mode and 1 is expanded mode
    if globalflags.isOverlay() and globalflags.DataSource == 'geant4':
        kwargs.setdefault("DataReadOutMode", 0)
    else:
        kwargs.setdefault("DataReadOutMode", 1)
    from StripDigitization.StripDigitizationConf import StripFrontEnd
    return StripFrontEnd(name, **kwargs)

######################################################################################

def commonStripDigitizationConfig(name,**kwargs):

    from Digitization.DigitizationFlags import digitizationFlags
    # If noise is turned off:
    if not digitizationFlags.doInDetNoise.get_Value():
        kwargs.setdefault("OnlyHitElements", True)

    kwargs.setdefault("InputObjectName", "SCT_Hits")
    kwargs.setdefault("EnableHits", True)
    kwargs.setdefault("BarrelOnly", False)

    # Set FixedTime for cosmics for use in SurfaceChargesGenerator
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType == "cosmics" :
        kwargs.setdefault("CosmicsRun", True)
        kwargs.setdefault("FixedTime", 10)

    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc() )
    streamName = "SCT_Digitization" # FIXME ideally random stream name would be configurable
    if not digitizationFlags.rndmSeedList.checkForExistingSeed(streamName):
        digitizationFlags.rndmSeedList.addSeed(streamName, 49261510, 105132394 )

    if digitizationFlags.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", StripFirstXing())
        kwargs.setdefault("LastXing",  StripLastXing() )

    from AthenaCommon import CfgMgr
    return CfgMgr.StripDigitizationTool(name,**kwargs)

######################################################################################
def getStripDigitizationTool(name="StripDigitizationTool", **kwargs):
    kwargs.setdefault("OutputObjectName", "SCT_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return commonStripDigitizationConfig(name,**kwargs)


######################################################################################
def getStripAmp(name="StripAmp", **kwargs):
    kwargs.setdefault("CrossFactor2sides", 0.1)
    kwargs.setdefault("CrossFactorBack", 0.07)
    kwargs.setdefault("PeakTime", 21)
    kwargs.setdefault("deltaT", 1.0)
    kwargs.setdefault("Tmin", -25.0)
    kwargs.setdefault("Tmax", 150.0)
    kwargs.setdefault("NbAverage", 0)
    from StripDigitization.StripDigitizationConf import StripAmp
    return StripAmp(name, **kwargs)


######################################################################################
def getStripFrontEnd(name="StripFrontEnd", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    #Setup noise treament in SCT_FrontEnd
    # To set the mean noise values for the different module types
    # Default values set at 0 degrees, plus/minus ~5 enc per plus/minus degree
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
    # If noise is turned off:
    if not digitizationFlags.doInDetNoise.get_Value():
        ###kwargs.setdefault("OnlyHitElements", True)
        print 'StripDigitization:::: Turned off Noise in StripFrontEnd'
        kwargs.setdefault("NoiseOn", False)
        kwargs.setdefault("AnalogueNoiseOn", False)
        kwargs.setdefault("AddNoiseDiodes", False)
    else:
        kwargs.setdefault("NoiseOn", True)
        kwargs.setdefault("AnalogueNoiseOn", True)
    # In overlay MC, only analogue noise is on. Noise hits are not added.
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.isOverlay() and globalflags.DataSource == 'geant4':
        kwargs["NoiseOn"] = False
        kwargs["AnalogueNoiseOn"] = True
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
    if digitizationFlags.PileUpPremixing:
        kwargs.setdefault("DataCompressionMode", 3)
    elif globalflags.isOverlay() and globalflags.DataSource == 'geant4':
        kwargs.setdefault("DataCompressionMode", 2)
    elif (jobproperties.Beam.bunchSpacing() <= 50):
        kwargs.setdefault("DataCompressionMode", 1) 
    else: 
        kwargs.setdefault("DataCompressionMode", 3) 
    # DataReadOutMode: 0 is condensed mode and 1 is expanded mode
    if globalflags.isOverlay() and globalflags.DataSource == 'geant4':
        kwargs.setdefault("DataReadOutMode", 0)
    else:
        kwargs.setdefault("DataReadOutMode", 1)
    from StripDigitization.StripDigitizationConf import StripFrontEnd
    return StripFrontEnd(name, **kwargs)

