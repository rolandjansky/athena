# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure e/gamma object quality"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egammaOQFlagsBuilderCfg(flags, name='egammaOQFlagsBuilder', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    from LArCalibUtils.LArHVScaleConfig import LArHVScaleCfg
    acc = LArHVScaleCfg(flags)

    kwargs.setdefault("CellsName", flags.Egamma.Keys.Input.CaloCells)
    kwargs.setdefault("affectedTool", CompFactory.CaloAffectedTool())

    tool = CompFactory.egammaOQFlagsBuilder(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    Configurable.configurableRun3Behavior = True

    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()
    ConfigFlags.dump()

    cfg = ComponentAccumulator()
    mlog = logging.getLogger("egammaOQFlagsBuilderTest")
    mlog.info("Configuring  egammaOQFlagsBuilder: ")
    printProperties(mlog, cfg.popToolsAndMerge(
        egammaOQFlagsBuilderCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)

    f = open("egammaoqflagsbuilder.pkl", "wb")
    cfg.store(f)
    f.close()
