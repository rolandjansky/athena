# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_Zjj
#  Powheg configuration for Zjj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for Zjj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_Zjj(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_Zjj, self).__init__(runArgs, opts)
        self._powheg_executable += "/Z2jet/pwhg_main"

        # Logger warnings for unvalidated process
        self.logger.warning("Integration parameters have not been validated - see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PowhegForATLAS#Changing_POWHEG_BOX_integration")

        # Add process specific options
        self.add_parameter("mllmax", 2.0 * self.beam_energy, desc="Maximum invariant mass of lepton pairs from Z decay")
        self.add_parameter("vdecaymodeZ", 11,                desc="(11:e-; 13:mu-). PDG ID of first Z decay product (must be positive)")

        # Decorate with generic option sets
        self.add_parameter_set("Breit-Wigner width")
        self.add_parameter_set("dilepton mass")
        self.add_parameter_set("electroweak properties", boson="w")
        self.add_parameter_set("fake virtual")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("radiation parametrisation")
        self.add_parameter_set("running scales")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")
        self.add_parameter_set("Vector boson + two jets")

        # Set optimised integration parameters
        self.itmx1    = 5
        self.itmx2    = 5
        self.ncall1   = 8000000
        self.ncall2   = 12000000
        self.nubound  = 32000000
        self.xupbound = 4
        self.foldx    = 25
        self.foldy    = 25
        self.foldphi  = 25

        # Override defaults
        self.bornktmin = 10.0
        self.minlo     = 1
        self.mllmin    = 60.0
        self.withdamp  = 1

        self.populate_default_strings()
