# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_DMV
#  Powheg configuration for DMV subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for DMV generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_DMV(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_DMV, self).__init__( runArgs, opts )
    self._powheg_executable += '/DMV/pwhg_main'

    ## Add process specific options
    self.V_mass  = 100
    self.V_width = 42.494

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'dark matter' )
    PowhegDecorators.decorate( self, 'mass window' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'running scale' )
    PowhegDecorators.decorate( self, 'running width' )
    PowhegDecorators.decorate( self, 'vector boson decay' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )

    ## Set optimised integration parameters
    self.ncall1   = 250000
    self.ncall2   = 1000000
    self.nubound  = 100000
    self.xupbound = 2
    self.foldx    = 2
    self.foldy    = 2
    self.foldphi  = 2

    ## Override defaults
    self.bornktmin    = 100.0
    self.doublefsr    = 1
    self.vdecaymode   = -1
    self.runningscale = 3 # ! (default 3) 0, 1, 2, 3 correspond to 2mX (fixed), ptj, XXbar invmass, Ht/2
    self.mass_low     = 0.0
    self.mass_high    = 2.0 * self.beam_energy
    self.minlo        = -1


  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( self.runcard_path(), 'a' ) as f :
      f.write( 'DMVmass '+str(self.V_mass)+'   ! mediator mass\n' )
      f.write( 'DMVwidth '+str(self.V_width)+' ! mediator width\n' )
