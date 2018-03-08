# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_HZJ
#  Powheg configuration for HZJ subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for HZJ generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_HZJ(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_HZJ, self).__init__( runArgs, opts )
    self._powheg_executable += '/HZJ/pwhg_main'

    ## Add process specific options
    self.bornsuppfactV = -1
    self.kappa_ghz     = 1.0
    self.ptVhigh       = -1
    self.ptVlow        = -1
    self.Vstep         = -1

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'Higgs mass window' )
    PowhegDecorators.decorate( self, 'Higgs properties' )
    PowhegDecorators.decorate( self, 'Higgs+V+jet' )
    PowhegDecorators.decorate( self, 'MiNLO NNLL' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'running scales' )
    PowhegDecorators.decorate( self, 'top mass' )
    PowhegDecorators.decorate( self, 'vector boson decay' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )
    PowhegDecorators.decorate( self, 'Z mass window' )

    ## Set optimised integration parameters
    self.ncall1   = 80000
    self.ncall2   = 50000
    self.nubound  = 50000
    self.xupbound = 2
    self.foldx    = 5
    self.foldy    = 5
    self.foldphi  = 5

  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( self.runcard_path(), 'a' ) as f :
      f.write( 'bornsuppfactV '+str(self.bornsuppfactV)+' ! \n' )
      f.write( 'kappa_ghz '+str(self.kappa_ghz)+'         ! multiplicative kappa-factor of the Higgs-Z coupling\n' )
      f.write( 'ptVhigh '+str(self.ptVhigh)+'             ! \n' )
      f.write( 'ptVlow '+str(self.ptVlow)+'               ! \n' )
      f.write( 'Vstep '+str(self.Vstep)+'                 ! \n' )
