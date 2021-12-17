# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure Conversion building"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaTrackTools.egammaTrackToolsConfig import EMExtrapolationToolsCfg


def EMConversionBuilderCfg(flags, name='EMConversionBuilder', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()
    EMConversionBuilder = CompFactory.EMConversionBuilder

    if "ExtrapolationTool" not in kwargs:
        extraptool = EMExtrapolationToolsCfg(flags)
        kwargs["ExtrapolationTool"] = extraptool.popToolsAndMerge(extraptool)
    kwargs.setdefault("ConversionContainerName",
                      flags.Egamma.Keys.Output.ConversionVertices)

    emconv = EMConversionBuilder(name, **kwargs)

    acc.setPrivateTools(emconv)
    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    Configurable.configurableRun3Behavior = True

    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()
    ConfigFlags.dump()

    cfg = ComponentAccumulator()
    mlog = logging.getLogger("EMConversionBuilderConfigTest")
    mlog.info("Configuring  EMConversionBuilder: ")
    printProperties(mlog, cfg.popToolsAndMerge(
        EMConversionBuilderCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)

    f = open("emshowerbuilder.pkl", "wb")
    cfg.store(f)
    f.close()
