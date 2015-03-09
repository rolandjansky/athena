# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg HWJ subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

###############################################################################
#
#  HWJ
#
###############################################################################
class PowhegConfig_HWJ(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  idvecbos    = -24
  mass_W_low  = 10.
  mass_W_high = 1000.

  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_HWJ, self).__init__( runArgs, opts )
    self._powheg_executable += '/HWJ/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'Higgs v2' )
    PowhegDecorators.decorate( self, 'HJ' )
    PowhegDecorators.decorate( self, 'HV' )

    # Set optimised integration parameters
    self.ncall1   = 30000
    self.ncall2   = 40000
    self.nubound  = 50000
    self.xupbound = 2
    self.foldx    = 5
    self.foldy    = 5
    self.foldphi  = 5

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'idvecbos '+str(self.idvecbos)+'       ! PDG code for vector boson to be produced (W:24)\n' )
      f.write( 'max_w_mass '+str(self.mass_W_high)+'  ! M W < mass high\n' )
      f.write( 'min_w_mass '+str(self.mass_W_low)+'   ! M W > mass low\n' )
