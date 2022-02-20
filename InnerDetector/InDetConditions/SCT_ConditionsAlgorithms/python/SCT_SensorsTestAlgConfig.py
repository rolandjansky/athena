"""Define method to configure and test SCT_SensorsTestAlg

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCT_MajorityConditionsTestAlgCfg(flags, name="SCT_SensorsTestAlg", **kwargs):
    """Return a configured SCT_SensorsTestAlg"""
    acc = ComponentAccumulator()
    from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_SensorsCfg
    kwargs.setdefault("SCT_SensorsTool", acc.popToolsAndMerge(SCT_SensorsCfg(flags, **kwargs)))
    acc.addEventAlgo(CompFactory.SCT_SensorsTestAlg(name))
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
    ConfigFlags.Input.ProjectName = "data12_8TeV"
    ConfigFlags.Input.RunNumber = 142913
    ConfigFlags.Input.TimeStamp = 1260630000 # During run 142913, 12/12/2009 @ 3:00pm (UTC)
    ConfigFlags.IOVDb.GlobalTag = "COMCOND-BLKPA-RUN1-09"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R1-2012-03-00-00"
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge(McEventSelectorCfg(ConfigFlags))

    kwargs = {}
    kwargs["folderTag"] = "SctSensors-Sep03-14"

    cfg.merge(SCT_MajorityConditionsTestAlgCfg(ConfigFlags, **kwargs))

    cfg.run(maxEvents=20)
