# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import os
from AthenaCommon import Logging
from .external_base import ExternalBase

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class ExternalMadSpin(ExternalBase):
    """! Class for running external MadSpin process.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, process):
        """! Constructor.

        @param process  MadSpin process description string.
        """
        super(ExternalMadSpin, self).__init__("MadSpin", os.environ["MADPATH"], "MadSpin", "madspin")

        # Add parameters used by MadSpin
        self.add_keyword("alphaem_inv")
        self.add_keyword("alphaqcd")
        self.add_keyword("BR_t_to_Wb")
        self.add_keyword("BR_t_to_Wd")
        self.add_keyword("BR_t_to_Ws")
        self.add_keyword("BR_W_to_hadrons")
        self.add_keyword("BR_W_to_leptons")
        self.add_keyword("bwcutoff")
        self.add_keyword("ebeam1")
        self.add_keyword("G_F")
        self.add_keyword("MadSpin_decays")
        self.add_keyword("MadSpin_enabled")
        self.add_keyword("MadSpin_model")
        self.add_keyword("MadSpin_mode")
        self.add_keyword("MadSpin_nFlavours")
        self.add_keyword("MadSpin_process", process)
        self.add_keyword("MadSpin_taus_are_leptons")
        self.add_keyword("mass_b")
        self.add_keyword("mass_H")
        self.add_keyword("mass_t")
        self.add_keyword("mass_tau")
        self.add_keyword("mass_W")
        self.add_keyword("mass_Z")
        self.add_keyword("powheg_top_decays_enabled")
        self.add_keyword("width_H")
        self.add_keyword("width_t")
        self.add_keyword("width_W")
        self.add_keyword("width_Z")

    def needs_scheduling(self, process):
        """! Report whether the MadSpin process should be scheduled.

        @param process PowhegBox process.
        """
        # Check that MadSpin is not disabled
        self.expose()
        if not self.MadSpin_enabled:
            logger.warning("MadSpin not enabled")
            return False
        # Check that tops have not already been decayed
        if self.powheg_top_decays_enabled:
            logger.info("Powheg top quark decays are enabled. MadSpin will not be used for decays.")
            return False
        # Check that decays are in correct format
        if not isinstance(self.MadSpin_decays, list):
            logger.warning("Could not interpret 'MadSpin_decays' argument as a list of allowed decays!")
            return False
        # Check that decay list has at least one entry otherwise add defaults
        if len(self.MadSpin_decays) == 0:
            logger.warning("No MadSpin decays specified, so defaults will be used.")
            for decay in ("t > w+ b, w+ > l+ vl", "t~ > w- b~, w- > l- vl~", "t > w+ b, w+ > j j", "t~ > w- b~, w- > j j"):
                logger.info("... adding MadSpin decay: 'decay {0}'".format(decay))
                list(self.parameters_by_name("MadSpin_decays"))[0].value.append("decay {0}".format(decay))
        return True
