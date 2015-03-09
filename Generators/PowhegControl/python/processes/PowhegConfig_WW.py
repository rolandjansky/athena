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
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

###############################################################################
#
#  WW
#
###############################################################################
class PowhegConfig_WW(PowhegConfig_base) :
  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_WW, self).__init__( runArgs, opts )
    self._powheg_executable += '/WW/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'diboson' )
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'v2' )

    # Set optimised integration parameters
    self.ncall1  = 40000
    self.ncall2  = 40000
    self.nubound = 60000
    self.itmx1   = 3

    # Override defaults
    self.allowed_decay_modes = [ 'WWevev', 'WWmuvmuv', 'WWtauvtauv', 'WWevmuv', 'WWmuvtauv', 'WWevtauv', 'WWlvlv', 'WWqqqq', 'WWlvqq' ]
    self.decay_mode = 'WWlvlv'

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()
