# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging


def EMVertexBuilderCfg(flags, name="EMVertexBuilder", **kwargs):
    acc = ComponentAccumulator()

    if "ExtrapolationTool" not in kwargs:
        from egammaTrackTools.egammaTrackToolsConfig import (
            EMExtrapolationToolsCfg)
        kwargs["ExtrapolationTool"] = acc.popToolsAndMerge(
            EMExtrapolationToolsCfg(flags))
    if "VertexFinderTool" not in kwargs:
        vtxFlags = flags.cloneAndReplace(
            "InDet.SecVertex", "InDet.SecVertexEGammaPileUp")
        from InDetConfig.ConversionFindingConfig import ConversionFinderCfg
        kwargs["VertexFinderTool"] = acc.popToolsAndMerge(
            ConversionFinderCfg(vtxFlags))

    alg = CompFactory.EMVertexBuilder(name, **kwargs)
    acc.addEventAlgo(alg)
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO_RUN2
    flags.lock()

    acc = MainServicesCfg(flags)
    acc.merge(EMVertexBuilderCfg(flags))
    mlog = logging.getLogger("EMVertexBuilderConfigTest")
    mlog.info("Configuring  EMVertexBuilder: ")
    printProperties(mlog,
                    acc.getEventAlgo("EMVertexBuilder"),
                    nestLevel=1,
                    printDefaults=True)
    with open("vertexbuilder.pkl", "wb") as f:
        acc.store(f)
