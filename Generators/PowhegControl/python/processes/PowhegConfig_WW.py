# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_WW
#  Powheg configuration for WW subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for WW generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_WW(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_WW, self).__init__( runArgs, opts )
    self._powheg_executable += '/WW/pwhg_main'

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'anomalous coupling' )
    PowhegDecorators.decorate( self, 'diboson' )
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'running width' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )
    PowhegDecorators.decorate( self, 'zero width' )

    ## Set optimised integration parameters
    self.ncall1  = 40000
    self.ncall2  = 40000
    self.nubound = 60000
    self.itmx1   = 3

    ## Override defaults
    self.allowed_decay_modes = [ 'WWevev', 'WWmuvmuv', 'WWtauvtauv', 'WWevmuv', 'WWmuvtauv', 'WWevtauv', 'WWlvlv', 'WWqqqq', 'WWlvqq' ]
    self.decay_mode = 'WWlvlv'
    self.minlo      = -1


  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()
