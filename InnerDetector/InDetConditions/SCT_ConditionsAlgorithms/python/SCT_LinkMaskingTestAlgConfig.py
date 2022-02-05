"""Define method to configure and test SCT_LinkMaskingTestAlg

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCT_LinkMaskingTestAlgCfg(flags, name="SCT_LinkMaskingTestAlg", **kwargs):
    """Return a configured SCT_LinkMaskingTestAlg"""
    acc = ComponentAccumulator()
    from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_LinkMaskingCfg
    kwargs.setdefault("LinkMaskingTool", acc.popToolsAndMerge(SCT_LinkMaskingCfg(flags)))
    acc.addEventAlgo(CompFactory.SCT_LinkMaskingTestAlg(name, **kwargs))
    return acc

if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = []
    ConfigFlags.Input.isMC = False
    ConfigFlags.Input.ProjectName = "data16_13TeV"
    ConfigFlags.Input.RunNumber = 310809
    ConfigFlags.Input.TimeStamp = 1476741326 # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)
    ConfigFlags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2017-10"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge(McEventSelectorCfg(ConfigFlags))

    cfg.merge(SCT_LinkMaskingTestAlgCfg(ConfigFlags))

    cfg.run(maxEvents=20)
