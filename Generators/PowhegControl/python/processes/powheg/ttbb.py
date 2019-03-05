# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_RES import PowhegRES

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class ttbb(PowhegRES):
    """! Default Powheg configuration for top-antitop-bottom-antibottom production.

    Create a configurable object with all applicable Powheg options.

    @author Stefan Richter  <stefan.richter@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(ttbb, self).__init__(base_directory, "ttbb", **kwargs)

        # Add parameter validation functions
        # self.validation_functions.append("validate_decays")

        ## List of allowed decay modes
        # TODO: adapt!
        # self.allowed_decay_modes = ["t t~ > all", "t t~ > b j j b~ j j", "t t~ > b l+ vl b~ l- vl~", "t t~ > b emu+ vemu b~ emu- vemu~", "t t~ > semileptonic"]

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("allrad", 1)
        self.add_keyword("alpha")
        self.add_keyword("alphas_from_lhapdf", 1)
        self.add_keyword("bmass")
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
        self.add_keyword("chklimseed")
        self.add_keyword("clobberlhe")
        self.add_keyword("colltest")
        self.add_keyword("complexGFermi")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("doublefsr")
        self.add_keyword("enhancereg")
        self.add_keyword("evenmaxrat")
        self.add_keyword("ewscheme")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedscale")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 5)
        self.add_keyword("foldphi", 1)
        self.add_keyword("foldy", 5)
        self.add_keyword("for_reweighting", 1)
        self.add_keyword("fullrwgt")
        self.add_keyword("hdamp", 172.5)
        self.add_keyword("hfact")
        self.add_keyword("hmass")
        self.add_keyword("hwidth")
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 2)
        self.add_keyword("itmx1btl")
        self.add_keyword("itmx1btlbrn")
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 3)
        self.add_keyword("itmx2btl")
        self.add_keyword("itmx2btlbrn")
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax")
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("lhrwgt_descr")
        self.add_keyword("lhrwgt_group_combine")
        self.add_keyword("lhrwgt_group_name")
        self.add_keyword("lhrwgt_id")
        self.add_keyword("LOevents")
        self.add_keyword("manyseeds")
        self.add_keyword("max_io_bufsize")
        self.add_keyword("maxseeds")
        self.add_keyword("minlo")
        self.add_keyword("mint_density_map")
        self.add_keyword("mintupbratlim")
        self.add_keyword("MSbarscheme")
        self.add_keyword("ncall1", 40000)
        self.add_keyword("ncall1btl")
        self.add_keyword("ncall1btlbrn")
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 40000)
        self.add_keyword("ncall2btl")
        self.add_keyword("ncall2btlbrn")
        self.add_keyword("ncall2rm", 80000)
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("nores")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 10000)
        self.add_keyword("olpreset")
        self.add_keyword("olverbose")
        self.add_keyword("openloops-stability")
        self.add_keyword("openloopsreal")
        self.add_keyword("openloopsvirtual")
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
        self.add_keyword("regridfix")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("runningscales", 2)
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("smartsig")
        self.add_keyword("softmismch")
        self.add_keyword("softonly")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("tmass_phsp")
        self.add_keyword("tmass")
        self.add_keyword("twidth")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails", 0) # having this set to the global default '1' causes a crash during parallelstage 2!
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("user_reshists_sep")
        self.add_keyword("verytinypars")
        self.add_keyword("virtonly")
        self.add_keyword("whichpwhgevent")
        self.add_keyword("withbtilde")
        self.add_keyword("withdamp", 1)
        self.add_keyword("withnegweights")
        self.add_keyword("withremnants")
        self.add_keyword("withsubtr")
        self.add_keyword("wmass")
        self.add_keyword("wwidth")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 2)
        self.add_keyword("zerowidth")
        self.add_keyword("zmass")
        self.add_keyword("zwidth")

    # def validate_decays(self):
    #     """! Validate semileptonic and topdecaymode keywords."""
    #     self.expose()  # convenience call to simplify syntax
    #     if self.decay_mode not in self.allowed_decay_modes:
    #         logger.warning("Decay mode {} not recognised!".format(self.decay_mode))
    #         raise ValueError("Decay mode {} not recognised!".format(self.decay_mode))
    #     # Calculate appropriate decay mode numbers
    #     __decay_mode_lookup = {"t t~ > all": "22222", "t t~ > b j j b~ j j": "00022", "t t~ > b l+ vl b~ l- vl~": "22200", "t t~ > b emu+ vemu b~ emu- vemu~": "22000", "t t~ > semileptonic": "11111"}
    #     self.parameters_by_keyword("topdecaymode")[0].value = __decay_mode_lookup[self.decay_mode]
    #     if self.decay_mode == "semileptonic":
    #         self.parameters_by_keyword("semileptonic")[0].value = 1
