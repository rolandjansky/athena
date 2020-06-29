# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2
from ..external import ExternalMadSpin
import shutil
import os
from os.path import dirname, join

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class tj(PowhegV2):
    """
    Powheg configuration for single-top (t-channel) plus jet

    Details: NLO-accurate simulation of t-channel single top plus jet production matched to
             parton showers via the Powheg method. The calculation underlying the simulation
             is enhanced with a process-specific implementation of the multi-scale improved
             NLO (Minlo) method, such that it gives physical predictions all through phase
             space, including regions where the jet additional to the t-channel single-top
             process is unresolved.

    rw:     Event files can be OPTIONALLY reweighted using the rw executable (make rw) so as
            to use the 'tuned' MiNLO prime Sudakov form factor, which results in this NLO
            single-top+jet code giving results for inclusive single-top production quantities
            agreeing with NLO predictions for inclusive single-top production, at around the
            1-2% level, provided the events are generated with input parameters not TOO FAR
            from those used in determining the tune (in general the tune is considered highly
            robust against changes e.g. using it on 8 TeV event vs 13 TeV ones yields as good
            results as if one had two distinct dedicated tunes at each COM energy).


    Publication (please cite if using this process): https://arxiv.org/abs/1805.09855

    Currently identified issues:
    - Powheg seems to generate a top quark, whether a quark or antiquark is requested via decay_mode

    @author Stefan Richter  <stefan.richter@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """
        Constructor: all configurable process options are set here, with defaults. Global defaults from the
        registry are used if not set explicitly here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(tj, self).__init__(base_directory, "STJ", **kwargs)

        # This is a hacky fix that's needed at the moment...
        self.link_madloop_libraries()

        logger.info('Copying ident_card.dat needed by Powheg tj process to working directory')
        ident_card_location = join(dirname(self.executable), 'testrun/ident_card.dat')
        shutil.copy(ident_card_location, os.getcwd())

        # Add algorithms to the sequence
        # The MadSpin process is set to None because it depends on whether a top quark or top antiquark
        # is generated. As such, it will be set later, in the function `validate_decays` below
        self.add_algorithm(ExternalMadSpin(process=None))

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        # List of allowed decay modes
        # and their MadSpin decay strings (or None)
        self.allowed_decay_modes = {
            "t > undecayed"     : None,
            "t~ > undecayed"    : None,
            "t > b l+ vl"       : ["decay t > w+ b, w+ > l+ vl"],
            "t~ > b~ l- vl~"    : ["decay t~ > w- b~, w- > l- vl~"],
            "t > b j j"         : ["decay t > w+ b, w+ > j j"],
            "t~ > b~ j j"       : ["decay t~ > w- b~, w- > j j"],
            "t > all"           : ["decay t > w+ b, w+ > l+ vl",   "decay t > w+ b, w+ > j j"],
            "t~ > all"          : ["decay t~ > w- b~, w- > l- vl~",   "decay t~ > w- b~, w- > j j"],
            }

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("alphaem_inv")
        self.add_keyword("bmass")
        self.add_keyword("bornktmin", 0.3) # authors recommend not to modify this
        self.add_keyword("bornonly", 0)
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
        self.add_keyword("check_bad_st2", 1)
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
        self.add_keyword("foldcsi", 2)
        self.add_keyword("foldphi", 1)
        self.add_keyword("foldy", 5)
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 1)
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 3)
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax")
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
        self.add_keyword("maxseeds", 9999)
        self.add_keyword("minlo", 1)
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("ncall1", 10000000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 5000000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 5000000)
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
        self.add_keyword("smartsig")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("sthw2")
        self.add_keyword("storeinfo_rwgt", 1)
        self.add_keyword("storemintupb")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("tmass")
        self.add_keyword("topmass")
        self.add_keyword("topwidth")
        # The decay mode also controls whether a top quark (t) or antitop quark (t~) is generated
        self.add_keyword("ttype", "t > undecayed", name="decay_mode")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("withdamp", 1)
        self.add_keyword("withnegweights", 1) # authors: needed for this process
        self.add_keyword("withsubtr")
        self.add_keyword("wmass")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 2.0)



    def validate_decays(self):
        """
        Validate decay_mode keyword and translate it from ATLAS input to Powheg input
        """
        self.expose() # convenience call to simplify syntax
        if self.decay_mode not in self.allowed_decay_modes.keys():
            error_message = "Decay mode '{given}' not recognised, valid choices are: '{choices}'!".format(given=self.decay_mode, choices="', '".join(self.allowed_decay_modes.keys()))
            logger.warning(error_message)
            raise ValueError(error_message)

        # Calculate appropriate decay mode numbers
        antitop = self.decay_mode.startswith("t~")
        self.parameters_by_keyword("ttype")[0].value = -1 if antitop else 1

        # Check if top quark decays are requested.
        # Accordingly, MadSpin will run or not run.
        if "undecayed" in self.decay_mode:
            return

        # Configure MadSpin run:
        madspin_config = {
            "powheg_top_decays_enabled" : False,
            "MadSpin_decays" : self.allowed_decay_modes[self.decay_mode],
            "MadSpin_enabled" : True,
            "MadSpin_mode" : "full",
            "MadSpin_model" : "loop_sm-no_b_mass", # "loop_sm-ckm",
            "MadSpin_nFlavours" : 5, # this Powheg process is calculated in the five-flavour scheme
            "MadSpin_process" : "generate p p > t~ j j $$ w+ w- [QCD]" if antitop else "generate p p > t j j $$ w+ w- [QCD]",
        }
        for key, val in madspin_config.items():
            self.externals["MadSpin"].parameters_by_keyword(key)[0].value = val
