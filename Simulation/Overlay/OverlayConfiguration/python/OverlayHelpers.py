#!/usr/bin/env python
"""Overlay configuration helpers

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def OverlayMessageSvcCfg(flags):
    """MessageSvc for overlay"""
    MessageSvc = CompFactory.MessageSvc
    acc = ComponentAccumulator()
    acc.addService(MessageSvc(setError=["HepMcParticleLink"]))
    return acc


def setupOverlayDetectorFlags(configFlags, detectors):
    """Setup Overlay detector flags"""
    if not detectors or 'BCM' in detectors or 'ID' in detectors:
        configFlags.Detector.OverlayBCM = True
    if not detectors or 'DBM' in detectors or 'ID' in detectors:
        configFlags.Detector.OverlayDBM = True
    if not detectors or 'Pixel' in detectors or 'ID' in detectors:
        configFlags.Detector.OverlayPixel = True
    if not detectors or 'SCT' in detectors or 'ID' in detectors:
        configFlags.Detector.OverlaySCT = True
    if not detectors or 'TRT' in detectors or 'ID' in detectors:
        configFlags.Detector.OverlayTRT = True
    if not detectors or 'LAr' in detectors or 'Calo' in detectors or 'L1Calo' in detectors:
        configFlags.Detector.OverlayLAr = True
    if not detectors or 'Tile' in detectors or 'Calo' in detectors or 'L1Calo' in detectors:
        configFlags.Detector.OverlayTile = True
    if not detectors or 'L1Calo' in detectors:
        configFlags.Detector.OverlayL1Calo = not configFlags.Overlay.DataOverlay
    if not detectors or 'CSC' in detectors or 'Muon' in detectors:
        configFlags.Detector.OverlayCSC = True
    if not detectors or 'MDT' in detectors or 'Muon' in detectors:
        configFlags.Detector.OverlayMDT = True
    if not detectors or 'RPC' in detectors or 'Muon' in detectors:
        configFlags.Detector.OverlayRPC = True
    if not detectors or 'TGC' in detectors or 'Muon' in detectors:
        configFlags.Detector.OverlayTGC = True
    if not detectors or 'sTGC' in detectors or 'Muon' in detectors:
        configFlags.Detector.OverlaysTGC = True
    if not detectors or 'MM' in detectors or 'Muon' in detectors:
        configFlags.Detector.OverlayMM = True


def accFromFragment(acc_string, flags):
    """Return initialized accumulator from post include fragment."""
    parts = acc_string.split('.')
    if len(parts) < 3:
        raise ValueError('Post include should be of the form Package.Module.Function')

    function = parts[-1]
    module = '.'.join(parts[:-1])

    from importlib import import_module
    loaded_module = import_module(module)
    function_def = getattr(loaded_module, function)

    return function_def(flags)
