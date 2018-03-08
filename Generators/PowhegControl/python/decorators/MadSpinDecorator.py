# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl MadSpinDecorator
#  Powheg runcard decorator for all processes which can run with a MadSpin afterburner
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
import os
from .. import ATLASCommonParameters
from AthenaCommon import Logging

logger = Logging.logging.getLogger("PowhegControl")


class MadSpinDecorator(object):

    # Define decorator name string
    name = "MadSpin"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated
        self.decorated._MadSpin_executable = os.environ["MADPATH"] + "/MadSpin/madspin"
        self.decorated.MadSpin_process = ""
        self.decorated.MadSpin_decays = []

        # Add phantom parameters used by the MadSpin runcard
        self.decorated.add_phantom("alphaqcd", ATLASCommonParameters.alphaqcd,                desc="Strong coupling")
        self.decorated.add_phantom("mass_H", ATLASCommonParameters.mass_H,                    desc="Higgs mass")
        self.decorated.add_phantom("mass_Z", ATLASCommonParameters.mass_Z,                    desc="Z mass")
        self.decorated.add_phantom("G_F", ATLASCommonParameters.G_F,                          desc="Fermi coupling")
        self.decorated.add_phantom("use_MadSpin", 1,                                          desc="(0:disabled; 1:enabled) use MadSpin for top decays (only if decays are disabled in Powheg)")
        self.decorated.add_phantom("MadSpin_mode", "madspin",                                 desc="(madspin; none) which spin mode to use in MadSpin")
        self.decorated.add_phantom("t_Wb_BR", 1.0,                                            desc="top branching ratio to b-quarks")
        self.decorated.add_phantom("t_Ws_BR", 0.0,                                            desc="top branching ratio to s-quarks")
        self.decorated.add_phantom("t_Wd_BR", 0.0,                                            desc="top branching ratio to d-quarks")
        self.decorated.add_phantom("W_hadron_BR", ATLASCommonParameters.W_hadron_BR,          desc="W branching ratio to hadrons")
        self.decorated.add_phantom("W_lepton_BR", ATLASCommonParameters.W_lepton_BR,          desc="W branching ratio to leptons")
        self.decorated.add_phantom("width_H", ATLASCommonParameters.width_H,                  desc="Higgs width")
        self.decorated.add_phantom("width_Z", ATLASCommonParameters.width_Z,                  desc="Z width")

    # Check that MadSpin_decays is valid
    def finalise(self):
        if not isinstance(self.decorated.MadSpin_decays, list):
            logger.fatal("Could not interpret 'MadSpin_decays' argument as a list of allowed decays!")
            raise ValueError("Could not interpret 'MadSpin_decays' argument as a list of allowed decays!")
        if len(self.decorated.MadSpin_decays) == 0:
            logger.info("No MadSpin decays specified, so defaults will be used (only relevant if MadSpin is enabled)")
            for decay in ("t > w+ b, w+ > l+ vl", "t~ > w- b~, w- > l- vl~", "t > w+ b, w+ > j j", "t~ > w- b~, w- > j j"):
                logger.info("... adding MadSpin decay: 'decay {0}'".format(decay))
                self.decorated.MadSpin_decays.append("decay {0}".format(decay))
