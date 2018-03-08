# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg HJJ subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

###############################################################################
#
#  HJJ
#
###############################################################################
class PowhegConfig_HJJ(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  fullphsp      = -1
  polecheck     = -1
  raisingscales = -1

  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_HJJ, self).__init__( runArgs, opts )
    self._powheg_executable += '/HJJ/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'Higgs v2' )
    PowhegDecorators.decorate( self, 'HJ' )

    # Set optimised integration parameters
    self.ncall1   = 9800000
    self.ncall1rm = 9900000
    self.ncall2   = 9800000
    self.nubound  = 9400000
    self.xupbound = 2
    self.foldx    = 5
    self.foldy    = 5
    self.foldphi  = 5
    
    # Override defaults
    self.bornktmin  = 5.0
    self.ckkwscalup = 1
    self.minlo      = 1
    self.factsc2min = 2.0
    self.frensc2min = 2.0

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'fullphsp '+str(self.fullphsp)+'           ! \n' )
      f.write( 'polecheck '+str(self.polecheck)+'         ! \n' )
      f.write( 'raisingscales '+str(self.raisingscales)+' ! \n' )
      
