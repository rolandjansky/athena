# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg ggHZ subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#            Daniel Hayden   <danhayden0@googlemail.com>
#            Stephen Bieniek <stephen.paul.bieniek@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from PowhegConfig_base import PowhegConfig_base
import PowhegDecorators
import SMParams

###############################################################################
#
#  ggHZ
#
###############################################################################
class PowhegConfig_ggHZ(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  mass_Z_low  = 10.
  mass_Z_high = 1000.

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    super(PowhegConfig_ggHZ, self).__init__(runArgs)
    self._powheg_executable += '/ggHZ/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'Higgs v2' )
    PowhegDecorators.decorate( self, 'vector boson decay' )

    # Set optimised integration parameters
    self.ncall1   = 25000 #50000
    self.ncall2   = 50000
    self.nubound  = 40000
    self.xupbound = 3
    self.itmx1    = 1
    self.itmx2    = 1

    # Override defaults
    self.minlo    = -1

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'max_z_mass '+str(self.mass_Z_high)+'  ! M Z < mass high\n')
      f.write( 'min_z_mass '+str(self.mass_Z_low)+'   ! M Z > mass low\n')
