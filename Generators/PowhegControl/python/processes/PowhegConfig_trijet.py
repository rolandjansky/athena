# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg trijet subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#            Daniel Hayden   <danhayden0@googlemail.com>
#            Stephen Bieniek <stephen.paul.bieniek@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

###############################################################################
#
#  trijet
#
###############################################################################
class PowhegConfig_trijet(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  brpar_diexp   = -1
  brpar_dijexp  = -1
  fullphsp      = -1
  HWJsudakov    = -1
  ktmerging     = -1
  raisingscales = -1
  rapsuppfact   = -1

  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_trijet, self).__init__( runArgs, opts )
    self._powheg_executable += '/trijet/pwhg_main'
    self._logger.warning( 'This process is UNVALIDATED! Behaviour may change between now and validation.' )
    self._logger.warning( 'Born suppression is enabled by default! This should NOT be used in conjunction with J-slicing.' )
    self._logger.warning( 'Negative weight fraction is outside usual recommendations - check final distributions with care!' )

    # Add decorators
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'v2' )

    # Set optimised integration parameters
    self.ncall1   = 9800000
    self.ncall1rm = 92000000
    self.ncall2   = 91750000
    self.nubound  = 91000000
    self.itmx1    = 2
    self.itmx1rm  = 2
    self.itmx2    = 10
    self.ixmax    = 5
    self.iymax    = 5
    
    # Override defaults
    self.bornktmin      = 20.0
    self.bornsuppfact   = 1000.0
    self.doublefsr      = 1
    self.minlo          = 1
    self.withdamp       = 1
    self.withnegweights = 1

  # Implement base-class function
  def generateRunCard(self) :
    # Set additional integration parameters before setting up the run card
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'brpar_diexp '+str(self.brpar_diexp)+'     ! \n' )
      f.write( 'brpar_dijexp '+str(self.brpar_dijexp)+'   ! \n' )
      f.write( 'fullphsp '+str(self.fullphsp)+'           ! \n' )
      f.write( 'HWJsudakov '+str(self.HWJsudakov)+'       ! \n' )
      f.write( 'ktmerging '+str(self.ktmerging)+'         ! \n' )
      f.write( 'raisingscales '+str(self.raisingscales)+' ! \n' )
      f.write( 'rapsuppfact '+str(self.rapsuppfact)+'     ! \n' )
