"""Construct ConfigFlags Beam Conditions Monitor Digitization

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createBCMCfgFlags():
    """Return an AthConfigFlags object with required flags"""
    flags = AthConfigFlags()
    # Run Inner Detector noise simulation?
    flags.addFlag("Digitization.DoInnerDetectorNoise", True)
    # Should pile-up digitization be done one bunch crossing at a time?
    # Else all bunch crossings are provided at once.
    flags.addFlag("Digitization.DoXingByXingPileUp", False)
    # Name of MC EvtStore to overlay
    flags.addFlag("Overlay.EventStore", "BkgEvent_0_SG")
    # Name of the random number service to use
    # of ("dSFMT", "Ranlux64", "Ranecu",)
    flags.addFlag("Random.Engine", "dSFMT")
    return flags

