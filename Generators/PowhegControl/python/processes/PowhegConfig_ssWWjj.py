# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_ssWWjj
#  Powheg configuration for WW subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for WW generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_ssWWjj(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_ssWWjj, self).__init__(runArgs, opts)
        self._powheg_executable += "/Wp_Wp_J_J/pwhg_main"

        # Logger warnings for unvalidated process
        self.logger.warning("Integration parameters have not been validated - see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PowhegForATLAS#Changing_POWHEG_BOX_integration")

        # Add process specific options
        self.add_parameter("smincuts", -1, desc="(-1:Powheg-default)")

        # Decorate with generic option sets
        self.add_parameter_set("fake virtual")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("running scales")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")
        self.add_parameter_set("WW decay", boson="W")
        self.add_parameter_set("zero width")

        # Set optimised integration parameters
        self.itmx1   = 5
        self.itmx2   = 5
        self.ncall1  = 10000000
        self.ncall2  = 1000000
        self.nubound = 1000000
        self.foldx   = 25
        self.foldy   = 25
        self.foldphi = 25

        # Override defaults
        self.withdamp = 0

        # Override defaults
        self.allowed_decay_modes = ["WpWpevev", "WpWpevmuv", "WpWpevtauv", "WpWpevemuv", "WpWpevlv",
                                    "WmWmevev", "WmWmevmuv", "WmWmevtauv", "WmWmevemuv", "WmWmevlv",
                                    "WpWpmuvmuv", "WpWpmuvtauv", "WpWpmuvemuv", "WpWpmuvlv",
                                    "WmWmmuvmuv", "WmWmmuvtauv", "WmWmmuvemuv", "WmWmmuvlv",
                                    "WpWptauvtauv", "WpWptauvemuv", "WpWptauvlv",
                                    "WmWmtauvtauv", "WmWmtauvemuv", "WmWmtauvlv",
                                    "WpWpemuvemuv", "WpWpemuvlv",
                                    "WmWmemuvemuv", "WmWmemuvlv",
                                    "WpWplvlv", "WmWmlvlv"]
        self.decay_mode = "WpWplvlv"

        self.populate_default_strings()
