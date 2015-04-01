# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_tt
#  Powheg configuration for tt subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators
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
    self.semileptonic = -1

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'heavy quark' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'second generation quark mass' )
    PowhegDecorators.decorate( self, 'top decay lepton' )
    PowhegDecorators.decorate( self, 'top decay mode' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )

    ## Set optimised integration parameters
    self.ncall1   = 10000
    self.ncall2   = 100000
    self.nubound  = 100000
    self.xupbound = 2

    ## Override defaults
    self.minlo        = -1
    self.topdecaymode = 22222

  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.quark_mass = ATLASCommonParameters.mass_t
    self.initialiseRunCard()

    with open( self.runcard_path(), 'a' ) as f :
      f.write( 'semileptonic '+str(self.semileptonic)+'                     ! only accept semileptonic decays\n' )
      f.write( 'tdec/bmass '+str(ATLASCommonParameters.mass_b)+'            ! b quark mass in t decay\n' )
      f.write( 'tdec/cmass '+str(ATLASCommonParameters.mass_c)+'            ! c quark mass\n' )
      f.write( 'tdec/dmass '+str(ATLASCommonParameters.mass_d)+'            ! d quark mass\n' )
      f.write( 'tdec/sin2cabibbo '+str(ATLASCommonParameters.sin2cabibbo)+' ! sine of Cabibbo angle squared\n' )
      f.write( 'tdec/smass '+str(ATLASCommonParameters.mass_s)+'            ! s quark mass\n' )
      f.write( 'tdec/twidth '+str(ATLASCommonParameters.width_t)+'          ! top width\n' )
      f.write( 'tdec/umass '+str(ATLASCommonParameters.mass_u)+'            ! u quark mass\n' )
      f.write( 'tdec/wmass '+str(ATLASCommonParameters.mass_W)+'            ! W mass for top decay\n' )
      f.write( 'tdec/wwidth '+str(ATLASCommonParameters.width_W)+'          ! W width\n' )
