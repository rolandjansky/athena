# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_ggF_H
#  Powheg configuration for ggF_H subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from .. import ATLASCommonParameters

# Default Powheg configuration for ggF_H generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_ggF_H(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_ggF_H, self).__init__(runArgs, opts)
        self._powheg_executable += "/gg_H_quark-mass-effects/pwhg_main"

        # Add process specific options
        self.add_parameter("bwshape", 1,                        desc="Functional form of Breit-Wigner used to distribute Higgs virtuality. 1:running width; 2:hwidth")
        self.add_parameter("ew", 1,                             desc="(0:disabled; 1:enabled) EW corrections")
        self.add_parameter("gfermi", ATLASCommonParameters.G_F, desc="Fermi constant")
        self.add_parameter("massren", 0,                        desc="(0 = OS, 1 = MSBAR, 2 = DRBAR)")
        self.add_parameter("model", 0,                          desc="(0 = SM)")

        # Decorate with generic option sets
        self.add_parameter_set("extra tests")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("Higgs decay mode")
        self.add_parameter_set("Higgs decay properties")
        self.add_parameter_set("Higgs properties")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("running scale", description="(0:scales equal to the Higgs pole mass; 1:scales equal to the Higgs virtuality)")
        self.add_parameter_set("top properties")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")
        self.add_parameter_set("zero width")

        # Set optimised integration parameters
        self.ncall1  = 50000
        self.ncall2  = 100000
        self.nubound = 50000

        # Override defaults
        self.hfact = 104.16

        self.populate_default_strings()
