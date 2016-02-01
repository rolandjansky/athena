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
    self.logger.warning( 'Integration parameters have not been validated - see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PowhegForATLAS#Changing_POWHEG_BOX_integration' )

    ## Add process specific options
    self.add_parameter( 'brpar_diexp', -1,  desc='(-1:Powheg default)' )
    self.add_parameter( 'brpar_dijexp', -1, desc='(-1:Powheg default)' )
    self.add_parameter( 'HWJsudakov', -1,   desc='(-1:Powheg default)' )
    self.add_parameter( 'ktmerging', -1,    desc='(-1:Powheg default)' )
    self.add_parameter( 'rapsuppfact', -1,  desc='(-1:Powheg default)' )

    ## Decorate with generic option sets
    self.add_parameter_set( 'generic scale' )
    self.add_parameter_set( 'old Dij' )
    self.add_parameter_set( 'PDF reweighting' )
    self.add_parameter_set( 'radiation parametrisation' )
    self.add_parameter_set( 'upper bound' )
    self.add_parameter_set( 'v2' )

    ## Set optimised integration parameters
    self.itmx1    = 20
    self.itmx2    = 20
    self.ncall1   = 10000000
    self.ncall1rm = 30000000
    self.ncall2   = 10000000
    self.nubound  = 10000000
    self.xupbound = 4
    self.foldx    = 25
    self.foldy    = 25
    self.foldphi  = 25

    ## Override defaults
    self.bornktmin      = 1.0
    self.bornsuppfact   = 800.0
    self.doublefsr      = 1
    self.withdamp       = 1

    self.populate_default_strings()
