# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_trijet
#  Powheg configuration for trijet subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for trijet generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_trijet(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_trijet, self).__init__( runArgs, opts )
    self._powheg_executable += '/trijet/pwhg_main'

    ## Logger warnings for unvalidated process
    self.logger().warning( 'This process is UNVALIDATED! Behaviour may change between now and validation.' )
    self.logger().warning( 'Born suppression is enabled by default! This should NOT be used in conjunction with J-slicing.' )
    self.logger().warning( 'Negative weight fraction is outside usual recommendations - check final distributions with care!' )

    ## Add process specific options
    self.brpar_diexp  = -1
    self.brpar_dijexp = -1
    self.HWJsudakov   = -1
    self.ktmerging    = -1
    self.rapsuppfact  = -1

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'extra tests' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'radiation parametrisation' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )

    ## Set optimised integration parameters
    self.ncall1   = 800000
    self.ncall1rm = 2000000
    self.ncall2   = 1750000
    self.nubound  = 1000000
    self.itmx1    = 2
    self.itmx1rm  = 2
    self.itmx2    = 10
    self.ixmax    = 5
    self.iymax    = 5

    ## Override defaults
    self.bornktmin      = 20.0
    self.bornsuppfact   = 1000.0
    self.doublefsr      = 1
    self.withdamp       = 1
    self.withnegweights = 1

  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( self.runcard_path(), 'a' ) as f :
      f.write( 'brpar_diexp '+str(self.brpar_diexp)+'   ! \n' )
      f.write( 'brpar_dijexp '+str(self.brpar_dijexp)+' ! \n' )
      f.write( 'HWJsudakov '+str(self.HWJsudakov)+'     ! \n' )
      f.write( 'ktmerging '+str(self.ktmerging)+'       ! \n' )
      f.write( 'rapsuppfact '+str(self.rapsuppfact)+'   ! \n' )
