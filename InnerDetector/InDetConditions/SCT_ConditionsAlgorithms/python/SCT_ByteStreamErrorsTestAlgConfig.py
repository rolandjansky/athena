"""Define method to configure and test SCT_ByteStreamErrorsTestAlg

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCT_ByteStreamErrorsTestAlgCfg(flags, name="SCT_ByteStreamErrorsTestAlg", **kwargs):
    """Return a configured SCT_ByteStreamErrorsTestAlg"""
    acc = ComponentAccumulator()
    from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ByteStreamErrorsToolCfg
    kwargs.setdefault("ByteStreamErrorsTool", acc.popToolsAndMerge(SCT_ByteStreamErrorsToolCfg(flags)))
    acc.addEventAlgo(CompFactory.SCT_ByteStreamErrorsTestAlg(name, **kwargs))
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
    cfg.merge(SCT_ByteStreamErrorsTestAlgCfg(ConfigFlags, **algkwargs))

    cfg.run(maxEvents=25)
