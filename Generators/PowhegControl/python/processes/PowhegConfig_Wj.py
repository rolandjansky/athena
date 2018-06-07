# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_Wj
#  Powheg configuration for Wj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

## Default Powheg configuration for Wj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_Wj(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_Wj, self).__init__( runArgs, opts )
    self._powheg_executable += '/Wj/pwhg_main'

    ## Decorate with generic option sets
    self.add_parameter_set( 'CKM' )
    self.add_parameter_set( 'LHEv3' )
    self.add_parameter_set( 'MiNLO NNLL' )
    self.add_parameter_set( 'NNLO reweighting' )
    self.add_parameter_set( 'running scales' )
    self.add_parameter_set( 'sin**2 theta W' )
    self.add_parameter_set( 'single vector boson' )
    self.add_parameter_set( 'V+jets' )
    self.add_parameter_set( 'vector boson decay' )
    self.add_parameter_set( 'v2' )
    self.add_parameter_set( 'W ID' )
    self.add_parameter_set( 'W mass window' )

    ## Set optimised integration parameters
    self.ncall1   = 40000
    self.ncall2   = 100000
    self.nubound  = 60000
    self.xupbound = 6
    self.foldx    = 5
    self.foldy    = 5
    self.foldphi  = 5

    ## Override defaults
    self.bornktmin   = 5.0
    self.mass_W_low  = 2.5
    self.mass_W_high = 2.0 * self.beam_energy
    self.withdamp    = 1

    self.populate_default_strings()
