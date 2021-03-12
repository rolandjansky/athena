# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from .external_base import ExternalBase

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class ExternalVBFNLO(ExternalBase):
    """! Class for running external VBFNLO process.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, vector_boson, allowed_decay_modes):
        """! Constructor.

        @param process  VBFNLO process
        """
        super(ExternalVBFNLO, self).__init__("VBFNLO")

        ## List of allowed decay modes
        self.allowed_decay_modes = allowed_decay_modes

        # Add parameters used by VBFNLO
        self.add_keyword("alphaem_inv")
        self.add_keyword("vdecaymode", list(self.allowed_decay_modes)[0], name="decay_mode", description="{} boson decay mode. [{}]".format(vector_boson, allowed_decay_modes))
        self.add_keyword("G_F")
        self.add_keyword("mass_b")
        self.add_keyword("mass_c")
        self.add_keyword("mass_H")
        self.add_keyword("mass_t")
        self.add_keyword("mass_tau")
        self.add_keyword("mass_W")
        self.add_keyword("mass_Z")
        self.add_keyword("sthw2")
        self.add_keyword("width_H")

    def needs_scheduling(self, process):
        """! Report whether the VBFNLO process should be scheduled.

        @param process PowhegBox process.
        """
        self.expose()  # convenience call to simplify syntax
        if self.decay_mode not in self.allowed_decay_modes:
            logger.fatal("Decay mode {} not recognised!".format(self.decay_mode))
            raise ValueError("VBF NLO cannot be correctly configured.")
        # Convert allowed decay mode into PROC_ID/DECAYMODE
        __vector_boson_type = self.decay_mode.split(">")[0].strip()
        __vector_boson_decay = self.decay_mode.split(" ")[2].replace("+", "").replace("-", "")
        # Add runcard entries
        VBF_runcard_entries = [("PROC_ID", {"z": 120, "w+": 130, "w-": 140}[__vector_boson_type], "{} boson".format(__vector_boson_type)),
                               ("DECAYMODE", {"e": 11, "mu": 13}[__vector_boson_decay], self.decay_mode),
                               ("HMASS", self.mass_H, self.parameters_by_name("mass_H")[0].description),
                               ("HWIDTH", self.width_H, self.parameters_by_name("width_H")[0].description),
                               ("EWSCHEME", 4, "Explicitly specify all EW parameters"),
                               ("ANOM_CPL", 0, "Disable anomalous couplings as these are not supported by the authors"),
                               ("TOPMASS", self.mass_t, self.parameters_by_name("mass_t")[0].description),
                               ("TAU_MASS", self.mass_tau, self.parameters_by_name("mass_tau")[0].description),
                               ("BOTTOMMASS", self.mass_b, self.parameters_by_name("mass_b")[0].description),
                               ("CHARMMASS", self.mass_c, self.parameters_by_name("mass_c")[0].description),
                               ("FERMI_CONST", self.G_F, self.parameters_by_name("G_F")[0].description),
                               ("INVALFA", self.alphaem_inv, self.parameters_by_name("alphaem_inv")[0].description),
                               ("SIN2W", self.sthw2, self.parameters_by_name("sthw2")[0].description),
                               ("WMASS", self.mass_W, self.parameters_by_name("mass_W")[0].description),
                               ("ZMASS", self.mass_Z, self.parameters_by_name("mass_Z")[0].description)]
        # Write special VBF runcard
        with open("vbfnlo.input", "w") as f:
            for runcard_entry in VBF_runcard_entries:
                f.write("{:<15} {:<15} ! {}\n".format(*runcard_entry))
        return True
