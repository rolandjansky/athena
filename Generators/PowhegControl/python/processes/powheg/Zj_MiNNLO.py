# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2
import os

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class Zj_MiNNLO(PowhegV2):
    """! Default Powheg configuration for single Z-boson production plus one jet using MiNNLOPS.

    Create a configurable object with all applicable Powheg options.

    @author Simone Amoroso  <simone.amoroso@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(Zj_MiNNLO, self).__init__(base_directory, os.path.join("Zj", "ZjMiNNLO"), powheg_executable="pwhg_main", **kwargs)

        # Add algorithms to the sequence

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        ## List of allowed decay modes
        self.allowed_decay_modes = ["z > e+ e-", "z > mu+ mu-", "z > tau+ tau-", "z > b b~"]

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("alphas_from_lhapdf",0.0)
        self.add_keyword("alphas_from_pdf",1.0)
        self.add_keyword("alphas_cutoff_fact")
        self.add_keyword("alphaem")
        self.add_keyword("bornktmin", 5.0)
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
        self.add_keyword("clobberlhe")
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("distribute_by_ub")
        self.add_keyword("distribute_by_ub_AP")
        self.add_keyword("doublefsr")
        self.add_keyword("evenmaxrat")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("factsc2min")
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 5)
        self.add_keyword("foldphi", 5)
        self.add_keyword("foldy", 5)
        self.add_keyword("frensc2min")
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("inc_delta_terms")
        self.add_keyword("itmx1", 5)
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 5)
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax")
        self.add_keyword("largeptscales", 0)
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("lhapdf6maxsets")
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
        self.add_keyword("max_Z_mass", 2.0 * self.parameters_by_name("beam_energy")[0].value)
        self.add_keyword("maxseeds")
        self.add_keyword("min_Z_mass", 60.0)
        self.add_keyword("minlo_nnll")
        self.add_keyword("minlo", 1)
        self.add_keyword("minnlo", 1)
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("modlog_p")
        self.add_keyword("ncall1", 20000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 70000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 10000)
        self.add_keyword("olddij")
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("par_fsrtinycsi")
        self.add_keyword("par_fsrtinyy")
        self.add_keyword("par_isrtinycsi")
        self.add_keyword("par_isrtinyy")
        self.add_keyword("parallelstage")
        self.add_keyword("pdfreweight")
        self.add_keyword("pdf_cutoff_fact")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("Q0",2)
        self.add_keyword("radregion")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("runningscales")
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
        self.add_keyword("sudscalevar")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("vdecaymode", self.allowed_decay_modes[0], name="decay_mode")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("Wmass")
        self.add_keyword("Wwidth")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 20)
        self.add_keyword("ZbbNLO")
        self.add_keyword("Zmass")
        self.add_keyword("Zwidth")

    def validate_decays(self):
        """! Validate vdecaymode keyword."""
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode, self.allowed_decay_modes)
        # Calculate appropriate decay mode numbers
        __decay_mode_lookup = {"b b~": 5, "e+ e-": 1, "mu+ mu-": 2, "tau+ tau-": 3}
        self.parameters_by_keyword("vdecaymode")[0].value = __decay_mode_lookup[self.decay_mode.split("> ")[1]]
