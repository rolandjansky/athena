# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_Z
#  Powheg configuration for Z subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for Z generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_Z(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_Z, self).__init__(runArgs, opts)
        self._powheg_executable += "/Z/pwhg_main"

        # Decorate with generic option sets
        self.add_parameter_set("electroweak properties", boson="W")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("mass window")
        self.add_parameter_set("running scale")
        self.add_parameter_set("running width")
        self.add_parameter_set("second generation quark mass", prefix="lhe")
        self.add_parameter_set("sin**2 theta W")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("vector boson decay mode")

        # Set optimised integration parameters
        self.ncall1  = 120000
        self.ncall2  = 250000
        self.nubound = 20000
        self.foldx   = 2
        self.foldy   = 2
        self.foldphi = 2

        # Override defaults
        self.withsubtr      = 1
        self.withnegweights = 1
        self.mass_low       = 60.0
        self.mass_high      = 2.0 * self.beam_energy

        self.populate_default_strings()
