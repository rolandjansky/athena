# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2
from ..external import ExternalMadSpin

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")



# Dictionary to convert the PowhegControl decay mode names to the appropriate
# decay mode numbers understood by Powheg
_decay_mode_lookup = {
    't t~ W- > all' : '33333',
    't t~ W- > b j j b~ j j j j' : '00033',
    't t~ W- > b l+ vl b~ l- vl~ l- vl~': '30000',
    't t~ W- > b j j vl b~ l- vl~ l- vl~': '30000', # additionally need to set the "samesignleptons" parameter
    't t~ W- > undecayed' : '0',
}



class ttWm_QCD(PowhegV2):
    """
    Powheg interface for ttW- production with NLO QCD corrections.

    Reference for this process: https://arxiv.org/abs/2101.11808

    @author Stefan Richter  <stefan.richter@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(ttWm_QCD, self).__init__(base_directory, "Wtt_dec/pp_ttWm_QCD", **kwargs)

        # List of allowed decay modes
        # (The sorting of the list is just to increase readability when it's printed)
        self.allowed_decay_modes = sorted(_decay_mode_lookup.keys())

        self.validation_functions.append("validate_decays")

        # Add all keywords for this process, overriding defaults if required
        # self.add_keyword("alphas_from_pdf", 1)       # not used by authors in example
        self.add_keyword("bornonly")
        self.add_keyword("BrWlep")
        self.add_keyword("btlscalect", 1)
        self.add_keyword("btlscalereal", 1)
        self.add_keyword("CKM_Vcd")
        self.add_keyword("CKM_Vcs")
        self.add_keyword("CKM_Vud")
        self.add_keyword("CKM_Vus")
        self.add_keyword("clobberlhe")                  # not used by authors in example
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe", 1)             # not used by authors in example
        self.add_keyword("compress_upb")                # not used by authors in example
        self.add_keyword("compute_rwgt")
        self.add_keyword("correlations")
        self.add_keyword("dampscfact")
        self.add_keyword("delta_mttmin")
        self.add_keyword("dynamic_hdamp")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fakevirt")
        self.add_keyword("fastbtlbound")                # not used by authors in example
        self.add_keyword("foldcsi", 2)
        self.add_keyword("foldphi", 2)
        self.add_keyword("foldy", 2)
        self.add_keyword("for_reweighting")             # not used by authors in example
        self.add_keyword("fullrwgt")                    # not used by authors in example
        self.add_keyword("fullrwgtmode")                # not used by authors in example
        self.add_keyword("gfermi")
        self.add_keyword("hbzd")
        self.add_keyword("hdamp")
        self.add_keyword("hmass")
        self.add_keyword("hwidth")
        self.add_keyword("icsimax", 3)
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 5)
        self.add_keyword("itmx2", 5)
        self.add_keyword("iymax", 3)
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("lhfm/bmass")
        self.add_keyword("lhfm/cmass")
        self.add_keyword("lhfm/emass")
        self.add_keyword("lhfm/mumass")
        self.add_keyword("lhfm/smass")
        self.add_keyword("lhfm/taumass")
        self.add_keyword("lhfm/umass")
        self.add_keyword("lhrwgt_descr")
        self.add_keyword("lhrwgt_group_combine")
        self.add_keyword("lhrwgt_group_name")
        self.add_keyword("lhrwgt_id")
        self.add_keyword("LOevents")
        self.add_keyword("manyseeds")                   # not used by authors in example
        self.add_keyword("maxseeds", 1000)              # not used by authors in example
        self.add_keyword("ncall1", 100000)
        self.add_keyword("ncall2", 100000)
        self.add_keyword("ncall2rm")                    # not used by authors in example
        self.add_keyword("nubound", 100000)
        self.add_keyword("parallelstage")               # not used by authors in example
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("samesignleptons")
        self.add_keyword("softtest")
        self.add_keyword("storeinfo_rwgt", 1)
        self.add_keyword("storemintupb")                # not used by authors in example
        self.add_keyword("tmass")
        self.add_keyword("topdecaymode", "t t~ W- > all", name="decay_mode")
        self.add_keyword("twidth")
        self.add_keyword("use-old-grid", 1)
        self.add_keyword("use-old-ubound", 1)
        self.add_keyword("withdamp", 1)                 # not used by authors in example
        self.add_keyword("wmass")
        self.add_keyword("wwidth")
        self.add_keyword("xgriditeration")              # not used by authors in example
        self.add_keyword("xupbound", 2)
        self.add_keyword("zerowidth")
        self.add_keyword("zmass")
        self.add_keyword("zwidth")



    def validate_decays(self):
        """
        Validate decay_mode keywords and translate them from ATLAS input to Powheg input
        """
        self.expose() # convenience call to simplify syntax
        if self.decay_mode not in self.allowed_decay_modes:
            error_message = "Decay mode '{given}' not recognised, valid choices are: '{choices}'!".format(given=self.decay_mode, choices="', '".join(self.allowed_decay_modes))
            logger.warning(error_message)
            raise ValueError(error_message)

        self.parameters_by_keyword("topdecaymode")[0].value = _decay_mode_lookup[self.decay_mode]
        if self.decay_mode == 't t~ W- > b j j vl b~ l- vl~ l- vl~':
            # Parameter samesignleptons must be set to 1 to actually get exactly two same-sign leptons
            self.parameters_by_keyword("samesignleptons")[0].value = 1
