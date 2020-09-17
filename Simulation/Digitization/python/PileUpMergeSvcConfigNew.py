"""ComponentAccumulator configuration for PileUpMergeSvc

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def PileUpMergeSvcCfg(flags, name="PileUpMergeSvc", **kwargs):
    """Return ComponentAccumulator with PileUpMergeSvc

    If doing XingByXingPileUp, the "Intervals" keyword argument should
    be set with PileUpXingFolder tools.
    Otherwise, "Intervals" is should be empty, and we enforce that here.
    """
    acc = ComponentAccumulator()

    if flags.Digitization.DoXingByXingPileUp:
        # handle input type variety
        Intervals = kwargs.setdefault("Intervals", [])
        if not isinstance(Intervals, list):
            kwargs["Intervals"] = [Intervals]
    else:
        kwargs["Intervals"] = []

    acc.addService(CompFactory.PileUpMergeSvc(name, **kwargs))
    return acc


def PileUpXingFolderCfg(flags, name="PileUpXingFolder" , **kwargs):
    """Return a configured PileUpXingFolder tool"""
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.PileUpXingFolder(name, **kwargs))
    return acc
