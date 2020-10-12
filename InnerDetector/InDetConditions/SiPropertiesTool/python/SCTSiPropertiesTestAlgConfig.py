"""Define method to configure and test SCTSiPropertiesTestAlg

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCTSiPropertiesTestAlgCfg(flags, name="SCTSiPropertiesTestAlg", **kwargs):
    """Return a configured SCTSiPropertiesTestAlg"""
    acc = ComponentAccumulator()
    from SiPropertiesTool.SCT_SiPropertiesConfig import SCT_SiPropertiesCfg
    kwargs.setdefault("SCTPropertiesTool", acc.popToolsAndMerge(SCT_SiPropertiesCfg(flags)))
    acc.addEventAlgo(CompFactory.SCTSiPropertiesTestAlg(**kwargs))
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
    ConfigFlags.Input.RunNumber = [300000] # MC16c 2017 run number
    ConfigFlags.addFlag("Input.InitialTimeStamp", 1500000000) # MC16c 2017 time stamp
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-18"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2015-03-01-00"
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge(McEventSelectorCfg(ConfigFlags))

    cfg.merge(SCTSiPropertiesTestAlgCfg(ConfigFlags))

    cfg.run(maxEvents=20)
