# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_bb
#  Powheg configuration for bb subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from .. import ATLASCommonParameters

# Default Powheg configuration for bb generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_bb(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_bb, self).__init__(runArgs, opts)
        self._powheg_executable += "/hvq/pwhg_main"

        # Decorate with generic option sets
        self.add_parameter_set("extra tests")
        self.add_parameter_set("fixed scale")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("heavy quark")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("second generation quark mass", prefix="lhe")
        self.add_parameter_set("top decay mode")
        self.add_parameter_set("top properties")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")

        # Set optimised integration parameters
        self.ncall1   = 10000
        self.ncall2   = 20000
        self.nubound  = 60000
        self.xupbound = 6
        self.foldx    = 5
        self.foldy    = 5
        self.foldphi  = 2

        # Override defaults
        self.bornktmin    = 5.0
        self.quark_mass   = ATLASCommonParameters.mass_b
        self.topdecaymode = 0

        self.populate_default_strings()
