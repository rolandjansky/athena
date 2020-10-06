# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def JobOptsDumperCfg(flags, FileName="JobOptsConfig.txt"):
    """Return ComponentAccumulator with JobOptsDumperAlg.

    This dumps the configuration to text in a file named by FileName.
    """
    acc = ComponentAccumulator()
    acc.addEventAlgo(CompFactory.JobOptsDumperAlg(FileName=FileName))
    return acc
