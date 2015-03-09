# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg ZZ subprocess
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
#  ZZ
#
###############################################################################
class PowhegConfig_ZZ(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  cutallpairs      = -1
  mllmin           = 4

  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_ZZ, self).__init__( runArgs, opts )
    self._powheg_executable += '/ZZ/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'diboson' )
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'v2' )

    # Set optimised integration parameters
    self.ncall1   = 50000
    self.ncall2   = 50000
    self.nubound  = 100000
    self.xupbound = 10
    self.itmx1    = 5
    self.itmx2    = 14
    self.foldx    = 2
    self.foldy    = 2

    # Override defaults
    self.allowed_decay_modes = [ 'ZZllll', 'ZZqqll', 'ZZqqqq', 'ZZvvvv', 'ZZvvll', 'ZZvvqq', 'ZZeeee', 'ZZmumumumu', 'ZZtautautautau', 'ZZmumuee', 'ZZeetautau', 'ZZtautaumumu' ]
    self.decay_mode = 'ZZllll'
    self._use_delta_lambda = False

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'cutallpairs '+str(self.cutallpairs)+'   ! \n' )
      f.write( 'mllmin '+str(self.mllmin)+'             ! Minimum invariant mass of lepton pairs from Z decay\n' )
