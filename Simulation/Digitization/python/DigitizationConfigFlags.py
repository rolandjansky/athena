"""Construct ConfigFlags for Digitization

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createDigitizationCfgFlags():
    """Return an AthConfigFlags object with required flags"""
    flags = AthConfigFlags()
    # Run Inner Detector noise simulation
    flags.addFlag("Digitization.DoInnerDetectorNoise", True)
    # Run pile-up digitization on one bunch crossing at a time?
    flags.addFlag("Digitization.DoXingByXingPileUp", False)
    # Run pile-up premixing
    flags.addFlag("Digitization.PileUpPremixing", False)
    # Run pile-up premixing for Multi-Threading
    flags.addFlag("Digitization.PileUpPremixingForMT", False)
    return flags

