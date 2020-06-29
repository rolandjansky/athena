# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2
import glob
import os
import shutil

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class chi0chi1(PowhegV2):
    """! Default Powheg configuration for neutralino plus chargino production.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory  path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(chi0chi1, self).__init__(base_directory, os.path.join("weakinos", "neuIchaJ"), powheg_executable="pwhg_main_nixj", **kwargs)
        if not os.path.isfile("input_mSUGRA.slha"):
            shutil.copy(os.path.join(os.path.split(self.executable)[0], "testrun", "input_mSUGRA.slha"), ".")
            logger.info("Locating default mSUGRA input distributed with POWHEG...")

        # Load SLHA input
        if len(glob.glob("*.slha")) > 0:
            input_slha = glob.glob("*.slha")[0]
            logger.info("Using input SLHA file: {}".format(input_slha))
            shutil.move(input_slha, "input_parameters.slha")
        else:
            logger.info("Locating input SLHA distributed with POWHEG...")
            shutil.copy(os.path.join(os.path.split(self.executable)[0], "testrun", "input_mSUGRA.slha"), "input_parameters.slha")

        # Add parameter validation functions
        self.validation_functions.append("validate_weakinos")

        ## List of allowed weakinos
        self.allowed_weakinos = {"chi10": 1000022, "chi20": 1000023, "chi30": 1000025, "chi40": 1000035, "chi1+": 1000024, "chi1-": -1000024, "chi2+": 1000037, "chi2-": -1000037}

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
        self.add_keyword("doublefsr")
        self.add_keyword("evenmaxrat")
        self.add_keyword("ewi")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fakevirtuals")
        self.add_keyword("fastbtlbound")
        self.add_keyword("fin1", "chi10", name="neutralino")
        self.add_keyword("fin2", "chi1+", name="chargino")
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
        self.add_keyword("incomingflavors")
        self.add_keyword("inv_alpha", -1)
        self.add_keyword("itmx1", 4)
        self.add_keyword("itmx1osres")
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 5)
        self.add_keyword("itmx2osres")
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
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("ncall1", 1000)
        self.add_keyword("ncall1osres")
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 10000)
        self.add_keyword("ncall2osres")
        self.add_keyword("ncall2rm")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 10000)
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
        self.add_keyword("runningscale")
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("skipextratests")
        self.add_keyword("SLHA")
        self.add_keyword("smartsig")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("Wmass", -1)
        self.add_keyword("Wwidth", -1)
        self.add_keyword("xupbound")
        self.add_keyword("Zmass", -1)
        self.add_keyword("Zwidth", -1)

    def validate_weakinos(self):
        """! Validate weakinos."""
        self.expose()  # convenience call to simplify syntax
        # Neutralino
        self.check_decay_mode(self.parameters_by_name("neutralino")[0].value, [x for x in self.allowed_weakinos.keys() if x[-1] == "0"])
        self.parameters_by_name("neutralino")[0].value = self.allowed_weakinos[self.parameters_by_name("neutralino")[0].value]
        # Chargino
        self.check_decay_mode(self.parameters_by_name("chargino")[0].value, [x for x in self.allowed_weakinos.keys() if x[-1] != "0"])
        self.parameters_by_name("chargino")[0].value = self.allowed_weakinos[self.parameters_by_name("chargino")[0].value]
