"""Construct Overlay configuration flags

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.AthConfigFlags import AthConfigFlags


def createOverlayConfigFlags():
    """Return an AthConfigFlags object with required flags"""
    flags = AthConfigFlags()
    # Data overlay flag
    flags.addFlag("Overlay.DataOverlay", False)
    # Overlay skip secondary events
    flags.addFlag("Overlay.SkipSecondaryEvents", -1)
    # Overlay background StoreGate key prefix
    flags.addFlag("Overlay.BkgPrefix", "Bkg_")
    # Overlay signal StoreGate key prefix
    flags.addFlag("Overlay.SigPrefix", "Sig_")
    # Overlay extra input dependencies
    flags.addFlag("Overlay.ExtraInputs", [("McEventCollection", "TruthEvent")])
    return flags
