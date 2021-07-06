# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure egammaLargeFWDClusterMaker, which chooses cells to store in the AOD" 
__author__ = "Jovan Mitrevski"

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
egammaLargeFWDClusterMaker=CompFactory.egammaLargeFWDClusterMaker

def egammaLargeFWDClusterMakerCfg(flags, **kwargs):

    acc = ComponentAccumulator
    
    kwargs.setdefault("CellsName", flags.Egamma.Keys.Input.CaloCells)
    kwargs.setdefault("InputClusterCollection", flags.Egamma.Keys.Output.FwdCluster)
    kwargs.setdefault("doFWDelesurraundingWindows", True)

    acc.setPrivateTools(egammaLargeFWDClusterMaker(**kwargs))

    return acc

