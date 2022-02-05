"""Configuration for the LAr SC Simple Maker algorithm

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def LArSCSimpleMakerCfg(flags, **kwargs):
    """Return ComponentAccumulator for LArSCSimpleMaker algorithm"""
    acc = ComponentAccumulator()

    acc.addCondAlgo(CompFactory.CaloNoiseSigmaDiffCondAlg(**kwargs))

    from CaloRec.CaloBCIDLumiCondAlgConfig import CaloBCIDLumiCondAlgCfg
    acc.merge(CaloBCIDLumiCondAlgCfg(flags))

    from CaloRec.CaloBCIDAvgAlgConfig import CaloBCIDAvgAlgCfg
    acc.merge(CaloBCIDAvgAlgCfg(flags))

    kwargs.setdefault("SCellContainer","SimpleSCellNoBCID")
    acc.addEventAlgo(CompFactory.LArSCSimpleMaker(**kwargs))
    return acc

def LArSuperCellBCIDEmAlgCfg(flags, **kwargs):
    """Return ComponentAccumulator for LArSuperCellBCIDEmAlg algorithm"""
    acc = ComponentAccumulator()

    from CaloRec.CaloBCIDLumiCondAlgConfig import CaloBCIDLumiCondAlgCfg
    acc.merge(CaloBCIDLumiCondAlgCfg(flags))

    from CaloRec.CaloBCIDAvgAlgConfig import CaloBCIDAvgAlgCfg
    acc.merge(CaloBCIDAvgAlgCfg(flags))

    kwargs.setdefault("SCellContainerIn","SimpleSCellNoBCID")
    kwargs.setdefault("SCellContainerOut","SCellEm")
    acc.addEventAlgo(CompFactory.LArSuperCellBCIDEmAlg(**kwargs))
    return acc
