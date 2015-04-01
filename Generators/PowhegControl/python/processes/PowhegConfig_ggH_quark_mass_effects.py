# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_ggH_quark_mass_effects
#  Powheg configuration for ggH_quark_mass_effects subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators
from .. import ATLASCommonParameters

## Default Powheg configuration for ggH_quark_mass_effects generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_ggH_quark_mass_effects(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_ggH_quark_mass_effects, self).__init__( runArgs, opts )
    self._powheg_executable += '/gg_H_quark-mass-effects/pwhg_main'

    ## Add process specific options
    self.bwshape = 1
    self.ew      = 1
    self.massren = 0
    self.model   = 0

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'Higgs decay mode' )
    PowhegDecorators.decorate( self, 'Higgs decay properties' )
    PowhegDecorators.decorate( self, 'Higgs properties' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'running scale' )
    PowhegDecorators.decorate( self, 'top mass' )
    PowhegDecorators.decorate( self, 'zero width' )

    ## Set optimised integration parameters
    self.ncall1  = 50000
    self.ncall2  = 100000
    self.nubound = 50000

  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( self.runcard_path(), 'a' ) as f :
      f.write( 'bwshape '+str(self.bwshape)+'                 ! Controls the functional form of the Breit-Wigner along which Higgs boson virtuality is distributed.\n' )
      f.write( '                                              !   1 (default) : running width that depends on Higgs boson virtuality\n' )
      f.write( '                                              !   2 : fix the BW width to the value in hwidth.\n' )
      f.write( 'ew '+str(self.ew)+'                           ! If 1 enable EW corrections\n' )
      f.write( 'gfermi '+str(ATLASCommonParameters.G_F)+'     ! Fermi constant\n' )
      f.write( 'massren '+str(self.massren)+'                 ! 0 = OS, 1 = MSBAR, 2 = DRBAR\n' )
      f.write( 'model '+str(self.model)+'                     ! 0 = SM\n' )
