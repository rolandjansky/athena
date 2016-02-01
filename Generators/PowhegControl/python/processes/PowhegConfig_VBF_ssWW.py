# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_VBF_ssWW
#  Powheg configuration for VBF_ssWW subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

## Default Powheg configuration for VBF_ssWW generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_VBF_ssWW(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_VBF_ssWW, self).__init__( runArgs, opts )
    self._powheg_executable += '/vbf_wp_wp/pwhg_main'

    ## Logger warnings for unvalidated process
    self.logger.warning( 'Integration parameters have not been validated - see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PowhegForATLAS#Changing_POWHEG_BOX_integration' )

    ## Decorate with generic option sets
    self.add_parameter_set( 'fake virtual' )
    self.add_parameter_set( 'generic scale' )
    self.add_parameter_set( 'old Dij' )
    self.add_parameter_set( 'PDF reweighting' )
    self.add_parameter_set( 'running scales' )
    self.add_parameter_set( 'upper bound' )
    self.add_parameter_set( 'v2' )
    self.add_parameter_set( 'WW decay', boson='w' )
    self.add_parameter_set( 'zero width' )

    ## Set optimised integration parameters
    self.itmx1   = 3
    self.itmx2   = 6
    self.ncall1  = 800000
    self.ncall2  = 800000
    self.nubound = 500000
    self.foldx   = 2
    self.foldy   = 2
    self.foldphi = 2

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
    self.withdamp   = 1

    self.populate_default_strings()
