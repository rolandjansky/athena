# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_ggF_HZ
#  Powheg configuration for ggF_HZ subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for ggF_HZ generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_ggF_HZ(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None, LO_process=True):
        super(PowhegConfig_ggF_HZ, self).__init__(runArgs, opts)
        self._powheg_executable += "/ggHZ/pwhg_main"

        # Decorate with generic option sets
        self.add_parameter_set("Higgs + vector boson", boson="Z")
        self.add_parameter_set("Higgs fixed width")
        self.add_parameter_set("Higgs properties")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("running scales")
        self.add_parameter_set("top properties")
        self.add_parameter_set("v2")
        self.add_parameter_set("vector boson decay mode")
        self.add_parameter_set("Z mass window")

        # Set optimised integration parameters
        self.ncall1   = 25000
        self.ncall2   = 60000
        self.nubound  = 60000
        self.xupbound = 6
        self.itmx1    = 1

        self.populate_default_strings()
