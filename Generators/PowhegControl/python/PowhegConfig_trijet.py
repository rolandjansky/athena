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
from PowhegConfig_base import PowhegConfig_base
import PowhegDecorators

###############################################################################
#
#  trijet
#
###############################################################################
class PowhegConfig_trijet(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  rapsuppfact   = -1
  HWJsudakov    = -1
  ktmerging     = -1
  raisingscales = -1
  fullphsp      = -1
  brpar_dijexp  = -1
  brpar_diexp   = -1
  itmx1rm       = None
  ncall1rm      = None

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    super(PowhegConfig_trijet, self).__init__(runArgs)
    self._powheg_executable += '/trijet/pwhg_main'
    self._logger.warning( 'This process is UNVALIDATED! Behaviour may change between now and validation.' )

    # Add decorators
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'v2' )

    # Set optimised integration parameters
    self.ncall1   = 500000
    self.ncall2   = 200000
    self.nubound  = 100000
    self.xupbound = 3
    self.itmx1    = 10
    self.itmx2    = 10
    self.foldx    = 10
    self.foldy    = 10
    self.foldphi  = 5
    self.ixmax    = 5
    self.iymax    = 5

    # Override defaults
    self.bornktmin      = 5.0
    self.withnegweights = 1
    self.minlo          = 1
    #self.storemintupb   = 1
    #self.fastbtlbound   = 1

  # Implement base-class function
  def generateRunCard(self) :
    # Set additional integration parameters before setting up the run card
    self.itmx1rm = self.itmx1
    self.ncall1rm = 2.0 * self.ncall1

    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'rapsuppfact '+str(self.rapsuppfact)+'     ! \n' )
      f.write( 'HWJsudakov '+str(self.HWJsudakov)+'       ! \n' )
      f.write( 'ktmerging '+str(self.ktmerging)+'         ! \n' )
      f.write( 'raisingscales '+str(self.raisingscales)+' ! \n' )
      f.write( 'fullphsp '+str(self.fullphsp)+'           ! \n' )
      f.write( 'brpar_dijexp '+str(self.brpar_dijexp)+'   ! \n' )
      f.write( 'brpar_diexp '+str(self.brpar_diexp)+'     ! \n' )
