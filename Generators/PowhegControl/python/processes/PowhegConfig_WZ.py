# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_WZ
#  Powheg configuration for WZ subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for WZ generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_WZ(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_WZ, self).__init__(runArgs, opts)
        self._powheg_executable += "/WZ/pwhg_main"

        # Add process specific options
        self.add_parameter("diagCKM", 0, desc="(default 0) 0:normal CKM; 1:diagonal CKM")

        # Decorate with generic option sets
        self.add_parameter_set("anomalous coupling")
        self.add_parameter_set("diboson")
        self.add_parameter_set("diboson interference")
        self.add_parameter_set("dilepton mass")
        self.add_parameter_set("extra tests")
        self.add_parameter_set("fixed scale")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("running width")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")
        self.add_parameter_set("zero width")

        # Set optimised integration parameters
        self.ncall1   = 30000
        self.ncall2   = 150000
        self.nubound  = 300000
        self.itmx2    = 12
        self.xupbound = 10

        # Override defaults
        # See https://docs.google.com/spreadsheets/d/1Aa7FwB74ppHbXles5LyHrKGlvUFi5PxbZC-Mrc3Lz90 for meanings
        self.allowed_decay_modes = ["WZevee", "WZmuvmumu", "WZtauvtautau", "WpZevee", "WmZevee",
                                    "WpZmuvmumu", "WmZmuvmumu", "WpZtauvtautau", "WmZtauvtautau", "WZevmumu",
                                    "WpZevmumu", "WmZevmumu", "WZmuvee", "WpZmuvee", "WmZmuvee",
                                    "WZevtautau", "WpZevtautau", "WmZevtautau", "WZmuvtautau", "WpZmuvtautau",
                                    "WmZmuvtautau", "WZtauvee", "WpZtauvee", "WmZtauvee", "WZtauvmumu",
                                    "WpZtauvmumu", "WmZtauvmumu", "WZlvqq", "WZqqll", "WZlvll",
                                    "WZlvvv", "WZqqvv", "WZqqqq"]
        self.decay_mode          = "WZlvll"
        self.withdamp            = 1

        self.populate_default_strings()
