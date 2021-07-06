#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SGInputLoaderCfg(flags, **kwargs):
    sgil = CompFactory.SGInputLoader(**kwargs)

    # Custom merge semantics to allow appending items to the Load list
    def merge_sgil_types(a, b):
        for item in b:
            if item not in a:
                a.append(item)
        return a
    sgil._descriptors['Load'].semantics.merge = merge_sgil_types

    acc = ComponentAccumulator()
    acc.addEventAlgo(sgil, primary=True)
    return acc
