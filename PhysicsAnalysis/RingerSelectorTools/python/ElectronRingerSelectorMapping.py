# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        ElectronRingerSelectorMapping.py
##
## Author:      Werner Spolidoro Freund (LPS/UFRJ)
## Created:     Mar 2015
##
## Description: Retrieve selector from mask mapping 
##=============================================================================

import cppyy

try :
  cppyy.loadDictionary('RingerSelectorToolsDict')
except RuntimeError, e:
  from AthenaCommon.Logging import logging
  mlog = logging.getLogger( 'ElectronRingerSelectorMapping.py' )
  mlog.error("Could not load RingerSelectorTools dictionary from cppyy.")
  raise RuntimeError(e)

from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import electronPIDmenu

# Import from Ringer selector tools dictionary:
from ROOT import Ringer

from RingerSelectorTools.ElectronRingerSelectorDefs import *

class electronRingerPIDmenu (electronPIDmenu):
  testMenu = 999

#####################################################################################################
##                                    Requirements defined are:                                    ##
##                                 (see RingerSelectorToolsDefs.h)                                 ##
#####################################################################################################
##  Loose_CutID_Pd                || Same detection probability as CutID Loose                     ##
##  Medium_CutID_Pd               || Same detection probability as CutID Medium                    ##
##  Tight_CutID_Pd                || Same detection probability as CutID Tight                     ##
##  Loose_CutID_Pf                || Same false alarm probability as CutID Loose                   ##
##  Medium_CutID_Pf               || Same false alarm probability as CutID Medium                  ##
##  Tight_CutID_Pf                || Same false alarm probability as CutID Tight                   ##
##  Loose_LH_Pd,                  || Same detection probability as LH Loose                        ##
##  Medium_LH_Pd,                 || Same detection probability as LH Medium                       ##
##  Tight_LH_Pd,                  || Same detection probability as LH Tight                        ##
##  Loose_LH_Pf,                  || Same false alarm probability as LH Loose                      ##
##  Medium_LH_Pf,                 || Same false alarm probability as LH Medium                     ##
##  Tight_LH_Pf,                  || Same false alarm probability as LH Tight                      ##
##  Medium-MaxSP                  || Maximum SP-product                                            ##
##  Loose                         || Same as LooseCutIDPd                                          ##
##  Medium                        || Same as Medium-MaxSP                                          ##
##  Tight                         || Same as TightCutIDPfa                                         ##
##  NoCut                         || Run to retrieve discriminators outputs, but no cut is applied ##
#####################################################################################################
#####################################################################################################

ElectronRingerMap = {
  (Ringer.Loose, electronRingerPIDmenu.testMenu): ElectronRingerSelectorTestLoose,
  (Ringer.Medium, electronRingerPIDmenu.testMenu): ElectronRingerSelectorTestMedium,
  (Ringer.Tight, electronRingerPIDmenu.testMenu):  ElectronRingerSelectorTestTight,
  (Ringer.NoCut, electronRingerPIDmenu.testMenu):  ElectronRingerSelectorTestNoCut,
  }
