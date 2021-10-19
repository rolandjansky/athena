"""Define method to configure and test SCT_AlignCondAlg

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeoModelCfg
from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeoModelCfg


def SCT_AlignCondAlgCfg(flags, name="SCT_AlignCondAlg", **kwargs):
    """Return a configured SCT_AlignCondAlg"""
    acc = SCT_GeoModelCfg(flags)
    if flags.GeoModel.Align.Dynamic:
        acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/AlignL1/ID", "/Indet/AlignL1/ID", className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/AlignL2/SCT", "/Indet/AlignL2/SCT", className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/AlignL3", "/Indet/AlignL3", className="AlignableTransformContainer"))
    else:
        acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align", className="AlignableTransformContainer"))

    kwargs.setdefault("UseDynamicAlignFolders", flags.GeoModel.Align.Dynamic)

    sctAlignCondAlg = CompFactory.SCT_AlignCondAlg(name, **kwargs)
    acc.addCondAlgo(sctAlignCondAlg)
    return acc


def ITkStripAlignCondAlgCfg(flags, name="ITkStripAlignCondAlg", **kwargs):
    """Return a configured SCT_AlignCondAlg for ITk"""
    acc = ITkStripGeoModelCfg(flags)
    if flags.GeoModel.Align.Dynamic:
        raise RuntimeError("Dynamic alignment not supported for ITk yet")
    else:
        acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align", className="AlignableTransformContainer"))

    kwargs.setdefault("DetManagerName", "ITkStrip")
    kwargs.setdefault("WriteKey", "ITkStripAlignmentStore")
    kwargs.setdefault("UseDynamicAlignFolders", flags.GeoModel.Align.Dynamic)

    sctAlignCondAlg = CompFactory.SCT_AlignCondAlg(name, **kwargs)
    acc.addCondAlgo(sctAlignCondAlg)
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
    cfg.merge(SCT_AlignCondAlgCfg(ConfigFlags, **algkwargs))

    cfg.run(maxEvents=25)
