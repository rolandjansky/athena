"""Construct ConfigFlags for Overlay

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createOverlayCfgFlags():
    """Return an AthConfigFlags object with required flags"""
    flags = AthConfigFlags()
    # Overlay background StoreGate key prefix
    flags.addFlag("Overlay.BkgPrefix", "Bkg_")
    # Overlay signal StoreGate key prefix
    flags.addFlag("Overlay.SigPrefix", "Sig_")
    # Are we multithreaded?
    flags.addFlag("Overlay.Legacy.MT", False)
    # Name of MC EvtStore to overlay
    flags.addFlag("Overlay.Legacy.EventStore", "BkgEvent_0_SG")
    return flags

