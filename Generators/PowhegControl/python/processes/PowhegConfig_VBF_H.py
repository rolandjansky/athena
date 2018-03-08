# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_VBF_H
#  Powheg configuration for VBF_H subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

# Default Powheg configuration for VBF_H generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_VBF_H(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_VBF_H, self).__init__(runArgs, opts)
        self._powheg_executable += "/VBF_H/pwhg_main"

        # Add process specific options
        self.add_parameter("complexpolescheme", -1, desc="(0:SM)")

        # Decorate with generic option sets
        self.add_parameter_set("extra tests")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("Higgs decay mode")
        self.add_parameter_set("Higgs fixed width")
        self.add_parameter_set("Higgs properties")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")

        # Set optimised integration parameters
        self.ncall1   = 20000
        self.ncall2   = 300000
        self.nubound  = 300000
        self.xupbound = 4
        self.foldx    = 2
        self.foldy    = 2
        self.foldphi  = 5

        # Override defaults
        self.withdamp = 1

        self.populate_default_strings()
