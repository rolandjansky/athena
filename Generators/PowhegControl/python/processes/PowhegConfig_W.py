# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_W
#  Powheg configuration for W subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for W generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_W(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_W, self).__init__( runArgs, opts )
    self._powheg_executable += '/W/pwhg_main'

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'CKM' )
    PowhegDecorators.decorate( self, 'mass window' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'running scale' )
    PowhegDecorators.decorate( self, 'second generation quark mass' )
    PowhegDecorators.decorate( self, 'sin**2 theta W' )
    PowhegDecorators.decorate( self, 'single vector boson' )
    PowhegDecorators.decorate( self, 'vector boson decay' )
    PowhegDecorators.decorate( self, 'W ID' )

    ## Set optimised integration parameters
    self.ncall1  = 120000
    self.ncall2  = 250000
    self.nubound = 20000

    ## Override defaults
    self.withdamp        = 1
    self.withsubtr       = 1
    self.withnegweights  = 1
    self.mass_low        = 2.5
    self.mass_high       = 2.0 * self.beam_energy

  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()
