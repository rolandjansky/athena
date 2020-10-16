# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Derive from the offline class and override InDetFlags
"""

__author__ = "J. Masik"
__version__= "$Revision: 1.2 $"
__doc__    = "ConfiguredNewTrackingTrigCuts"

from AthenaCommon.Include import include

_sharedcuts = False
if _sharedcuts:
  from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts as InDetTrigTrackingCuts
else:
  from InDetTrigRecExample.InDetTrigTrackingCuts import InDetTrigTrackingCuts
del _sharedcuts

class ConfiguredNewTrackingTrigCuts(InDetTrigTrackingCuts):
  def __set_indetflags(self):
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    self.__indetflags = InDetTrigFlags



EFIDTrackingCuts = ConfiguredNewTrackingTrigCuts("Offline")
EFIDTrackingCutsCosmics = ConfiguredNewTrackingTrigCuts("Cosmics")
EFIDTrackingCutsBeamGas = ConfiguredNewTrackingTrigCuts("BeamGas")
EFIDTrackingCutsLowPt = ConfiguredNewTrackingTrigCuts("LowPt")
EFIDTrackingCutsTRT = ConfiguredNewTrackingTrigCuts("TRT")
EFIDTrackingCutsHeavyIon = ConfiguredNewTrackingTrigCuts("HeavyIon")
EFIDTrackingCutsLRT = ConfiguredNewTrackingTrigCuts("LRT")
FTKTrackingCuts = ConfiguredNewTrackingTrigCuts("FTK")
L2IDTrackingCuts = EFIDTrackingCuts
