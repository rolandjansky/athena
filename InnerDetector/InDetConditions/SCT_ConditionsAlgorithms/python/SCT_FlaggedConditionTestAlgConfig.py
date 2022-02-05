"""Define method to configure and test SCT_FlaggedConditionTestAlg

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCT_FlaggedConditionTestAlgCfg(flags, name="SCT_FlaggedConditionTestAlg", **kwargs):
    """Return a configured SCT_FlaggedConditionTestAlg"""
    acc = ComponentAccumulator()
    from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_FlaggedConditionToolCfg
    kwargs.setdefault("FlaggedConditionTool", acc.popToolsAndMerge(SCT_FlaggedConditionToolCfg(flags)))
    acc.addEventAlgo(CompFactory.SCT_FlaggedConditionTestAlg(**kwargs))
    return acc

if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.isMC = False
    ConfigFlags.Input.Files = ["./myESD.pool.root"]
    ConfigFlags.Input.ProjectName = "data17_13TeV" # q431 input
    ConfigFlags.Input.RunNumber = 330470 # q431 input
    ConfigFlags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2018-03" # q431 setup
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01" # q431 setup
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    algkwargs = {}
    algkwargs["OutputLevel"] = INFO
    cfg.merge(SCT_FlaggedConditionTestAlgCfg(ConfigFlags, **algkwargs))

    cfg.run(maxEvents=25)
