# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_ZZ
#  Powheg configuration for ZZ subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for ZZ generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_ZZ(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_ZZ, self).__init__(runArgs, opts)
        self._powheg_executable += "/ZZ/pwhg_main"

        # Add process specific options
        self.add_parameter("cutallpairs", -1, desc="(-1:Powheg-default)")

        # Decorate with generic option sets
        self.add_parameter_set("diboson")
        self.add_parameter_set("diboson interference")
        self.add_parameter_set("dilepton mass")
        self.add_parameter_set("fixed scale")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("running width")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")
        self.add_parameter_set("zero width")

        # Set optimised integration parameters
        self.ncall1   = 50000
        self.ncall2   = 50000
        self.nubound  = 200000
        self.xupbound = 10
        self.itmx1    = 5
        self.itmx2    = 14
        self.foldx    = 2
        self.foldy    = 2

        # Override defaults
        # See https://docs.google.com/spreadsheets/d/1Aa7FwB74ppHbXles5LyHrKGlvUFi5PxbZC-Mrc3Lz90 for meanings
        self.allowed_decay_modes = ["ZZllll", "ZZqqll", "ZZqqqq", "ZZvvvv", "ZZvvll",
                                    "ZZvvqq", "ZZeeee", "ZZmumumumu", "ZZtautautautau", "ZZmumuee",
                                    "ZZeetautau", "ZZtautaumumu", "ZZvvee", "ZZvvmumu", "ZZvvtautau"]
        self.decay_mode          = "ZZllll"
        self.withdamp            = 1

        self.populate_default_strings()
