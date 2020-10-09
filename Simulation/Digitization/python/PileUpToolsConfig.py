"""Configure the algorithm to carry PileUpTools for Digitization

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
PileUpToolsAlg, DigitizationAlg=CompFactory.getComps("PileUpToolsAlg","DigitizationAlg")


def PileUpToolsCfg(flags, name="StandardPileUpToolsAlg", **kwargs):
    """
    Return ComponentAccumulator with the correct algorithm to carry Digitization PileUpTools

    Keyword argument "PileUpTools" may be an AlgTool or list of AlgTools.
    """
    acc = ComponentAccumulator()

    # handle input type variety
    PileUpTools = kwargs.setdefault("PileUpTools", [])
    if not isinstance(PileUpTools, list):
        kwargs["PileUpTools"] = [PileUpTools]

    # declare common extra inputs
    kwargs["ExtraInputs"] = flags.Digitization.ExtraInputs

    # choose the correct alg
    if flags.Digitization.DoXingByXingPileUp:
        Alg = PileUpToolsAlg
    else:
        Alg = DigitizationAlg

    acc.addEventAlgo(Alg(name, **kwargs))
    return acc
