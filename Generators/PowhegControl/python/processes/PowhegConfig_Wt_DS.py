# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_Wt_DS
#  Powheg configuration for Wt_DS subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from .. import ATLASCommonParameters

## Default Powheg configuration for Wt_DS generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_Wt_DS(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_Wt_DS, self).__init__( runArgs, opts )
    self._powheg_executable += '/ST_wtch_DS/pwhg_main'

    ## Add process specific options
    self.add_parameter( 'nwidthcutoff', -1 )
    self.add_parameter( 'withfluxfactor', -1 )
    self.add_parameter( 'withthetacut', -1 )

    ## Decorate with generic option sets
    self.add_parameter_set( 'CKM' )
    self.add_parameter_set( 'lepton mass' )
    self.add_parameter_set( 'sin**2 theta W' )
    self.add_parameter_set( 'single top' )
    self.add_parameter_set( 'top decay branching' )
    self.add_parameter_set( 'top decay mode' )
    self.add_parameter_set( 'top decay second generation quark' )
    self.add_parameter_set( 'top mass' )
    self.add_parameter_set( 'W decay mode' )

    ## Set optimised integration parameters
    self.ncall1   = 40000
    self.ncall2   = 200000
    self.nubound  = 200000
    self.xupbound = 4
    self.foldx    = 5
    self.foldy    = 2
    self.foldphi  = 2

    ## Override defaults
    self.topdecaymode = 11111
    self.withdamp     = 0

    self.populate_default_strings()
