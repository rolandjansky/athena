"""Define method to configure and test SCT_SiliconConditionsTestAlgConfig

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCT_SiliconConditionsTestAlgCfg(flags, name="SCT_SiliconConditionsTestAlg", **kwargs):
    """Return a configured SCT_SiliconConditionsTestAlg"""
    acc = ComponentAccumulator()
    from SCT_ConditionsTools.SCT_SiliconConditionsConfig import SCT_SiliconConditionsCfg
    kwargs.setdefault("SCT_SiliconConditionsTool", acc.popToolsAndMerge(SCT_SiliconConditionsCfg(flags)))
    acc.addEventAlgo(CompFactory.SCT_SiliconConditionsTestAlg(**kwargs))
    return acc

if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.isMC = True
    ConfigFlags.Input.ProjectName = "mc16_13TeV"
    ConfigFlags.Input.RunNumber = 300000 # MC16c 2017 run number
    ConfigFlags.addFlag("Input.InitialTimeStamp", 1500000000) # MC16c 2017 time stamp
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-18"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2015-03-01-00"
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge(McEventSelectorCfg(ConfigFlags))

    cfg.merge(SCT_SiliconConditionsTestAlgCfg(ConfigFlags))

    cfg.run(maxEvents=20)
