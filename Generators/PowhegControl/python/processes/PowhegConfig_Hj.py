# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_Hj
#  Powheg configuration for Hj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

## Default Powheg configuration for Hj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_Hj(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_Hj, self).__init__( runArgs, opts )
    self._powheg_executable += '/HJ/pwhg_main'

    ## Add process specific options
    self.add_parameter( 'bmass_in_minlo', 0,   desc='(default 0) 1: use non-zero b-mass in MiNLO' )
    self.add_parameter( 'quarkmasseffects', 1, desc='(default 0) 1: enable quark mass effects' )

    ## Decorate with generic option sets
    self.add_parameter_set( 'CKKW' )
    self.add_parameter_set( 'extra tests' )
    self.add_parameter_set( 'H+jets' )
    self.add_parameter_set( 'Higgs decay properties' )
    self.add_parameter_set( 'Higgs fixed width' )
    self.add_parameter_set( 'Higgs properties' )
    self.add_parameter_set( 'LHEv3' )
    self.add_parameter_set( 'MiNLO NNLL' )
    self.add_parameter_set( 'NNLO reweighting' )
    self.add_parameter_set( 'running scales' )
    self.add_parameter_set( 'top mass' )
    self.add_parameter_set( 'v2' )

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

    self.populate_default_strings()
