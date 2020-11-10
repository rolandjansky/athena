"""Define method to configure and test SCT_TdaqEnabledTestAlg

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCT_TdaqEnabledTestAlgCfg(flags, name="SCT_TdaqEnabledTestAlg", **kwargs):
    """Return a configured SCT_TdaqEnabledTestAlg"""
    acc = ComponentAccumulator()
    from InDetConfig.InDetRecToolConfig import SCT_TdaqEnabledToolCfg
    kwargs.setdefault("SCT_TdaqEnabledTool", acc.popToolsAndMerge(SCT_TdaqEnabledToolCfg(flags)))
    acc.addEventAlgo(CompFactory.SCT_TdaqEnabledTestAlg(**kwargs))
    return acc

if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.isMC = False
    ConfigFlags.Input.ProjectName = "data16_13TeV"
    ConfigFlags.Input.RunNumber = 310809
    ConfigFlags.addFlag("Input.InitialTimeStamp", 1476741326) # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)
    ConfigFlags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2017-06"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge(McEventSelectorCfg(ConfigFlags))

    algkwargs = {}
    algkwargs["OutputLevel"] = INFO
    cfg.merge(SCT_TdaqEnabledTestAlgCfg(ConfigFlags, **algkwargs))

    cfg.run(maxEvents=20)
