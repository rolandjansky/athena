# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_DMS_tloop
#  Powheg configuration for DMS_tloop subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

## Default Powheg configuration for DMS_tloop generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_DMS_tloop(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_DMS_tloop, self).__init__( runArgs, opts, LO_process=True )
    self._powheg_executable += '/DMS_tloop/pwhg_main'

    ## Decorate with generic option sets
    self.add_parameter_set( 'CKKW' )
    self.add_parameter_set( 'dark matter' )
    self.add_parameter_set( 'dark matter phi' )
    self.add_parameter_set( 'mass window' )
    self.add_parameter_set( 'PDF reweighting' )
    self.add_parameter_set( 'running scale', description='(0:2mX fixed; 1:pTj; 2:XXbar invmass; 3:Ht/2)' )
    self.add_parameter_set( 'running width' )
    self.add_parameter_set( 'top properties' )
    self.add_parameter_set( 'v2' )

    ## Set optimised integration parameters
    self.ncall1   = 250000
    self.ncall2   = 1000000
    self.nubound  = 100000
    self.xupbound = 2
    self.foldx    = 1
    self.foldy    = 1
    self.foldphi  = 1

    ## Override defaults
    self.bornktmin    = 100.0
    self.doublefsr    = 1
    self.minlo        = -1
    self.runningscale = 3

    self.populate_default_strings()
