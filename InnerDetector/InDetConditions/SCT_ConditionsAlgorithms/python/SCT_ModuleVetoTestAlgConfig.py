"""Define method to configure and test SCT_ModuleVetoTestAlg

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCT_StripVetoTestAlgCfg(flags, name="SCT_ModuleVetoTestAlg", **kwargs):
    """Return a configured SCT_ModuleVetoTestAlg"""
    acc = ComponentAccumulator()
    from SCT_ConditionsTools.SCT_ModuleVetoConfig import SCT_ModuleVetoCfg
    acc.addEventAlgo(CompFactory.SCT_ModuleVetoTestAlg(name=name,
                                                       ModuleVetoTool=acc.popToolsAndMerge(SCT_ModuleVetoCfg(flags, **kwargs))))
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

    kwargs = {}

    ### Use COOL database for SCT_ModuleVetoTool
    kwargs["useDB"] = True # False
    if kwargs["useDB"]:
        kwargs["folderTag"] = "SCTManualBadModules-000-00"
        kwargs["BadModuleIdentifiers"] = ["database"]
    else:
        kwargs["BadModuleIdentifiers"] = ["1", "2"]

    cfg.merge(SCT_StripVetoTestAlgCfg(ConfigFlags, **kwargs))

    cfg.run(maxEvents=20)
