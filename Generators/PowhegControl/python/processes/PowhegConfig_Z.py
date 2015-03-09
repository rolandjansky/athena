# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg Z subprocess
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
#  Z
#
###############################################################################
class PowhegConfig_Z(PowhegConfig_base) :
  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_Z, self).__init__( runArgs, opts )
    self._powheg_executable += '/Z/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'heavy quark' )
    PowhegDecorators.decorate( self, 'running scale' )
    PowhegDecorators.decorate( self, 'single boson' )

    # Set optimised integration parameters
    self.ncall1  = 120000
    self.ncall2  = 250000
    self.nubound = 20000
    self.foldx   = 2
    self.foldy   = 2
    self.foldphi = 2

    # Override defaults
    self.withsubtr       = 1
    self.withnegweights  = 1
    self.mass_low        = 60.0
    self.mass_high       = 2.0 * self.beam_energy

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()
