# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg W subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#            Daniel Hayden   <danhayden0@googlemail.com>
#            Stephen Bieniek <stephen.paul.bieniek@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

###############################################################################
#
#  W
#
###############################################################################
class PowhegConfig_W(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  idvecbos = 24

  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_W, self).__init__( runArgs, opts )
    self._powheg_executable += '/W/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'CKM' )
    PowhegDecorators.decorate( self, 'heavy quark' )
    PowhegDecorators.decorate( self, 'running scale' )
    PowhegDecorators.decorate( self, 'single boson' )

    # Set optimised integration parameters
    self.ncall1  = 120000
    self.ncall2  = 250000
    self.nubound = 20000

    # Override defaults
    self.withdamp        = 1
    self.withsubtr       = 1
    self.withnegweights  = 1
    self.mass_low        = 2.5
    self.mass_high       = 2.0 * self.beam_energy

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'idvecbos '+str(self.idvecbos)+'               ! PDG code for vector boson to be produced (W:24)\n' )
