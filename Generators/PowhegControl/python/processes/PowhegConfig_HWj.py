# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_HWj
#  Powheg configuration for HWj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

## Default Powheg configuration for HWj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_HWj(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_HWj, self).__init__( runArgs, opts )
    self._powheg_executable += '/HWJ/pwhg_main'

    ## Add process specific options
    self.add_parameter( 'bornsuppfactW', -1 )
    self.add_parameter( 'kappa_ghw', 1, desc='multiplicative kappa-factor of the Higgs-W coupling' )

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
    self.add_parameter_set( 'W ID' )
    self.add_parameter_set( 'W mass window' )

    ## Set optimised integration parameters
    self.ncall1   = 150000
    self.ncall2   = 100000
    self.nubound  = 100000
    self.foldx    = 5
    self.foldy    = 5
    self.foldphi  = 10

    ## Override defaults
    self.doublefsr   = 1
    self.mass_W_low  = 2.5
    self.mass_W_high = 2.0 * self.beam_energy
