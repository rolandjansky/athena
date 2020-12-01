# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

""" Derive from the offline class and override InDetFlags
"""

__author__ = "J. Masik"
__version__= "$Revision: 1.2 $"
__doc__    = "ConfiguredNewTrackingTrigCuts"


_sharedcuts = False
if _sharedcuts:
  from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts as InDetTrigTrackingCuts
else:
  from InDetTrigRecExample.InDetTrigTrackingCuts import InDetTrigTrackingCuts
del _sharedcuts

class ConfiguredNewTrackingTrigCuts(InDetTrigTrackingCuts):
  pass
  
if not 'EFIDTrackingCuts' in globals():
  EFIDTrackingCuts = ConfiguredNewTrackingTrigCuts("Offline")
  EFIDTrackingCutsCosmics = ConfiguredNewTrackingTrigCuts("Cosmics")
  EFIDTrackingCutsBeamGas = ConfiguredNewTrackingTrigCuts("BeamGas")
  EFIDTrackingCutsLowPt = ConfiguredNewTrackingTrigCuts("LowPt")
  EFIDTrackingCutsTRT = ConfiguredNewTrackingTrigCuts("TRT")
  EFIDTrackingCutsHeavyIon = ConfiguredNewTrackingTrigCuts("HeavyIon")
  L2IDTrackingCuts = EFIDTrackingCuts
