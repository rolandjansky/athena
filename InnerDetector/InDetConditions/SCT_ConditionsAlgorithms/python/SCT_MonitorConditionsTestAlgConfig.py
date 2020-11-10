"""Define method to configure and test SCT_MonitorConditionsTestAlg

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCT_MonitorConditionsTestAlgCfg(flags, name="SCT_MonitorConditionsTestAlg", **kwargs):
    """Return a configured SCT_MonitorConditionsTestAlg"""
    acc = ComponentAccumulator()
    from InDetConfig.InDetRecToolConfig import SCT_MonitorConditionsToolCfg
    kwargs.setdefault("SCT_MonitorConditionsTool", acc.popToolsAndMerge(SCT_MonitorConditionsToolCfg(flags)))
    acc.addEventAlgo(CompFactory.SCT_MonitorConditionsTestAlg(**kwargs))
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
    ConfigFlags.Input.RunNumber = 310000 # MC16e 2018 run number
    ConfigFlags.addFlag("Input.InitialTimeStamp", 1550000000) # MC16e 2018 time stamp
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-RUN2-01"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge(McEventSelectorCfg(ConfigFlags))

    algkwargs = {}
    algkwargs["OutputLevel"] = INFO
    cfg.merge(SCT_MonitorConditionsTestAlgCfg(ConfigFlags, **algkwargs))

    cfg.run(maxEvents=20)
