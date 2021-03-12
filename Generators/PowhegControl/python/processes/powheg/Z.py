# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V1 import PowhegV1

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class Z(PowhegV1):
    """! Default Powheg configuration for single Z-boson production.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(Z, self).__init__(base_directory, "Z", **kwargs)

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        ## List of allowed decay modes
        self.allowed_decay_modes = ["z > e+ e-", "z > mu+ mu-", "z > tau+ tau-", "z > ve ve~", "z > vm vm~", "z > vt vt~"]

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("alphaem")
        self.add_keyword("bmass_lhe")
        self.add_keyword("bornktmin")
        self.add_keyword("bornonly")
        self.add_keyword("bornsuppfact")
        self.add_keyword("bornzerodamp")
        self.add_keyword("bottomthr")
        self.add_keyword("bottomthrpdf")
        self.add_keyword("btlscalect")
        self.add_keyword("btlscalereal")
        self.add_keyword("charmthr")
        self.add_keyword("charmthrpdf")
        self.add_keyword("cmass_lhe")
        self.add_keyword("colltest")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 5)
        self.add_keyword("foldphi", 2)
        self.add_keyword("foldy", 2)
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 5)
        self.add_keyword("itmx2", 2)
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax")
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("manyseeds")
        self.add_keyword("mass_high", 2.0 * list(self.parameters_by_name("beam_energy"))[0].value)
        self.add_keyword("mass_low", 60.0)
        self.add_keyword("masswindow_high")
        self.add_keyword("masswindow_low")
        self.add_keyword("ncall1", 40000)
        self.add_keyword("ncall2", 70000)
        self.add_keyword("nubound", 10000)
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("pdfreweight")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("radregion")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("running_width")
        self.add_keyword("runningscale")
        self.add_keyword("smartsig")
        self.add_keyword("softtest")
        self.add_keyword("sthw2")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("vdecaymode", self.allowed_decay_modes[0], name="decay_mode")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("Wmass")
        self.add_keyword("Wwidth")
        self.add_keyword("xupbound")
        self.add_keyword("Zmass")
        self.add_keyword("Zwidth")

    def validate_decays(self):
        """! Validate vdecaymode keyword."""
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode, self.allowed_decay_modes)
        # Calculate appropriate decay mode numbers
        __decay_mode_lookup = {"e+ e-": 1, "mu+ mu-": 2, "tau+ tau-": 3, "ve ve~": 4, "vm vm~": 5, "vt vt~": 6}
        list(self.parameters_by_keyword("vdecaymode"))[0].value = __decay_mode_lookup[self.decay_mode.split("> ")[1]]
