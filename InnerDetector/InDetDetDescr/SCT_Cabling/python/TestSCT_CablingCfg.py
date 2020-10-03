# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/AthenaJobConfigRun3

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def SCT_TestCablingAlgCfg(configFlags):
    cfg=ComponentAccumulator()

    from SCT_Cabling.SCT_CablingConfig import SCT_CablingCondAlgCfg
    cfg.merge(SCT_CablingCondAlgCfg(configFlags))

    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    geoCfg=GeoModelCfg(configFlags)
    cfg.merge(geoCfg)

    from AthenaCommon.Constants import INFO
    SCT_CablingTool = CompFactory.SCT_CablingTool()
    SCT_CablingTool.DataSource = "COOLVECTOR"
    SCT_CablingTool.OutputLevel = INFO

    SCT_TestCablingAlg=CompFactory.SCT_TestCablingAlg
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
    ConfigFlags.Input.RunNumber = [300000]
    ConfigFlags.addFlag("Input.InitialTimeStamp", 1500000000)
    # https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConditionsRun1RunNumbers
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-RUN12-SDR-25"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2015-03-01-00"
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg=MainServicesCfg(ConfigFlags)

    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge(McEventSelectorCfg(ConfigFlags))

    cfg.merge(SCT_TestCablingAlgCfg(ConfigFlags))

    cfg.run(maxEvents=20)
