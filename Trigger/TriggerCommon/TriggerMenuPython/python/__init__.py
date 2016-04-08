# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: TriggerJobOpts/__init__.py

__version__ = '$Revision 1.1 $'
__author__  = 'Tomasz.Bold@cern.ch'
__all__ = [ 'GenerateMenu',
            # flags common to each slice
            'CommonSliceFlags',
            'CommonSliceHelper',
            # slice flags
            'EgammaSliceFlags',
            'TauSliceFlags',
            'BphysicsSliceFlags',
            'BjetSliceFlags',
            'JetSliceFlags',
            'METSliceFlags',
            'MuonSliceFlags',
            'CalibSliceFlags',
            'L1CaloSliceFlags',
            # generators
            'Electron', 'MultiElectron', 'Photon',
            'Lumi' ]
