# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class WZ(PowhegV2):
    """! Default Powheg configuration for W-boson and Z-boson production including interference for identical leptons.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(WZ, self).__init__(base_directory, "WZ", **kwargs)

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        ## List of allowed decay modes
        self.allowed_decay_modes = ["w z > e ve e+ e-", "w z > e ve mu+ mu-", "w z > e ve tau+ tau-", "w z > j j j j",
                                    "w z > j j l+ l-", "w z > j j v v~", "w z > l v j j", "w z > l v l+ l-",
                                    "w z > l v v v~", "w z > mu vm e+ e-", "w z > mu vm mu+ mu-",
                                    "w z > mu vm tau+ tau-", "w z > tau vt e+ e-", "w z > tau vt mu+ mu-",
                                    "w z > tau vt tau+ tau-", "w- z > e- ve~ e+ e-", "w- z > e- ve~ mu+ mu-",
                                    "w- z > e- ve~ tau+ tau-", "w- z > mu- vm~ e+ e-", "w- z > mu- vm~ mu+ mu-",
                                    "w- z > mu- vm~ tau+ tau-", "w- z > tau- vt~ e+ e-", "w- z > tau- vt~ mu+ mu-",
                                    "w- z > tau- vt~ tau+ tau-", "w+ z > e+ ve e+ e-", "w+ z > e+ ve mu+ mu-",
                                    "w+ z > e+ ve tau+ tau-", "w+ z > mu+ vm e+ e-", "w+ z > mu+ vm mu+ mu-",
                                    "w+ z > mu+ vm tau+ tau-", "w+ z > tau+ vt e+ e-", "w+ z > tau+ vt mu+ mu-",
                                    "w+ z > tau+ vt tau+ tau-"]

        # Add all keywords for this process, overriding defaults if required
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
        self.add_keyword("clobberlhe")
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("delg1_g")
        self.add_keyword("delg1_z")
        self.add_keyword("delk_g")
        self.add_keyword("delk_z")
        self.add_keyword("diagCKM")
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
        self.add_keyword("mllmin", 20.0)
        self.add_keyword("ncall1", 400000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 1000000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 1000000)
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
        self.add_keyword("withdamp", 1)
        self.add_keyword("withinterference")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("WmZevee", self.allowed_decay_modes[0], name="decay_mode", hidden=False)
        self.add_keyword("WmZevmumu")
        self.add_keyword("WmZevtautau")
        self.add_keyword("WmZmuvee")
        self.add_keyword("WmZmuvmumu")
        self.add_keyword("WmZmuvtautau")
        self.add_keyword("WmZtauvee")
        self.add_keyword("WmZtauvmumu")
        self.add_keyword("WmZtauvtautau")
        self.add_keyword("WpZevee")
        self.add_keyword("WpZevmumu")
        self.add_keyword("WpZevtautau")
        self.add_keyword("WpZmuvee")
        self.add_keyword("WpZmuvmumu")
        self.add_keyword("WpZmuvtautau")
        self.add_keyword("WpZtauvee")
        self.add_keyword("WpZtauvmumu")
        self.add_keyword("WpZtauvtautau")
        self.add_keyword("WZevee")
        self.add_keyword("WZevmumu")
        self.add_keyword("WZevtautau")
        self.add_keyword("WZlvll")
        self.add_keyword("WZlvqq")
        self.add_keyword("WZlvvv")
        self.add_keyword("WZmuvee")
        self.add_keyword("WZmuvmumu")
        self.add_keyword("WZmuvtautau")
        self.add_keyword("WZqqll")
        self.add_keyword("WZqqqq")
        self.add_keyword("WZqqvv")
        self.add_keyword("WZtauvee")
        self.add_keyword("WZtauvmumu")
        self.add_keyword("WZtauvtautau")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 10)
        self.add_keyword("zerowidth")

    def validate_decays(self):
        """! Validate the various decay mode keywords."""
        # See https://docs.google.com/spreadsheets/d/1Aa7FwB74ppHbXles5LyHrKGlvUFi5PxbZC-Mrc3Lz90 for meanings
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode, self.allowed_decay_modes)
        # Enable appropriate decay mode
        list(self.parameters_by_keyword("WmZevee"))[0].value = 0  # disable the one used as a proxy for decay mode
        __decay_mode_lookup = {"w z > e ve e+ e-": "WZevee",
                               "w z > e ve mu+ mu-": "WZevmumu",
                               "w z > e ve tau+ tau-": "WZevtautau",
                               "w z > j j j j": "WZqqqq",
                               "w z > j j l+ l-": "WZqqll",
                               "w z > j j v v~": "WZqqvv",
                               "w z > l v j j": "WZlvqq",
                               "w z > l v l+ l-": "WZlvll",
                               "w z > l v v v~": "WZlvvv",
                               "w z > mu vm e+ e-": "WZmuvee",
                               "w z > mu vm mu+ mu-": "WZmuvmumu",
                               "w z > mu vm tau+ tau-": "WZmuvtautau",
                               "w z > tau vt e+ e-": "WZtauvee",
                               "w z > tau vt mu+ mu-": "WZtauvmumu",
                               "w z > tau vt tau+ tau-": "WZtauvtautau",
                               "w- z > e- ve~ e+ e-": "WmZevee",
                               "w- z > e- ve~ mu+ mu-": "WmZevmumu",
                               "w- z > e- ve~ tau+ tau-": "WmZevtautau",
                               "w- z > mu- vm~ e+ e-": "WmZmuvee",
                               "w- z > mu- vm~ mu+ mu-": "WmZmuvmumu",
                               "w- z > mu- vm~ tau+ tau-": "WmZmuvtautau",
                               "w- z > tau- vt~ e+ e-": "WmZtauvee",
                               "w- z > tau- vt~ mu+ mu-": "WmZtauvmumu",
                               "w- z > tau- vt~ tau+ tau-": "WmZtauvtautau",
                               "w+ z > e+ ve e+ e-": "WpZevee",
                               "w+ z > e+ ve mu+ mu-": "WpZevmumu",
                               "w+ z > e+ ve tau+ tau-": "WpZevtautau",
                               "w+ z > mu+ vm e+ e-": "WpZmuvee",
                               "w+ z > mu+ vm mu+ mu-": "WpZmuvmumu",
                               "w+ z > mu+ vm tau+ tau-": "WpZmuvtautau",
                               "w+ z > tau+ vt e+ e-": "WpZtauvee",
                               "w+ z > tau+ vt mu+ mu-": "WpZtauvmumu",
                               "w+ z > tau+ vt tau+ tau-": "WpZtauvtautau"}
        list(self.parameters_by_keyword(__decay_mode_lookup[self.decay_mode]))[0].value = 1
