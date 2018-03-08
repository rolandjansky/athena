# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_Wj
#  Powheg configuration for Wj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for Wj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_Wj(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_Wj, self).__init__( runArgs, opts )
    self._powheg_executable += '/Wj/pwhg_main'

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'CKM' )
    PowhegDecorators.decorate( self, 'mass window' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'running scales' )
    PowhegDecorators.decorate( self, 'sin**2 theta W' )
    PowhegDecorators.decorate( self, 'single vector boson' )
    PowhegDecorators.decorate( self, 'V+jets' )
    PowhegDecorators.decorate( self, 'vector boson decay' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )
    PowhegDecorators.decorate( self, 'W ID' )
    PowhegDecorators.decorate( self, 'W mass window' )

    ## Set optimised integration parameters
    self.ncall1   = 60000
    self.ncall2   = 80000
    self.nubound  = 40000
    self.xupbound = 3
    self.foldx    = 10
    self.foldy    = 10
    self.foldphi  = 5

    ## Override defaults
    self.bornktmin = 5.0
    self.mass_low  = 2.5
    self.mass_high = 2.0 * self.beam_energy

  ## Extend base-class runcard generation
  def generateRunCard(self) :
    ## Set W mass based on mass window parameters
    self.mass_W_low = self.mass_low
    self.mass_W_high = self.mass_high

    self.initialiseRunCard()
