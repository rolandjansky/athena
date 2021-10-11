# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def EMVertexBuilderCfg(flags, name="EMVertexBuilder", **kwargs):
    acc = ComponentAccumulator()

    if "ExtrapolationTool" not in kwargs:
        from egammaTrackTools.egammaTrackToolsConfig import EMExtrapolationToolsCfg
        kwargs["ExtrapolationTool"] = acc.popToolsAndMerge(EMExtrapolationToolsCfg(flags))
    if "VertexFinderTool" not in kwargs:        
        vtxFlags = flags.cloneAndReplace("InDet.SecVertex", "InDet.SecVertexEGammaPileUp" )
        from InDetConfig.VertexFindingConfig import ConversionFinderCfg
        kwargs["VertexFinderTool"] = acc.popToolsAndMerge( ConversionFinderCfg(vtxFlags) )

    alg = CompFactory.EMVertexBuilder(name, **kwargs)
    acc.addEventAlgo(alg)
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RAW
    acc = MainServicesCfg(flags)
    acc.merge(EMVertexBuilderCfg(flags))
    with open("vertexbuilder.pkl", "wb") as f:
        acc.store(f)
    