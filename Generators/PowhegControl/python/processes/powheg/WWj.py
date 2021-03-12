# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class WWj(PowhegV2):
    """! Default Powheg configuration for W-boson pair production plus one jet.

    Create a configurable object with all applicable Powheg options.

    @author Timoth\'ee Theveneaux-Pelzer  <tpelzer@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(WWj, self).__init__(base_directory, "WWJ", **kwargs)

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        ## List of allowed decay modes
        self.allowed_decay_modes = ["w+ w- > e+ ve e- ve~",
                                    "w+ w- > mu+ vm mu- vm~",
                                    "w+ w- > tau+ vt tau- vt~",
                                    "w+ w- > e+ ve mu- vm~ / mu+ vm e- ve~",
                                    "w+ w- > l+ vl l'- vl'~",
                                    "w+ w- > l+ vl j j / j j l- vl~",
                                    "w+ w- > e+ ve j j / j j e- ve~ / mu+ vmu j j / j j mu- vmu~",
                                    "w+ w- > j j j j",
                                    "w+ w- > e+ ve mu- vm~",
                                    "w+ w- > mu+ vm e- ve~"]

        # Add all keywords for this process, overriding defaults if required
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
        self.add_keyword("clobberlhe")
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("delg1_g")
        self.add_keyword("delg1_z")
        self.add_keyword("delk_g")
        self.add_keyword("delk_z")
        self.add_keyword("doublefsr")
        self.add_keyword("dronly")
        self.add_keyword("evenmaxrat")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("fixedscale")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi")
        self.add_keyword("foldphi")
        self.add_keyword("foldy")
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 4)
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 5)
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax")
        self.add_keyword("lambda_g")
        self.add_keyword("lambda_z")
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("lhapdf6maxsets")
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
        self.add_keyword("ncall1", 40000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 80000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 30000)
        self.add_keyword("nnlops")
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
        self.add_keyword("runningscales") # 0 = fixed scale 2m(W), 1=m(WW), 2=mT(W+) + mT(W-)
        self.add_keyword("runningwidth")
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("skipextratests")
        self.add_keyword("smartsig")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("tevscale")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("e+e-")
        self.add_keyword("mu+mu-")
        self.add_keyword("tau+tau-")
        self.add_keyword("leptonic_notau")
        self.add_keyword("leptonic")
        self.add_keyword("hadronic")
        self.add_keyword("semileptonic", "w+ w- > l+ vl l'- vl'~", name="decay_mode", hidden=False)
        self.add_keyword("semileptonic_notau")
        self.add_keyword("e+mu-")
        self.add_keyword("mu+e-")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound")
        self.add_keyword("zerowidth")

    def validate_decays(self):
        """! Validate the various decay mode keywords."""
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode, self.allowed_decay_modes)
        # Enable appropriate decay mode
        list(self.parameters_by_keyword("semileptonic"))[0].value = 0 # disable the one used as a proxy for decay mode
        __decay_mode_lookup = { "w+ w- > e+ ve e- ve~": "e+e-",
                                "w+ w- > mu+ vm mu- vm~": "mu+mu-",
                                "w+ w- > tau+ vt tau- vt~": "tau+tau-",
                                "w+ w- > e+ ve mu- vm~ / mu+ vm e- ve~": "leptonic_notau",
                                "w+ w- > l+ vl l'- vl'~": "leptonic",
                                "w+ w- > l+ vl j j / j j l- vl~": "hadronic",
                                "w+ w- > e+ ve j j / j j e- ve~ / mu+ vmu j j / j j mu- vmu~": "semileptonic",
                                "w+ w- > j j j j": "semileptonic_notau",
                                "w+ w- > e+ ve mu- vm~": "e+mu-",
                                "w+ w- > mu+ vm e- ve~": "mu+e-"}
        
        list(self.parameters_by_keyword(__decay_mode_lookup[self.decay_mode]))[0].value = 1
