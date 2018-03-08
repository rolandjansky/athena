# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_HZj
#  Powheg configuration for HZj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

## Default Powheg configuration for HZj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_HZj(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_HZj, self).__init__( runArgs, opts )
    self._powheg_executable += '/HZJ/pwhg_main'

    ## Add process specific options
    self.add_parameter( 'bornsuppfactV', -1 )
    self.add_parameter( 'kappa_ghz', 1, desc='multiplicative kappa-factor of the Higgs-Z coupling' )
    self.add_parameter( 'ptVhigh', -1 )
    self.add_parameter( 'ptVlow', -1 )
    self.add_parameter( 'Vstep', -1 )

    ## Decorate with generic option sets
    self.add_parameter_set( 'Higgs mass window' )
    self.add_parameter_set( 'Higgs properties' )
    self.add_parameter_set( 'Higgs+V+jet' )
    self.add_parameter_set( 'LHEv3' )
    self.add_parameter_set( 'MiNLO NNLL' )
    self.add_parameter_set( 'running scales' )
    self.add_parameter_set( 'top mass' )
    self.add_parameter_set( 'vector boson decay' )
    self.add_parameter_set( 'v2' )
    self.add_parameter_set( 'Z mass window' )

    ## Set optimised integration parameters
    self.ncall1   = 40000
    self.ncall2   = 150000
    self.nubound  = 100000
    self.xupbound = 4
    self.foldx    = 5
    self.foldy    = 10
    self.foldphi  = 5

    ## Override defaults
    self.doublefsr   = 1
    self.mass_Z_low  = 60.0
    self.mass_Z_high = 2.0 * self.beam_energy

    self.populate_default_strings()
