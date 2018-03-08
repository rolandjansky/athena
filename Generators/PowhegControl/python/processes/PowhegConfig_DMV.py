# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_DMV
#  Powheg configuration for DMV subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for DMV generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_DMV(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_DMV, self).__init__(runArgs, opts)
        self._powheg_executable += "/DMV/pwhg_main"

        # Add process specific options
        self.add_parameter("V_mass", 100,     desc="mediator mass",  parameter="DMVmass")
        self.add_parameter("V_width", 42.494, desc="mediator width", parameter="DMVwidth")

        # Decorate with generic option sets
        self.add_parameter_set("dark matter")
        self.add_parameter_set("extra tests")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("mass window")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("running scale", description="(0:2mX fixed; 1:pTj; 2:XXbar invmass; 3:Ht/2)")
        self.add_parameter_set("running width")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")
        self.add_parameter_set("vector boson decay mode")

        # Set optimised integration parameters
        self.ncall1   = 40000
        self.ncall2   = 80000
        self.nubound  = 80000
        self.xupbound = 4
        self.foldx    = 2
        self.foldy    = 2
        self.foldphi  = 2

        # Override defaults
        self.bornktmin    = 100.0
        self.doublefsr    = 1
        self.vdecaymode   = -1
        self.runningscale = 3

        self.populate_default_strings()
