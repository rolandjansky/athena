# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_tt
#  Powheg configuration for tt subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from .. import ATLASCommonParameters

## Default Powheg configuration for tt generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_tt(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_tt, self).__init__( runArgs, opts )
    self._powheg_executable += '/hvq/pwhg_main'

    ## Add process specific options
    self.add_parameter( 'fullrwgt', -1,                                   desc='(-1:Powheg default)' )
    self.add_parameter( 'mass_b', ATLASCommonParameters.mass_b,           desc='b quark mass in t decay', parameter='tdec/bmass' )
    self.add_parameter( 'mass_c', ATLASCommonParameters.mass_c,           desc='c quark mass', parameter='tdec/cmass' )
    self.add_parameter( 'mass_d', ATLASCommonParameters.mass_d,           desc='d quark mass', parameter='tdec/dmass' )
    self.add_parameter( 'sin2cabibbo', ATLASCommonParameters.sin2cabibbo, desc='sine of Cabibbo angle squared', parameter='tdec/sin2cabibbo' )
    self.add_parameter( 'mass_s', ATLASCommonParameters.mass_s,           desc='s quark mass', parameter='tdec/smass' )
    self.add_parameter( 'width_t', ATLASCommonParameters.width_t,         desc='top width', parameter='tdec/twidth' )
    self.add_parameter( 'mass_u', ATLASCommonParameters.mass_u,           desc='u quark mass', parameter='tdec/umass' )
    self.add_parameter( 'mass_W', ATLASCommonParameters.mass_W,           desc='W mass for top decay', parameter='tdec/wmass' )
    self.add_parameter( 'width_W', ATLASCommonParameters.width_W,         desc='W width', parameter='tdec/wwidth' )

    ## Decorate with generic option sets
    self.add_parameter_set( 'extra tests' )
    self.add_parameter_set( 'lepton mass', prefix='tdec' )
    self.add_parameter_set( 'top decay branching', prefix='tdec' )
    self.add_parameter_set( 'fixed scale' )
    self.add_parameter_set( 'generic scale' )
    self.add_parameter_set( 'heavy quark' )
    self.add_parameter_set( 'old Dij' )
    self.add_parameter_set( 'PDF reweighting' )
    self.add_parameter_set( 'second generation quark mass', prefix='lhe' )
    self.add_parameter_set( 'semileptonic' )
    self.add_parameter_set( 'top decay mode' )
    self.add_parameter_set( 'upper bound' )
    self.add_parameter_set( 'v2' )

    ## Set optimised integration parameters
    self.ncall1   = 10000
    self.ncall2   = 100000
    self.nubound  = 100000
    self.xupbound = 2

    ## Override defaults
    self.minlo        = -1
    self.quark_mass   = ATLASCommonParameters.mass_t
    self.topdecaymode = 22222

    self.populate_default_strings()
