# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def McEventSelectorCfg(configFlags):
    cfg=ComponentAccumulator()

    from AthenaCommon import AtlasUnixStandardJob

    from McEventSelector.McEventSelectorConf import McCnvSvc
    mcCnvSvc = McCnvSvc()
    cfg.addService(mcCnvSvc)
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc
    cfg.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[mcCnvSvc.getFullJobOptName(),]))

    from McEventSelector.McEventSelectorConf import McEventSelector
    evSel=McEventSelector("EventSelector")
    evSel.RunNumber = configFlags.Input.RunNumber
    evSel.InitialTimeStamp = configFlags.Input.InitialTimeStamp
    cfg.addService(evSel)
    cfg.setAppProperty("EvtSel",evSel.getFullJobOptName())

    return cfg


def SCT_CablingFoldersCfg(configFlags):
    cfg=ComponentAccumulator()

    path="/SCT/DAQ/Config/"
    if configFlags.IOVDb.DatabaseInstance == "COMP200":
        path="/SCT/DAQ/Configuration/"

    instance="SCT"
    if ConfigFlags.Input.isMC:
        instance="SCT_OFL"

    from IOVDbSvc.IOVDbSvcConfig import addFolders, IOVDbSvcCfg
    cfg.merge(addFolders(configFlags, [path+"ROD", path+"RODMUR", path+"MUR", path+"Geog"], instance, className="CondAttrListVec")[0])

    return cfg, path


def SCT_CablingCondAlgCfg(configFlags):
    cfg=ComponentAccumulator()

    foldersCfg,path=SCT_CablingFoldersCfg(configFlags)
    cfg.merge(foldersCfg)

    from SCT_Cabling.SCT_CablingConf import SCT_CablingCondAlgFromCoraCool
    cfg.addCondAlgo(SCT_CablingCondAlgFromCoraCool(ReadKeyRod=path+"ROD",
                                                   ReadKeyRodMur=path+"RODMUR",
                                                   ReadKeyMur=path+"MUR",
                                                   ReadKeyGeo=path+"Geog"))

    return cfg


def SCT_TestCablingAlgCfg(configFlags):
    cfg=ComponentAccumulator()

    cfg.merge(SCT_CablingCondAlgCfg(configFlags))

    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    geoCfg,gms=GeoModelCfg(configFlags)
    cfg.merge(geoCfg)

    from AthenaCommon.Constants import INFO
    from AthenaCommon.CfgGetter import getPrivateTool
    SCT_CablingTool = getPrivateTool("SCT_CablingTool")
    SCT_CablingTool.DataSource = "COOLVECTOR"
    SCT_CablingTool.OutputLevel = INFO

    from SCT_Cabling.SCT_CablingConf import SCT_TestCablingAlg
    testAlg = SCT_TestCablingAlg(SCT_CablingTool = SCT_CablingTool,
                                 OutputLevel = INFO)
    cfg.addEventAlgo(testAlg)

    return cfg


if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.isMC = True
    ConfigFlags.Input.RunNumber = 300000
    ConfigFlags.addFlag("Input.InitialTimeStamp", 1500000000)
    # https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConditionsRun1RunNumbers
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-RUN12-SDR-25"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2015-03-01-00"
    ConfigFlags.lock()

    cfg=ComponentAccumulator()
    cfg.merge(McEventSelectorCfg(ConfigFlags))
    cfg.merge(SCT_TestCablingAlgCfg(ConfigFlags))

    f=open("TestSCT_CablingCfg.pkl","w")
    cfg.store(f)
    f.close()
