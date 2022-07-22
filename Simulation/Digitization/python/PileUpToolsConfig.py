"""Configure the algorithm to carry PileUpTools for Digitization

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep

PileUpToolsAlg, PileUpMTAlg, DigitizationAlg = CompFactory.getComps(
    "PileUpToolsAlg", "PileUpMTAlg", "DigitizationAlg"
)


def PileUpToolsCfg(flags, **kwargs):
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
        if flags.Concurrency.NumThreads > 0:
            Alg = PileUpMTAlg
        else:
            Alg = PileUpToolsAlg
    else:
        Alg = DigitizationAlg

    # setup EventInfo
    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs["EventInfoKey"] = flags.Overlay.BkgPrefix + "EventInfo"

    acc.addEventAlgo(Alg(flags.Digitization.DigiSteeringConf, **kwargs))
    return acc
