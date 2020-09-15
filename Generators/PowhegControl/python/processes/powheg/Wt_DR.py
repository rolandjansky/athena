# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class Wt_DR(PowhegV2):
    """! Default Powheg configuration for single top production with an associated W-boson through the t-channel (diagram removal).

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(Wt_DR, self).__init__(base_directory, "ST_wtch_DR", **kwargs)

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        ## List of allowed top decay modes
        self.allowed_top_decay_modes = ["t > all", "t > b j j", "t > b l+ vl", "t > b emu+ vemu", "t > b e+ ve",
                                        "t > b mu+ vm", "t > b tau+ vt", "t~ > all", "t~ > b~ j j", "t~ > b~ l- vl~",
                                        "t~ > b~ emu- vemu~", "t~ > b~ e- ve~", "t~ > b~ mu- vm~", "t~ > b~ tau- vt~"]

        ## List of allowed W decay modes
        self.allowed_W_decay_modes = ["w > all", "w > j j", "w > l vl", "w > emu vemu", "w > e ve", "w > mu vm", "w > tau vt"]

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("alphaem_inv")
        self.add_keyword("alphas_from_lhapdf")
        self.add_keyword("bornktmin")
        self.add_keyword("bornonly")
        self.add_keyword("bornsuppfact")
        self.add_keyword("bornzerodamp")
        self.add_keyword("bottomthr")
        self.add_keyword("bottomthrpdf")
        self.add_keyword("btildeborn")
        self.add_keyword("btildecoll")
        self.add_keyword("btildereal")
        self.add_keyword("btildevirt")
        self.add_keyword("btlscalect")
        self.add_keyword("btlscalereal")
        self.add_keyword("charmthr")
        self.add_keyword("charmthrpdf")
        self.add_keyword("check_bad_st2")
        self.add_keyword("CKM_Vcb")
        self.add_keyword("CKM_Vcd")
        self.add_keyword("CKM_Vcs")
        self.add_keyword("CKM_Vtb")
        self.add_keyword("CKM_Vtd")
        self.add_keyword("CKM_Vts")
        self.add_keyword("CKM_Vub")
        self.add_keyword("CKM_Vud")
        self.add_keyword("CKM_Vus")
        self.add_keyword("clobberlhe")
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("doublefsr")
        self.add_keyword("evenmaxrat")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi")
        self.add_keyword("foldphi")
        self.add_keyword("foldy")
        self.add_keyword("fullrwgt")
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 5)
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 6)
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax")
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("lhfm/bmass")
        self.add_keyword("lhfm/cmass")
        self.add_keyword("lhfm/emass")
        self.add_keyword("lhfm/mumass")
        self.add_keyword("lhfm/taumass")
        self.add_keyword("lhrwgt_descr")
        self.add_keyword("lhrwgt_group_combine")
        self.add_keyword("lhrwgt_group_name")
        self.add_keyword("lhrwgt_id")
        self.add_keyword("LOevents")
        self.add_keyword("manyseeds")
        self.add_keyword("max_io_bufsize")
        self.add_keyword("maxseeds")
        self.add_keyword("minlo")
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("ncall1", 50000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 150000)
        self.add_keyword("ncall2rm")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 150000)
        self.add_keyword("olddij")
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("parallelstage")
        self.add_keyword("pdfreweight")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("radregion")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("skipextratests")
        self.add_keyword("smartsig")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("sthw2")
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("tdec/elbranching")
        self.add_keyword("tdec/emass")
        self.add_keyword("tdec/mumass")
        self.add_keyword("tdec/taumass")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("topdecaymode", self.allowed_top_decay_modes[0], name="decay_mode_top")
        self.add_keyword("topmass")
        self.add_keyword("topwidth")
        self.add_keyword("ttype", hidden=True)
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("wdecaymode", self.allowed_W_decay_modes[0], name="decay_mode_W")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("wmass")
        self.add_keyword("wwidth")
        self.add_keyword("xupbound", 4)
        self.add_keyword("fulloffshell")
        self.add_keyword("numwidth_W")
        self.add_keyword("min_W_mass")

    def validate_decays(self):
        """! Validate ttype, topdecaymode and wdecaymode keywords."""
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode_top, self.allowed_top_decay_modes)
        self.check_decay_mode(self.decay_mode_W, self.allowed_W_decay_modes)
        # Calculate appropriate decay mode numbers
        __top_decay = self.decay_mode_top.replace("~", "").replace("+", "").replace("-", "").replace("b", "").split("t > ")[1].strip()
        __W_decay = self.decay_mode_W.split("w > ")[1].strip()
        __decay_mode_lookup = {"all": "11111", "j j": "00011", "l vl": "11100", "emu vemu": "11000", "e ve": "10000", "mu vm": "01000", "tau vt": "00100"}
        self.parameters_by_keyword("ttype")[0].value = [+1, -1][self.decay_mode_top.startswith("t~")]
        self.parameters_by_keyword("topdecaymode")[0].value = __decay_mode_lookup[__top_decay]
        self.parameters_by_keyword("wdecaymode")[0].value = __decay_mode_lookup[__W_decay]
