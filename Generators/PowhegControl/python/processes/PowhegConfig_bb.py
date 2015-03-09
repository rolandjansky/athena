# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg bb subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#            Daniel Hayden   <danhayden0@googlemail.com>
#            Stephen Bieniek <stephen.paul.bieniek@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators
from .. import ATLASCommonParameters

###############################################################################
#
#  bb
#
###############################################################################
class PowhegConfig_bb(PowhegConfig_base) :
  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_bb, self).__init__( runArgs, opts )
    self._powheg_executable += '/hvq/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'heavy quark' )
    PowhegDecorators.decorate( self, 'v2' )

    # Set optimised integration parameters
    self.ncall1   = 10000
    self.ncall2   = 20000
    self.nubound  = 20000
    self.xupbound = 2
    self.foldx    = 5
    self.foldy    = 5
    self.foldphi  = 2

    # Override defaults
    self.bornktmin  = 5.0

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'qmass '+str(ATLASCommonParameters.mass_b)+'     ! mass of heavy quark in GeV\n' )
      f.write( 'topdecaymode 0                                  ! disable topdecaymode\n' )
