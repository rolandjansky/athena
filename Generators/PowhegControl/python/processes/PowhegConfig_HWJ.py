# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_HWJ
#  Powheg configuration for HWJ subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for HWJ generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_HWJ(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_HWJ, self).__init__( runArgs, opts )
    self._powheg_executable += '/HWJ/pwhg_main'

    ## Add process specific options
    self.bornsuppfactW = -1
    self.kappa_ghw     = 1.0

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'Higgs mass window' )
    PowhegDecorators.decorate( self, 'Higgs properties' )
    PowhegDecorators.decorate( self, 'Higgs+V+jet' )
    PowhegDecorators.decorate( self, 'MiNLO NNLL' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'running scales' )
    PowhegDecorators.decorate( self, 'W ID' )
    PowhegDecorators.decorate( self, 'W mass window' )
    PowhegDecorators.decorate( self, 'top mass' )
    PowhegDecorators.decorate( self, 'vector boson decay' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )

    ## Set optimised integration parameters
    self.ncall1   = 30000
    self.ncall2   = 40000
    self.nubound  = 50000
    self.xupbound = 2
    self.foldx    = 5
    self.foldy    = 5
    self.foldphi  = 5

  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( self.runcard_path(), 'a' ) as f :
      f.write( 'bornsuppfactW '+str(self.bornsuppfactW)+' ! \n' )
      f.write( 'kappa_ghw '+str(self.kappa_ghw)+'         ! multiplicative kappa-factor of the Higgs-W coupling\n' )
