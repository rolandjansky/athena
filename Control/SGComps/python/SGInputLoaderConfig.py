#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SGInputLoaderCfg(flags, **kwargs):
    sgil = CompFactory.SGInputLoader(**kwargs)
    acc = ComponentAccumulator()
    acc.addEventAlgo(sgil, primary=True)
    return acc
