# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_t_tch_4FS
#  Powheg configuration for t_tch_4FS subprocess
#
#  Authors: Riccardo Di Sipio <disipio@cern.ch>
#           James Robinson <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from .. import ATLASCommonParameters

# Default Powheg configuration for t-channel single top generation in the four-flavour scheme
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_t_tch_4FS(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_t_tch_4FS, self).__init__(runArgs, opts)
        self._powheg_executable += "/ST_tch_4f/pwhg_main"

        # Add process specific options
        self.add_parameter("alphaem_inv", 1.0 / float(ATLASCommonParameters.alphaem), desc="EM coupling reciprocal")
        self.add_parameter("mass_b", ATLASCommonParameters.mass_b, desc="b quark mass in t decay", parameter="bmass")
        self.add_parameter("mass_W", ATLASCommonParameters.mass_W, desc="mass of W boson in GeV", parameter="wmass")

        # Add phantom parameters used by the MadSpin runcard
        self.add_phantom("mass_tau", ATLASCommonParameters.mass_b, desc="b quark mass in t decay")
        self.add_phantom("width_W", ATLASCommonParameters.width_W, desc="width of W boson in GeV")

        # Decorate with generic option sets
        self.add_parameter_set("CKM")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("MadSpin")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("sin**2 theta W")
        self.add_parameter_set("top decay mode")
        self.add_parameter_set("top properties")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")
        self.add_parameter_set("PDF reweighting")

        # Set optimised integration parameters
        self.ncall2   = 250000
        self.nubound  = 200000
        self.xupbound = 2
        self.foldx    = 10
        self.foldy    = 10
        self.foldphi  = 5

        # Override defaults
        self.MadSpin_process = "generate p p > t b~ j $$ w+ w- [QCD]"
        self.topdecaymode    = "00000" # use MadSpin to decay the tops by default
        self.withdamp        = 1

        self.populate_default_strings()
