"""ComponentAccumulator configuration utilities for LAr Raw Conditions

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from IOVDbSvc.IOVDbSvcConfig import addFolders
from LArCabling.LArCablingConfig import LArIdMapCfg
from LArBadChannelTool.LArBadChannelConfig import LArBadChannelCfg, LArBadFebCfg
from LArRecUtils.LArRecUtilsConfig import LArMCSymCondAlgCfg
from LArBadChannelTool.LArBadChannelToolConf import LArBadFebCondAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArDAC2uAMC_LArDAC2uASym_ as LArDAC2uASymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMinBiasAverageMC_LArMinBiasAverageSym_ as LArMinBiasAverageSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMinBiasMC_LArMinBiasSym_ as LArMinBiasSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArNoiseMC_LArNoiseSym_ as LArNoiseSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArRampMC_LArRampSym_ as LArRampSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArfSamplMC_LArfSamplSym_ as LArfSamplSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LAruA2MeVMC_LAruA2MeVSym_ as LAruA2MeVSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArAutoCorrMC_LArAutoCorrSym_ as LArAutoCorrSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArShape32MC_LArShape32Sym_ as LArShapeSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMphysOverMcalMC_LArMphysOverMcalSym_ as LArMPhysOverMcalSymAlg
from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArHVScaleCorrFlat_ as LArHVScaleCorrCondFlatAlg
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LAruA2MeVFlat_ as LAruA2MeVCondAlg
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArDAC2uAFlat_ as LArDAC2uACondAlg
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArPedestalFlat_ as LArPedestalCondAlg
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArRampFlat_ as LArRampCondAlg
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArMphysOverMcalFlat_ as LArMphysOverMcalCondAlg
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArOFCFlat_ as LArOFCCondAlg
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArShapeFlat_ as LArShapeCondAlg

def LArRawConditionsMCCfg(flags):
    """Return ComponentAccumulator configured to access LArRawConditions objects from COOL, for Monte Carlo"""
    # replaces LArConditionsCommon_MC_jobOptions.py
    # config_ElecCalibMC() not migrated
    acc = LArIdMapCfg(flags)
    acc.merge(LArBadChannelCfg(flags))
    acc.merge(LArBadFebCfg(flags))
    acc.addCondAlgo(LArBadFebCondAlg("LArKnownBadFebAlg", ReadKey="", WriteKey="LArKnownBadFEBs"))
    acc.addCondAlgo(LArBadFebCondAlg("LArKnownMNBFebAlg", ReadKey="", WriteKey="LArKnownMNBFEBs"))
    acc.merge(LArMCSymCondAlgCfg(flags))
    # get parameters
    LArDBConnection = flags.LAr.DBConnection
    LArDB = ("LAR_OFL" if LArDBConnection == "" else "")
    # utility
    def addLArFolder(className, folder, key, calg):
        folder = "/LAR/ElecCalibMC/" + folder
        acc.merge(addFolders(flags, folder + LArDBConnection, LArDB, className))
        if calg is not None:
            acc.addCondAlgo(calg(ReadKey=key, WriteKey=key + "Sym"))
        # override not migrated
        # larCondFlags.addTag(fldr, conddb)

    # CondDB folders and algs
    addLArFolder("LArRampMC", "Ramp", "LArRamp", LArRampSymAlg)
    addLArFolder("LArAutoCorrMC", "AutoCorr", "LArAutoCorr", LArAutoCorrSymAlg)
    addLArFolder("LArDAC2uAMC", "DAC2uA", "LArDAC2uA", LArDAC2uASymAlg)
    addLArFolder("LArPedestalMC", "Pedestal", "LArPedestal", None)
    addLArFolder("LArNoiseMC", "Noise", "LArNoise", LArNoiseSymAlg)
    addLArFolder("LArfSamplMC", "fSampl", "LArfSampl", LArfSamplSymAlg)
    addLArFolder("LAruA2MeVMC", "uA2MeV", "LAruA2MeV", LAruA2MeVSymAlg)
    addLArFolder("LArMinBiasMC", "MinBias", "LArMinBias", LArMinBiasSymAlg)
    addLArFolder("LArMinBiasAverageMC", "MinBiasAverage", "LArMinBiasAverage", LArMinBiasAverageSymAlg)
    if flags.LAr.UseMCShape:
        addLArFolder("LArShape32MC", "Shape", "LArShape",LArShapeSymAlg)
    if flags.LAr.HasMphys:
        addLArFolder("LArMphysOverMcalMC", "MphysOverMcal", "LArMphysOverMcal", LArMPhysOverMcalSymAlg)
    if flags.LAr.HasHVCorr:
        addLArFolder("LArHVScaleCorrComplete", "HVScaleCorr", "LArHVScaleCorr", None)
    # override not migrated
    # larCondFlags.addTag("/LAR/ElecCalibMC",conddb)
    return acc

def LArRawConditionsDataCfg(flags):
    """Return ComponentAccumulator configured to access LArRawConditions objects from COOL, for data"""
    # replaces LArConditionsCommon_comm_jobOptions.py
    # this migration replaces flag sets with local variables
    # and so loses the locked override ability 
    acc = LArIdMapCfg(flags)
    # get parameters
    if flags.Input.ProjectName.startswith("data09"):
        CoolChannelSelection = "3:238,306,313,319,325,331,338,344,350,1001:1012,1021,1022"
        OFCShapeFolder = "5samples3bins17phases"
    else:
        CoolChannelSelection = ""
        OFCShapeFolder = ""
    if flags.LAr.ElecCalibSqlite != "":
        # Assume sqlite file contains the online database
        sqlDB = "<db>sqlite://;schema=" + flags.LAr.ElecCalibSqlite + ";dbname=CONDB2</db>"
        ONLDB = ""
    else:
        sqlDB = ""
        ONLDB = "LAR_ONL"
    if flags.LAr.ForceIOVRunNumber == "":
        forceRN = ""
    else:
        forceRN = "<forceRunNumber>" + flags.LAr.ForceIOVRunNumber + "</forceRunNumber>"
    # utility
    def addLArFlatFolder(db, obj, calg, qual=""):
        """Merge folder and calg with given properties"""
        folder = "/LAR/ElecCalibFlat/" + obj
        acc.merge(addFolders(flags, folder + forceRN + qual, db, "CondAttrListCollection"))
        acc.addCondAlgo(calg(ReadKey=folder, WriteKey="LAr" + obj))

    def addLArFolder(db, obj, cls, qual=""):
        folder = "ElecCalib" + ("Ofl" if db.endswith("OFL") else "Onl")
        acc.merge(addFolders(flags, "/LAR/" + folder + "/" + obj + forceRN + qual, db, cls))

    # Bad channels
    rekeyBC="<key>/LAR/BadChannels/BadChannels</key>"
    acc.merge(LArBadChannelCfg(flags, tag=forceRN + rekeyBC))
    rekeyMF="<key>/LAR/BadChannels/MissingFEBs</key>"
    acc.merge(LArBadFebCfg(flags, tag=forceRN + rekeyMF))
    # not migrated reco folders for ESD or RDOTrigger here
    # haveElecCalibInline <=> Run 2
    haveElecCalibInline = (flags.IOVDb.DatabaseInstance == "CONDBR2")
    if haveElecCalibInline:
        theLArCondSvc = LArFlatConditionSvc()
        acc.addService(theLArCondSvc)
        acc.getService("ProxyProviderSvc").ProviderNames += ["LArFlatConditionSvc"]
        # load HVScaleCorr
        addLArFlatFolder(ONLDB, "HVScaleCorr", LArHVScaleCorrCondFlatAlg, sqlDB)
        theLArCondSvc.HVScaleCorrInput = "/LAR/ElecCalibFlat/HVScaleCorr" # TEMPORARY
    # not migrated RecoFlags.rec.readESD here
    # return now if not loading ElecCalib
    if not flags.LAr.LoadElecCalib:
        return acc
    # decide on selection
    if CoolChannelSelection != "":
        selection = "<channelSelection>" + CoolChannelSelection + "</channelSelection>"
    else:
        selection = ""
    # load ElecCalib state:
    # 1. uA2MeV
    # 2. DAC2uA
    # 3. Pedestal
    # 4. Ramp
    # 5. MphysOverMcal
    # 6. HVScaleCorr
    # 7. OFCs
    # 8. Shape
    if haveElecCalibInline:
        # Run 2 case
        addLArFlatFolder("LAR_ONL", "uA2MeV", LAruA2MeVCondAlg)
        addLArFlatFolder("LAR_ONL", "DAC2uA", LArDAC2uACondAlg)
        addLArFlatFolder(ONLDB, "Pedestal", LArPedestalCondAlg, sqlDB)
        addLArFlatFolder(ONLDB, "Ramp", LArRampCondAlg, sqlDB)
        addLArFlatFolder(ONLDB, "MphysOverMcal", LArMphysOverMcalCondAlg, sqlDB)
        # HVScaleCorr moved outside of the if loadElecCalib clause b/c it"s now used by the CaloNoiseTool
        addLArFlatFolder(ONLDB, "OFC", LArOFCCondAlg, sqlDB)
        addLArFolder("LAR_OFL", "OFC/PhysWave/RTM/" + OFCShapeFolder, "LArOFCComplete", selection)
        # TEMPORARY
        theLArCondSvc.uA2MeVInput = "/LAR/ElecCalibFlat/uA2MeV"
        theLArCondSvc.DAC2uAInput = "/LAR/ElecCalibFlat/DAC2uA"
        theLArCondSvc.PedestalInput = "/LAR/ElecCalibFlat/Pedestal"
        theLArCondSvc.RampInput = "/LAR/ElecCalibFlat/Ramp"
        theLArCondSvc.MphysOverMcalInput = "/LAR/ElecCalibFlat/MphysOverMcal"
        theLArCondSvc.OFCInput = "/LAR/ElecCalibFlat/OFC"
        if flags.LAr.UseShape:
            if OFCShapeFolder == "":
                addLArFlatFolder(ONLDB, "Shape", LArShapeCondAlg, sqlDB)
                theLArCondSvc.ShapeInput = "/LAR/ElecCalibFlat/Shape" # TEMPORARY
            else:
                addLArFolder("LAR_OFL", "Shape/RTM/" + OFCShapeFolder, "LArShapeComplete", selection)
    else: 
        # Run 1 case - read some electronic calibration constants from the offline DB
        ua2MeVFolder = "uA2MeV/Symmetry"
        MphysOverMcalFolder = "MphysOverMcal/RTM"
        OFCShapeFolder = "5samples1phase"
        addLArFolder("LAR_OFL", ua2MeVFolder, "LAruA2MeVMC")
        addLArFolder("LAR_ONL", "DAC2uA", "LArDAC2uAMC")
        addLArFolder("LAR_ONL", "Pedestal", "LArPedestalComplete", "<key>LArPedestal</key>")
        addLArFolder("LAR_ONL", "Ramp", "LArRampComplete")
        addLArFolder("LAR_OFL", MphysOverMcalFolder, "LArMphysOverMcalComplete")
        addLArFolder("LAR_ONL", "HVScaleCorr", "LArHVScaleCorrComplete")
        addLArFolder("LAR_OFL", "OFC/PhysWave/RTM/" + OFCShapeFolder, "LArOFCComplete", selection)
        if flags.LAr.UseShape:
            addLArFolder("LAR_OFL", "Shape/RTM/" + OFCShapeFolder, "LArShapeComplete", selection)
    return acc

