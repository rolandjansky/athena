"""Define method to configure and test SCT_ConditionsParameterTestAlg

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCT_ConditionsParameterTestAlgCfg(flags, name="SCT_ConditionsParameterTestAlg", **kwargs):
    """Return a configured SCT_ConditionsParameterTestAlg"""
    acc = ComponentAccumulator()
    from SCT_ConditionsTools.SCT_ConditionsParameterConfig import SCT_ConditionsParameterCfg
    kwargs.setdefault("SCT_ConditionsParameterTool", acc.popToolsAndMerge(SCT_ConditionsParameterCfg(flags)))
    acc.addEventAlgo(CompFactory.SCT_ConditionsParameterTestAlg(**kwargs))
    return acc

if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.isMC = False
    ConfigFlags.Input.ProjectName = "data12_8TeV"
    ConfigFlags.Input.RunNumber = 215643
    ConfigFlags.addFlag("Input.InitialTimeStamp", 1354748400) # LB 469 of run 215643, 2012-12-05 @ 11:00pm (UTC)
    ConfigFlags.IOVDb.GlobalTag = "COMCOND-BLKPA-RUN1-09"
    ConfigFlags.IOVDb.DatabaseInstance = "COMP200"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R1-2012-03-00-00"
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge(McEventSelectorCfg(ConfigFlags))

    cfg.merge(SCT_ConditionsParameterTestAlgCfg(ConfigFlags))

    cfg.run(maxEvents=20)
