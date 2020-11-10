"""Define method to configure and test SCT_ReadCalibDataTestAlg

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCT_ReadCalibDataTestAlgCfg(flags, name="SCT_ReadCalibDataTestAlg", **kwargs):
    """Return a configured SCT_ReadCalibDataTestAlg"""
    acc = ComponentAccumulator()
    from InDetConfig.InDetRecToolConfig import SCT_ReadCalibDataToolCfg
    kwargs.setdefault("SCT_ReadCalibDataTool", acc.popToolsAndMerge(SCT_ReadCalibDataToolCfg(flags)))
    acc.addEventAlgo(CompFactory.SCT_ReadCalibDataTestAlg(**kwargs))
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

    algkwargs = {}
    ## Modules to test:
    algkwargs["ModuleOfflinePosition"] = [2, 1, 2, 12, 1, 464] #(EC/B, Disk/Layer, eta, phi, side, strip)
    algkwargs["DoTestmyConditionsSummary"] = True
    algkwargs["DoTestmyDefectIsGood"] = True
    algkwargs["OutputLevel"] = INFO
    cfg.merge(SCT_ReadCalibDataTestAlgCfg(ConfigFlags, **algkwargs))

    cfg.run(maxEvents=20)
