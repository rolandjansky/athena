# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_Z
#  Powheg configuration for Z subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for Z generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_Z(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_Z, self).__init__( runArgs, opts )
    self._powheg_executable += '/Z/pwhg_main'

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'mass window' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'running scale' )
    PowhegDecorators.decorate( self, 'second generation quark mass' )
    PowhegDecorators.decorate( self, 'sin**2 theta W' )
    PowhegDecorators.decorate( self, 'single vector boson' )
    PowhegDecorators.decorate( self, 'vector boson decay' )

    ## Set optimised integration parameters
    self.ncall1  = 120000
    self.ncall2  = 250000
    self.nubound = 20000
    self.foldx   = 2
    self.foldy   = 2
    self.foldphi = 2

    ## Override defaults
    self.withsubtr      = 1
    self.withnegweights = 1
    self.mass_low       = 60.0
    self.mass_high      = 2.0 * self.beam_energy

  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()
