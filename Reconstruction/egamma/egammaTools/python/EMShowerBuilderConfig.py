# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configuration for EMShowerBuilder"

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaCommon.Logging import logging
from CaloIdentifier import SUBCALO


def EMShowerBuilderCfg(flags, name='EMShowerBuilder', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()
    egammaIso, egammaShowerShape = CompFactory.getComps(
        "egammaIso", "egammaShowerShape",)
    kwargs.setdefault("CellsName", flags.Egamma.Keys.Input.CaloCells)
    kwargs.setdefault(
        "CaloNums", [SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.TILE])
    kwargs.setdefault("ShowerShapeTool", egammaShowerShape())
    kwargs.setdefault("HadronicLeakageTool", egammaIso())

    tool = CompFactory.EMShowerBuilder(name, **kwargs)

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
    mlog = logging.getLogger("EMShowerBuilderConfigTest")
    mlog.info("Configuring  EMShowerBuilder: ")
    printProperties(mlog, cfg.popToolsAndMerge(
        EMShowerBuilderCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)

    f = open("emshowerbuilder.pkl", "wb")
    cfg.store(f)
    f.close()
