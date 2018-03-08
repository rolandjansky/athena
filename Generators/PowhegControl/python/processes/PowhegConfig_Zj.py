# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_Zj
#  Powheg configuration for Zj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for Zj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_Zj(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_Zj, self).__init__(runArgs, opts)
        self._powheg_executable += "/Zj/pwhg_main"

        # Decorate with generic option sets
        self.add_parameter_set("electroweak properties", boson="W")
        self.add_parameter_set("extra tests")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("ISR/FSR")
        self.add_parameter_set("lepton mass", prefix="lhfm")
        self.add_parameter_set("MiNLO NNLL")
        self.add_parameter_set("NNLO reweighting")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("running scales")
        self.add_parameter_set("sin**2 theta W")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")
        self.add_parameter_set("vector boson decay mode")
        self.add_parameter_set("Z mass window")

        # Set optimised integration parameters
        self.ncall1   = 30000
        self.ncall2   = 350000
        self.nubound  = 3000000
        self.xupbound = 20
        self.foldx    = 10
        self.foldy    = 5
        self.foldphi  = 5

        # Override defaults
        self.bornktmin   = 5.0
        self.mass_Z_low  = 60.0
        self.mass_Z_high = 2.0 * self.beam_energy
        self.minlo       = 1

        self.populate_default_strings()
