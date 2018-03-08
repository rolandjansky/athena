# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_VBF_Z
#  Powheg configuration for VBF Z subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for VBF Z generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_VBF_Z(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_VBF_Z, self).__init__(runArgs, opts)
        self._powheg_executable += "/VBF_W-Z/pwhg_main"

        # Logger warnings for unvalidated process
        self.logger.warning("Integration parameters have not been validated - see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PowhegForATLAS#Changing_POWHEG_BOX_integration")

        # Decorate with generic option sets
        self.add_parameter_set("fake virtual")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("VBF vector boson decay")

        # Set optimised integration parameters
        self.itmx1    = 4
        self.itmx2    = 5
        self.ncall1   = 65000000
        self.ncall2   = 150000000
        self.nubound  = 10000000
        self.xupbound = 6
        self.foldx    = 25
        self.foldy    = 25
        self.foldphi  = 25

        # Override defaults
        self.allowed_decay_modes = ["Zee", "Zmumu"]
        self.decay_mode          = "Zee"
        self.mll_gencut          = 60
        self.withdamp            = 1

        self.populate_default_strings()
