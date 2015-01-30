# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg WW subprocesses
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
#  WW
#
###############################################################################
class PowhegConfig_WW(PowhegConfig_base) :
  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    super(PowhegConfig_WW, self).__init__(runArgs)
    self._powheg_executable += '/WW/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'diboson' )
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'v2' )

    # Set optimised integration parameters
    self.ncall1  = 40000 #50000
    self.ncall2  = 40000 #50000
    self.nubound = 60000 #80000
    self.itmx1   = 3

    # Override defaults
    self.allowed_decay_modes = [ 'e+e-', 'e+mu-', 'mu+mu-', 'tau+tau-', 'leptonic', 'hadronic', 'semileptonic' ]
    self.decay_mode = 'e+mu-'

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()
