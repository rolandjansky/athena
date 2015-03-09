# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg HZJ subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

###############################################################################
#
#  HZJ
#
###############################################################################
class PowhegConfig_HZJ(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  mass_Z_low  = 10.
  mass_Z_high = 1000.

  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_HZJ, self).__init__( runArgs, opts )
    self._powheg_executable += '/HZJ/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'Higgs v2' )
    PowhegDecorators.decorate( self, 'HJ' )
    PowhegDecorators.decorate( self, 'HV' )

    # Set optimised integration parameters
    self.ncall1   = 80000
    self.ncall2   = 50000
    self.nubound  = 50000
    self.xupbound = 2
    self.foldx    = 5
    self.foldy    = 5
    self.foldphi  = 5

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'max_z_mass '+str(self.mass_Z_high)+'  ! M Z < mass high\n')
      f.write( 'min_z_mass '+str(self.mass_Z_low)+'   ! M Z > mass low\n')
