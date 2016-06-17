# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_HZj
#  Powheg configuration for HZj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for HZj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_HZj(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_HZj, self).__init__(runArgs, opts)
        self._powheg_executable += "/HZJ/pwhg_main"

        # Decorate with generic option sets
        self.add_parameter_set("generic scale")
        self.add_parameter_set("Higgs + vector boson", boson="Z")
        self.add_parameter_set("Higgs + vector boson + jet")
        self.add_parameter_set("Higgs properties")
        self.add_parameter_set("MiNLO NNLL")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("running scales")
        self.add_parameter_set("top properties")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")
        self.add_parameter_set("vector boson decay mode")
        self.add_parameter_set("Z mass window")

        # Set optimised integration parameters
        self.itmx1    = 8
        self.itmx2    = 5
        self.ncall1   = 60000
        self.ncall2   = 60000
        self.nubound  = 300000
        self.xupbound = 4
        self.foldx    = 10
        self.foldy    = 5
        self.foldphi  = 5

        # Override defaults
        self.doublefsr   = 1
        self.mass_Z_low  = 10.0
        self.mass_Z_high = 2.0 * self.beam_energy
        self.minlo       = 1

        self.populate_default_strings()
