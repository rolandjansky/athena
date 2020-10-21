"""Define method to configure and test SCT_ConditionsSummaryTestAlg

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCT_ConditionsSummaryTestAlgCfg(flags, name="SCT_ConditionsSummaryTestAlg", **kwargs):
    """Return a configured SCT_ConditionsSummaryTestAlg"""
    acc = ComponentAccumulator()
    from InDetConfig.InDetRecToolConfig import InDetSCT_ConditionsSummaryToolCfg
    InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(InDetSCT_ConditionsSummaryToolCfg(flags,withFlaggedCondTool=False))
    kwargs.setdefault("SCT_ConditionsSummaryTool", InDetSCT_ConditionsSummaryToolWithoutFlagged)
    acc.addEventAlgo(CompFactory.SCT_ConditionsSummaryTestAlg(**kwargs))
    return acc

if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.isMC = False
    ConfigFlags.Input.ProjectName = "data17_13TeV"
    ConfigFlags.Input.RunNumber = [310809]
    ConfigFlags.addFlag("Input.InitialTimeStamp", 1476741326) # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)
    ConfigFlags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2017-06"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge(McEventSelectorCfg(ConfigFlags))

    cfg.merge(SCT_ConditionsSummaryTestAlgCfg(ConfigFlags))

    cfg.run(maxEvents=20)
