# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_HJ
#  Powheg configuration for HJ subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for HJ generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_HJ(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_HJ, self).__init__( runArgs, opts )
    self._powheg_executable += '/HJ/pwhg_main'

    ## Add process specific options
    self.bmass_in_minlo   = 0
    self.quarkmasseffects = 1

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'CKKW' )
    PowhegDecorators.decorate( self, 'extra tests' )
    PowhegDecorators.decorate( self, 'H+jets' )
    PowhegDecorators.decorate( self, 'Higgs decay properties' )
    PowhegDecorators.decorate( self, 'Higgs fixed width' )
    PowhegDecorators.decorate( self, 'Higgs properties' )
    PowhegDecorators.decorate( self, 'MiNLO NNLL' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'running scales' )
    PowhegDecorators.decorate( self, 'top mass' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )

    ## Set optimised integration parameters
    self.ncall1   = 150000
    self.ncall1rm = 150000
    self.ncall2   = 150000
    self.nubound  = 100000
    self.xupbound = 2
    self.foldx    = 5
    self.foldy    = 5
    self.foldphi  = 2

    ## Override defaults
    self.bornktmin    = 0.26
    self.bornzerodamp = 1
    self.par_diexp    = 2
    self.par_dijexp   = 2


  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( self.runcard_path(), 'a' ) as f :
      f.write( 'quarkmasseffects '+str(self.quarkmasseffects)+' ! (default 0) 1: enable quark mass effects\n' )
      f.write( 'bmass_in_minlo '+str(self.bmass_in_minlo)+'     ! (default 0) 1: use non-zero b-mass in MiNLO\n' )
