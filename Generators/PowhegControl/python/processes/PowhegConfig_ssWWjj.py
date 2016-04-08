# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_ssWWjj
#  Powheg configuration for WW subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

## Default Powheg configuration for WW generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_ssWWjj(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_ssWWjj, self).__init__( runArgs, opts )
    self._powheg_executable += '/Wp_Wp_J_J/pwhg_main'

    ## Logger warnings for unvalidated process
    self.logger.warning( 'Integration parameters have not been validated - see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PowhegForATLAS#Changing_POWHEG_BOX_integration' )

    ## Add process specific options
    self.add_parameter( 'smincuts', -1, desc='(default -1, Powheg-default)' )

    ## Decorate with generic option sets
    self.add_parameter_set( 'fake virtual' )
    self.add_parameter_set( 'LHEv3' )
    self.add_parameter_set( 'v2' )
    self.add_parameter_set( 'WW decay' )

    ## Set optimised integration parameters
    self.itmx1   = 3
    self.itmx2   = 4
    self.ncall1  = 1000000
    self.ncall2  = 100000
    self.nubound = 100000
    self.foldx   = 5
    self.foldy   = 5
    self.foldphi = 10

    ## Override defaults
    self.withdamp = 0

    ## Override defaults
    self.allowed_decay_modes = [ 'WpWpevev', 'WpWpevmuv', 'WpWpevtauv', 'WpWpevemuv', 'WpWpevlv',\
                                 'WmWmevev', 'WmWmevmuv', 'WmWmevtauv', 'WmWmevemuv', 'WmWmevlv',\
                                 'WpWpmuvmuv', 'WpWpmuvtauv', 'WpWpmuvemuv', 'WpWpmuvlv',\
                                 'WmWmmuvmuv', 'WmWmmuvtauv', 'WmWmmuvemuv', 'WmWmmuvlv',\
                                 'WpWptauvtauv', 'WpWptauvemuv', 'WpWptauvlv',\
                                 'WmWmtauvtauv', 'WmWmtauvemuv', 'WmWmtauvlv',\
                                 'WpWpemuvemuv', 'WpWpemuvlv',\
                                 'WmWmemuvemuv', 'WmWmemuvlv',\
                                 'WpWplvlv', 'WmWmlvlv' ]
    self.decay_mode = 'WpWplvlv'
    self.minlo      = -1

    self.populate_default_strings()
