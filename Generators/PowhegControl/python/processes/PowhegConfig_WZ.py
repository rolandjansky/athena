# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg WZ subprocess
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
#  WZ
#
###############################################################################
class PowhegConfig_WZ(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  diagCKM          = 0
  mllmin           = 10

  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_WZ, self).__init__( runArgs, opts )
    self._powheg_executable += '/WZ/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'diboson' )
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'v2' )

    # Set optimised integration parameters
    self.ncall1   = 40000
    self.ncall2   = 70000
    self.nubound  = 80000
    self.itmx1    = 4
    self.itmx2    = 8
    self.xupbound = 10

    # Override defaults
    self.allowed_decay_modes = [ 'WZevee', 'WZmuvmumu', 'WZtauvtautau', 'WZevmumu', 'WZmuvee', 'WZevtautau', 'WZmuvtautau', 'WZtauvee', 'WZtauvmumu', 'WZlvqq', 'WZqqll', 'WZlvll', 'WZlvvv', 'WZqqvv', 'WZqqqq', 'e+ee', 'e+mumu', 'e-mumu' ]
    self.decay_mode = 'WZlvll'

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'diagCKM '+str(self.diagCKM)+' ! if 1 (true) then diagonal CKM (default 0)\n' )
      f.write( 'mllmin '+str(self.mllmin)+'                     ! Minimum invariant mass of lepton pairs from Z decay\n' )
