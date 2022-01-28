# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the TRT Standalone Thinning
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def ThinGeantTruthCfg(flags, name="ThinGeantTruthAlg", **kwargs):

    mlog = logging.getLogger(name)
    mlog.info("Starting  Geant4 Truth Thinning configuration")
    acc = ComponentAccumulator()
    kwargs.setdefault("StreamName", "StreamAOD")
    kwargs.setdefault("keepEGamma", flags.Reco.EnableEgamma)
    kwargs.setdefault("keepMuons", flags.Reco.EnableCombinedMuon)
    acc.addEventAlgo(CompFactory.ThinGeantTruthAlg(name, **kwargs))
    mlog.info("Geant4 Truth Thinning configured")
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable

    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg

    flags.Input.Files = defaultTestFiles.RDO_RUN2
    flags.Output.doWriteAOD = True  # To test the AOD parts
    flags.lock()
    acc = MainServicesCfg(flags)
    acc.merge(ThinGeantTruthCfg(flags))
    mlog = logging.getLogger("ThinGeantTruthConfigTest")
    printProperties(
        mlog,
        acc.getEventAlgo("ThinGeantTruthAlg"),
        nestLevel=1,
        printDefaults=True,
    )

    with open("thingeanttruchcfg.pkl", "wb") as f:
        acc.store(f)
