# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg Wj subprocess
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
#  Wj
#
###############################################################################
class PowhegConfig_Wj(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  idvecbos = 24

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    super(PowhegConfig_Wj, self).__init__(runArgs)
    self._powheg_executable += '/Wj/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'CKM' )
    PowhegDecorators.decorate( self, 'single boson' )

    # Set optimised integration parameters
    self.ncall1   = 60000
    self.ncall2   = 80000
    self.nubound  = 40000
    self.xupbound = 3
    self.foldx    = 10
    self.foldy    = 10
    self.foldphi  = 5

    # Override defaults
    self.bornktmin       = 5.0
    self.masswindow_low  = 2.5
    self.masswindow_high = 2.0 * self.beam_energy

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'idvecbos '+str(self.idvecbos)+'         ! PDG code for vector boson to be produced (W:24)\n' )
