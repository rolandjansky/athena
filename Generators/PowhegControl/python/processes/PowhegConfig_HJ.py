# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg HJ subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

###############################################################################
#
#  HJ
#
###############################################################################
class PowhegConfig_HJ(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  skipextratests = -1

  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_HJ, self).__init__( runArgs, opts )
    self._powheg_executable += '/HJ/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'Higgs v2' )
    PowhegDecorators.decorate( self, 'HJ' )

    # Set optimised integration parameters
    self.ncall1   = 150000
    self.ncall1rm = 150000
    self.ncall2   = 150000
    self.nubound  = 100000
    self.xupbound = 2
    self.foldx    = 5
    self.foldy    = 5
    self.foldphi  = 2

    # Override defaults
    self.bornktmin  = 0.26
    self.ckkwscalup = 1
    self.minlo      = 1
    self.factsc2min = 2.0
    self.frensc2min = 2.0
  
  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'skipextratests '+str(self.skipextratests)+' ! \n' )
      
