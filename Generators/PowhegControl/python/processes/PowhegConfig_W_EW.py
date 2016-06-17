# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_W_EW
#  Powheg configuration for W_EW subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for W_EW generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_W_EW(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_W_EW, self).__init__(runArgs, opts)
        self._powheg_executable += "/W_ew-BMNNP/pwhg_main"
        self._PHOTOS_executable = self.powheg_directory + "/W_ew-BMNNP/main-PHOTOS-lhef"

        # Add process specific options
        self.add_parameter("kt2minqed", -1,  desc="(-1:Powheg default)")
        self.add_parameter("phsp_Wm", -1,    desc="(-1:Powheg default)")
        self.add_parameter("phsp_Ww", -1,    desc="(-1:Powheg default)")
        self.add_parameter("theta_damp", -1, desc="(-1:Powheg default)")

        # Decorate with generic option sets
        self.add_parameter_set("CKM")
        self.add_parameter_set("electroweak properties", boson="W")
        self.add_parameter_set("extra tests")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("mass window")
        self.add_parameter_set("NLO EW")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("running scale")
        self.add_parameter_set("second generation quark mass", prefix="lhe")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")
        self.add_parameter_set("vector boson decay mode")
        self.add_parameter_set("W ID")

        # Set optimised integration parameters
        self.ncall1  = 1000
        self.ncall2  = 10000
        self.nubound = 10000
        self.foldx   = 2
        self.foldy   = 2

        # Override defaults
        self.idvecbos       = -24
        self.vdecaymode     = 11
        self.mass_low       = 2.5
        self.mass_high      = 2.0 * self.beam_energy
        self.withdamp       = 1
        self.withnegweights = 1

        self.populate_default_strings()
