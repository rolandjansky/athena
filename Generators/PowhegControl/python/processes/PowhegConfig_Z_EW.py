# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_Z_EW
#  Powheg configuration for Z_EW subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from .. import ATLASCommonParameters

# Default Powheg configuration for Z generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_Z_EW(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_Z_EW, self).__init__(runArgs, opts)
        self._powheg_executable += "/Z_ew-BMNNPV/pwhg_main"
        self._PHOTOS_executable = self.powheg_directory + "/Z_ew-BMNNPV/main-PHOTOS-lhef"

        # Add process specific options
        self.add_parameter("alphaem_z", ATLASCommonParameters.alphaem, desc="alphaem")
        self.add_parameter("complexmasses", -1,                        desc="(-1:Powheg default)")
        self.add_parameter("iftopinloop", -1,                          desc="(-1:Powheg default)")
        self.add_parameter("ktmaxqed", -1,                             desc="(-1:Powheg default)")
        self.add_parameter("mass_b", ATLASCommonParameters.mass_b,     desc="b mass",                parameter="Bmass")
        self.add_parameter("mass_c", ATLASCommonParameters.mass_c,     desc="c mass",                parameter="Cmass")
        self.add_parameter("mass_d", ATLASCommonParameters.mass_d,     desc="d mass",                parameter="Dmass")
        self.add_parameter("mass_e", ATLASCommonParameters.mass_e,     desc="electron mass",         parameter="Elmass")
        self.add_parameter("mass_mu", ATLASCommonParameters.mass_mu,   desc="muon mass",             parameter="Mumass")
        self.add_parameter("mass_s", ATLASCommonParameters.mass_s,     desc="s mass",                parameter="Smass")
        self.add_parameter("mass_tau", ATLASCommonParameters.mass_tau, desc="tau mass",              parameter="Taumass")
        self.add_parameter("mass_u", ATLASCommonParameters.mass_u,     desc="u mass",                parameter="Umass")
        self.add_parameter("photoninduced", -1,                        desc="(-1:Powheg default)")
        self.add_parameter("phsp_Zm", -1,                              desc="(-1:Powheg default)")
        self.add_parameter("phsp_Zw", -1,                              desc="(-1:Powheg default)")
        self.add_parameter("width_H", ATLASCommonParameters.width_H,   desc="width of Higgs in GeV", parameter="Hwidth")

        # Decorate with generic option sets
        self.add_parameter_set("electroweak properties", boson="W", coupling="alphaem")
        self.add_parameter_set("generic scale")
        self.add_parameter_set("mass window")
        self.add_parameter_set("MiNLO NNLL")
        self.add_parameter_set("NLO EW")
        self.add_parameter_set("old Dij")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("radiation parametrisation")
        self.add_parameter_set("running scale")
        self.add_parameter_set("running scales")
        self.add_parameter_set("second generation quark mass", prefix="lhe")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("v2")
        self.add_parameter_set("vector boson decay mode")

        # Set optimised integration parameters
        self.ncall1  = 1000
        self.ncall2  = 10000
        self.nubound = 20000
        self.foldx   = 2
        self.foldy   = 2
        self.foldphi = 1

        # Override defaults
        self.mass_low       = 60.0
        self.mass_high      = 2.0 * self.beam_energy
        self.vdecaymode     = 11
        self.withnegweights = 1

        self.populate_default_strings()
