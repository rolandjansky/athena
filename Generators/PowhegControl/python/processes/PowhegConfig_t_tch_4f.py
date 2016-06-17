# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_t_tch_4f
#  Powheg configuration for t_tch_4f subprocess
#
#  Authors: Riccardo Di Sipio <disipio@cern.ch>
#           James Robinson <james.robinson@cern.ch>

#! /usr/bin/env python
import os
from ..PowhegConfig_base import PowhegConfig_base
from .. import ATLASCommonParameters

# Default Powheg configuration for t_tch_4f generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_t_tch_4f(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_t_tch_4f, self).__init__(runArgs, opts)
        self._powheg_executable += "/ST_tch_4f/pwhg_main"
        self._MadSpin_executable = os.environ["MADPATH"] + "/MadSpin/madspin"

        # Add process specific options
        self.add_parameter("mass_b", ATLASCommonParameters.mass_b, desc="b quark mass in t decay", parameter="bmass")

        # Add phantom parameters used by the MadSpin runcard
        self.add_phantom("alphaqcd", ATLASCommonParameters.alphaqcd,       desc="Strong coupling")
        self.add_phantom("mass_H", ATLASCommonParameters.mass_H,           desc="Higgs mass")
        self.add_phantom("G_F", ATLASCommonParameters.G_F,                 desc="Fermi coupling")
        self.add_phantom("W_hadron_BR", ATLASCommonParameters.W_hadron_BR, desc="W branching ratio to hadrons")
        self.add_phantom("W_lepton_BR", ATLASCommonParameters.W_lepton_BR, desc="W branching ratio to leptons")
        self.add_phantom("width_H", ATLASCommonParameters.width_H,         desc="Higgs width")

        # Decorate with generic option sets
        self.add_parameter_set("CKM")
        self.add_parameter_set("electroweak properties", boson="w")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("lepton mass", prefix="tdec")
        self.add_parameter_set("sin**2 theta W")
        self.add_parameter_set("top decay branching", prefix="tdec")
        self.add_parameter_set("top decay mode")
        self.add_parameter_set("top properties")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("extra tests")

        # Set optimised integration parameters
        self.ncall2   = 250000
        self.nubound  = 200000
        self.xupbound = 2
        self.foldx    = 10
        self.foldy    = 10
        self.foldphi  = 5

        # Override defaults
        self.topdecaymode = 00000  # use MadSpin to decay the tops
        self.withdamp     = 1

        self.populate_default_strings()
