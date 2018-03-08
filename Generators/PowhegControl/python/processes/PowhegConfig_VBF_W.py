# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_VBF_W
#  Powheg configuration for VBF W subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

## Default Powheg configuration for VBF W generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_VBF_W(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_VBF_W, self).__init__( runArgs, opts )
    self._powheg_executable += '/VBF_W-Z/pwhg_main'

    ## Decorate with generic option sets
    self.add_parameter_set( 'fake virtual' )
    self.add_parameter_set( 'generic scale' )
    self.add_parameter_set( 'upper bound' )
    self.add_parameter_set( 'VBF vector boson decay' )

    ## Set optimised integration parameters
    self.itmx1    = 4
    self.itmx2    = 5
    self.ncall1   = 800000
    self.ncall2   = 6000000
    self.nubound  = 5000000
    self.xupbound = 6
    self.foldx    = 2
    self.foldy    = 2
    self.foldphi  = 2

    ## Override defaults
    self.allowed_decay_modes = [ 'Wpev', 'Wmev', 'Wmmuv','Wmmuv' ]
    self.decay_mode          = 'Wpev'
    self.withdamp            = 1

    self.populate_default_strings()
