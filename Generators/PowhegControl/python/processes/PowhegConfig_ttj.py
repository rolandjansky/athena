# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_ttj
#  Powheg configuration for ttj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from .. import ATLASCommonParameters

# Default Powheg configuration for ttj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_ttj(PowhegConfig_base):

    # Constructor: set process-dependent executable path here
    # @param runArgs athena run arguments
    # @param opts athena run options
    def __init__(self, runArgs=None, opts=None):
        super(PowhegConfig_ttj, self).__init__(runArgs, opts)
        self._powheg_executable += "/ttJ/pwhg_main"

        # Logger warnings for unvalidated process
        self.logger.warning("Integration parameters have not been validated - see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PowhegForATLAS#Changing_POWHEG_BOX_integration")

        # Add process specific options
        self.add_parameter("bcut", -1,                                desc="(-1:Powheg default)")
        self.add_parameter("collremnsamp", -1,                        desc="(-1:Powheg default)", parameter='collremnsamp"')
        self.add_parameter("ffltest", -1,                             desc="(-1:Powheg default)")
        self.add_parameter("ggproc", -1,                              desc="(-1:Powheg default)")
        self.add_parameter("gqbproc", -1,                             desc="(-1:Powheg default)")
        self.add_parameter("largecorrfact", -1,                       desc="(-1:Powheg default)")
        self.add_parameter("psgen", -1,                               desc="(-1:Powheg default)")
        self.add_parameter("ptmin_jet", 25,                           desc="jet min pt in GeV for use in analysis code (not relevant here)")
        self.add_parameter("qgproc", -1,                              desc="(-1:Powheg default)")
        self.add_parameter("qqbproc", -1,                             desc="(-1:Powheg default)")
        self.add_parameter("qqpproc", -1,                             desc="(-1:Powheg default)")
        self.add_parameter("qqproc", -1,                              desc="(-1:Powheg default)")
        self.add_parameter("R_jet", 0.4,                              desc="jet radius for use in internal Powheg analysis code (not relevant here)")
        self.add_parameter("topwidth", ATLASCommonParameters.width_t, desc="top width")
        self.add_parameter("use_OLP_interface", -1,                   desc="(-1:Powheg default)", parameter="use-OLP-interface")

        # Decorate with generic option sets
        self.add_parameter_set("CKM")
        self.add_parameter_set("electroweak properties", boson="W")
        self.add_parameter_set("ISR/FSR")
        self.add_parameter_set("jacsing")
        self.add_parameter_set("lepton mass", prefix="lhfm")
        self.add_parameter_set("mass window")
        self.add_parameter_set("PDF reweighting")
        self.add_parameter_set("running scale")
        self.add_parameter_set("second generation quark mass", prefix="lhfm")
        self.add_parameter_set("semileptonic")
        self.add_parameter_set("sin**2 theta W")
        self.add_parameter_set("top decay branching")
        self.add_parameter_set("top decay mode")
        self.add_parameter_set("upper bound")
        self.add_parameter_set("zero width")

        # Set optimised integration parameters
        self.itmx1   = 15
        self.itmx2   = 20
        self.ncall1  = 20000000
        self.ncall2  = 50000000
        self.nubound = 50000000
        self.foldx   = 25
        self.foldy   = 25
        self.foldphi = 25

        # Override defaults
        self.bornktmin    = 5.0
        self.bornsuppfact = 100.0
        self.minlo        = -1
        self.quark_mass   = ATLASCommonParameters.mass_t
        self.topdecaymode = 22222

        self.populate_default_strings()
