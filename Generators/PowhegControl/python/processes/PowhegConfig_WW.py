# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_WW
#  Powheg configuration for WW subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

## Default Powheg configuration for WW generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_WW(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_WW, self).__init__( runArgs, opts )
    self._powheg_executable += '/WW/pwhg_main'

    ## Decorate with generic option sets
    self.add_parameter_set( 'anomalous coupling' )
    self.add_parameter_set( 'diboson' )
    self.add_parameter_set( 'extra tests' )
    self.add_parameter_set( 'fixed scale' )
    self.add_parameter_set( 'generic scale' )
    self.add_parameter_set( 'old Dij' )
    self.add_parameter_set( 'PDF reweighting' )
    self.add_parameter_set( 'running width' )
    self.add_parameter_set( 'upper bound' )
    self.add_parameter_set( 'v2' )
    self.add_parameter_set( 'zero width' )

    ## Set optimised integration parameters
    self.ncall1  = 40000
    self.ncall2  = 40000
    self.nubound = 60000
    self.itmx1   = 3

    ## Override defaults
    # See https://docs.google.com/spreadsheets/d/1Aa7FwB74ppHbXles5LyHrKGlvUFi5PxbZC-Mrc3Lz90 for meanings
    self.allowed_decay_modes = [ 'WpWmevev', 'WpWmmuvmuv', 'WpWmtauvtauv', 'WpWmevmuv', 'WpWmmuvev',\
                                 'WpWmevtauv', 'WpWmtauvev', 'WpWmmuvtauv', 'WpWmtauvmuv', 'WWevmuv',\
                                 'WWmuvtauv', 'WWevtauv', 'WWlvlv', 'WWqqqq', 'WWlvqq' ]
    self.decay_mode = 'WWlvlv'
    self.minlo      = -1
    self.withdamp   = 1

    self.populate_default_strings()
