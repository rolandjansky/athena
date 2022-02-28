# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configuration for EMClusterTool"

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaCommon.Logging import logging

from egammaMVACalib.egammaMVACalibConfig import egammaMVASvcCfg


def EMClusterToolCfg(flags, name='EMClusterTool', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    if "MVACalibSvc" not in kwargs:
        mvacal = egammaMVASvcCfg(flags)
        kwargs["MVACalibSvc"] = acc.getPrimaryAndMerge(mvacal)
    kwargs.setdefault("OutputClusterContainerName",
                      flags.Egamma.Keys.Output.CaloClusters)

    tool = CompFactory.EMClusterTool(name, **kwargs)

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
    mlog = logging.getLogger("EMClusterToolConfigTest")
    mlog.info("Configuring  EMClusterTool: ")
    printProperties(mlog, cfg.popToolsAndMerge(
        EMClusterToolCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)

    f = open("emclustertool.pkl", "wb")
    cfg.store(f)
    f.close()
