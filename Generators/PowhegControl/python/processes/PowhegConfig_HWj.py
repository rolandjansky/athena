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
    self.add_parameter( 'bornsuppfactW', -1, desc='(-1:Powheg default)' )
    self.add_parameter( 'kappa_ghw',      1, desc='multiplicative kappa-factor of the Higgs-W coupling' )

    ## Decorate with generic option sets
    self.add_parameter_set( 'generic scale' )
    self.add_parameter_set( 'Higgs + vector boson' )
    self.add_parameter_set( 'Higgs + vector boson + jet' )
    self.add_parameter_set( 'Higgs properties' )
    self.add_parameter_set( 'MiNLO NNLL' )
    self.add_parameter_set( 'old Dij' )
    self.add_parameter_set( 'PDF reweighting' )
    self.add_parameter_set( 'running scales' )
    self.add_parameter_set( 'top properties' )
    self.add_parameter_set( 'upper bound' )
    self.add_parameter_set( 'vector boson decay mode' )
    self.add_parameter_set( 'v2' )
    self.add_parameter_set( 'W ID' )
    self.add_parameter_set( 'W mass window' )

    ## Set optimised integration parameters
    self.itmx1    = 2
    self.ncall1   = 70000
    self.ncall2   = 70000
    self.nubound  = 150000
    self.xupbound = 20
    self.foldx    = 10
    self.foldy    = 5
    self.foldphi  = 5

    ## Override defaults
    self.doublefsr   = 1
    self.mass_W_low  = 2.5
    self.mass_W_high = 2.0 * self.beam_energy

    self.populate_default_strings()
