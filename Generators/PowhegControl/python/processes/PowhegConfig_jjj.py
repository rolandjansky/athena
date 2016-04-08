# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_jjj
#  Powheg configuration for jjj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

## Default Powheg configuration for jjj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_jjj(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_jjj, self).__init__( runArgs, opts )
    self._powheg_executable += '/trijet/pwhg_main'

    ## Logger warnings for unvalidated process
    self.logger.warning( 'Negative weight fraction will probably be outside usual recommendations - check final distributions with care!' )

    ## Add process specific options
    self.add_parameter( 'brpar_diexp', -1 )
    self.add_parameter( 'brpar_dijexp', -1 )
    self.add_parameter( 'HWJsudakov', -1 )
    self.add_parameter( 'ktmerging', -1 )
    self.add_parameter( 'rapsuppfact', -1 )

    ## Decorate with generic option sets
    self.add_parameter_set( 'LHEv3' )
    self.add_parameter_set( 'radiation parametrisation' )
    self.add_parameter_set( 'v2' )

    ## Set optimised integration parameters
    self.ncall1   = 800000
    # self.ncall1rm = 2000000
    self.ncall2   = 1750000
    self.nubound  = 1000000
    self.itmx1    = 2
    # self.itmx1rm  = 2
    self.itmx2    = 10
    self.ixmax    = 5
    self.iymax    = 5

    ## Override defaults
    self.bornktmin      = 20.0
    self.bornsuppfact   = 1000.0
    self.doublefsr      = 1
    self.withdamp       = 1

    self.populate_default_strings()
