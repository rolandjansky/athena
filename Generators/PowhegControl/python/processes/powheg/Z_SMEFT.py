# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2
import itertools

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class Z_SMEFT(PowhegV2):
    """!
    Powheg configuration for Z boson production in Standard Model Effective Field Theory

    This process is an implementation of the Drell-Yan NLO cross sections pp -> Z/y* -> l+l- by SMEFT operators

    It includes 196 SMEFT coefficients that the user can set. For symmetry, the authors included coefficients
    for operators involving top-quarks. These should not be used as they are only relevant when initial-state
    tops are considered, which is never the case.

    Publication (please cite if using this process): https://arxiv.org/abs/1804.07407
    Brief manual (highly recommended reading if using this process): accessible on LXPlus etc. at $POWHEGPATH/POWHEG-BOX-V2/Z_smeft/Docs/manual-BOX-Z_smeft.pdf

    @author Stefan Richter <stefan.richter@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """!
        Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(Z_SMEFT, self).__init__(base_directory, "Z_smeft", **kwargs)

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
        # clobberlhe: needed to make reweighting work --- otherwise Powheg crashes because it
        # cannot overwrite the LHE file with events before reweighting.
        # The value given to the parameter here doesn't matter, it just needs to be present in
        # the powheg.input file. The reweighter postprocessor will replace the value with 1
        # anyway.
        # self.add_keyword("clobberlhe")
        self.add_keyword("cmass_lhe")
        self.add_keyword("colltest")
        self.add_keyword("compute_rwgt")
        self.add_keyword("dim6", 1)
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fcnc", 0)
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 5)
        self.add_keyword("foldphi", 2)
        self.add_keyword("foldy", 2)
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
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
        self.add_keyword("LambdaNP", 1000.0)
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("lhrwgt_descr")
        self.add_keyword("lhrwgt_group_combine")
        self.add_keyword("lhrwgt_group_name")
        self.add_keyword("lhrwgt_id")
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
        self.add_keyword("runningscale", 1) # if 1, central scale equal to Z/gamma virtuality. If 0, central scale equal to M_Z
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("smartsig")
        self.add_keyword("softtest")
        self.add_keyword("sthw2")
        self.add_keyword("storeinfo_rwgt")
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
        # SMEFT operator coefficients
        for coefficient_name in ["ReGEw", "ImGEw", "ReGEe", "ImGEe"]:
            self.add_keyword(coefficient_name)
        for coefficient in ["ReGUw_{flavours}", "ImGUw_{flavours}", "ReGUe_{flavours}", "ImGUe_{flavours}", "ReLeQu3_{flavours}", "ImLeQu3_{flavours}", "ReLeQu_{flavours}", "ImLeQu_{flavours}"]:
            for flavours in ["{}{}".format(*x) for x in itertools.product(["u", "c", "t"], repeat=2)]:
                coefficient_name = coefficient.format(flavours=flavours)
                self.add_keyword(coefficient_name)
        for coefficient in ["ReGDw_{flavours}", "ImGDw_{flavours}", "ReGDe_{flavours}", "ImGDe_{flavours}", "ReLedQ_{flavours}", "ImLedQ_{flavours}"]:
            for flavours in ["{}{}".format(*x) for x in itertools.product(["d", "s", "b"], repeat=2)]:
                coefficient_name = coefficient.format(flavours=flavours)
                self.add_keyword(coefficient_name)
        for coefficient in ["QphiU_{flavours}", "Uphi_{flavours}", "QLu_{flavours}", "Ceu_{flavours}", "CLu_{flavours}"]:
            for flavours in ["uu", "uc", "ut", "cc", "ct", "tt"]:
                coefficient_name = coefficient.format(flavours=flavours)
                self.add_keyword(coefficient_name)
        for coefficient in ["QphiD_{flavours}", "Dphi_{flavours}", "QLd_{flavours}", "Ced_{flavours}", "CLd_{flavours}", "Qe_{flavours}"]:
            for flavours in ["dd", "ds", "db", "ss", "sb", "bb"]:
                coefficient_name = coefficient.format(flavours=flavours)
                self.add_keyword(coefficient_name)



    def validate_decays(self):
        """! Validate vdecaymode keyword."""
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode, self.allowed_decay_modes)
        # Calculate appropriate decay mode numbers
        __decay_mode_lookup = {"e+ e-": 1, "mu+ mu-": 2, "tau+ tau-": 3, "ve ve~": 4, "vm vm~": 5, "vt vt~": 6}
        list(self.parameters_by_keyword("vdecaymode"))[0].value = __decay_mode_lookup[self.decay_mode.split("> ")[1]]
