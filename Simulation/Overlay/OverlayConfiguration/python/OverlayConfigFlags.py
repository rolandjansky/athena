"""Construct Overlay configuration flags

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.AthConfigFlags import AthConfigFlags


def createOverlayConfigFlags():
    """Return an AthConfigFlags object with required flags"""
    flags = AthConfigFlags()
    # Data overlay flag
    flags.addFlag("Overlay.DataOverlay", False)
    # Overlay background StoreGate key prefix
    flags.addFlag("Overlay.BkgPrefix", "Bkg_")
    # Overlay signal StoreGate key prefix
    flags.addFlag("Overlay.SigPrefix", "Sig_")
    # Overlay extra input dependencies
    flags.addFlag("Overlay.ExtraInputs", [("McEventCollection", "TruthEvent")])
    return flags


def setupOverlayDetectorFlags(configFlags):
    """Setup Overlay detector flags"""
    # Set all to true if this function is called
    configFlags.Detector.OverlayBCM = True
    configFlags.Detector.OverlayDBM = True
    configFlags.Detector.OverlayPixel = True
    configFlags.Detector.OverlaySCT = True
    configFlags.Detector.OverlayTRT = True
    configFlags.Detector.OverlayLAr = True
    configFlags.Detector.OverlayTile = True
    configFlags.Detector.OverlayL1Calo = not configFlags.Overlay.DataOverlay
    configFlags.Detector.OverlayCSC = True
    configFlags.Detector.OverlayMDT = True
    configFlags.Detector.OverlayRPC = True
    configFlags.Detector.OverlayTGC = True
    configFlags.Detector.OverlaysTGC = True
    configFlags.Detector.OverlayMM = True
