# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_jjj
#  Powheg configuration for jjj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for jjj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_jjj(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_jjj, self).__init__(runArgs, opts)
        self._powheg_executable += "/trijet/pwhg_main"

        # Logger warnings for unvalidated process
        self.logger.warning("Integration parameters have not been validated - see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PowhegForATLAS#Changing_POWHEG_BOX_integration")

        # Add process specific options
        self.add_parameter("brpar_diexp", -1,  desc="(-1:Powheg default)")
        self.add_parameter("brpar_dijexp", -1, desc="(-1:Powheg default)")
        self.add_parameter("HWJsudakov", -1,   desc="(-1:Powheg default)")
        self.add_parameter("ktmerging", -1,    desc="(-1:Powheg default)")
        self.add_parameter("rapsuppfact", -1,  desc="(-1:Powheg default)")

        # Decorate with generic option sets
        self.add_parameter_set("extra tests")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("radiation parametrisation")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")

        # Set optimised integration parameters
        self.itmx1    = 2
        self.itmx2    = 10
        self.ncall1   = 500000
        self.ncall1rm = 1000000
        self.ncall2   = 1000000
        self.nubound  = 1500000
        self.xupbound = 4
        self.foldx    = 5
        self.foldy    = 5
        self.foldphi  = 2

        # NB. These settings give the following output
        # Cross-section test : 0.00% PASSED
        # Neg. weight test   : 13.40% FAILED
        # Upper bound test   : 0.45% PASSED

        # Override defaults
        self.bornktmin    = 1.0
        self.bornsuppfact = 800.0
        self.doublefsr    = 1
        self.minlo        = 1
        self.withdamp     = 1

        self.populate_default_strings()
