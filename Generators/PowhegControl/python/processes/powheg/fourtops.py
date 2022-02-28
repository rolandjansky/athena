# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...parameters import powheg_atlas_common
from ..powheg_V2 import PowhegV2
from ..external import ExternalMadSpin

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


# Dictionary to convert the PowhegControl decay mode names to the appropriate
# decay mode numbers understood by Powheg
_decay_mode_lookup = {
    "t t~ t t~ > all [MadSpin]" : "00000", # switch off decays in Powheg and let MadSpin handle them!
    "t t~ t t~ > all": "44444", # all decays
    "t t~ t t~ > 4l": "44400", # exactly 4 leptons
    "t t~ t t~ > 3l4l": "4", # at least 3 leptons
    "t t~ t t~ > 2lSS": "3", # exactly 2 leptons, same sign
    "t t~ t t~ > 2lOS": "2", # exactly 2 leptons, opposite sign
    "t t~ t t~ > 1l": "1", # exactly 1 lepton
    "t t~ t t~ > allhad": "00044", # fully-hadronic
    "t t~ t t~ > undecayed": "00000" # undecayed (e.g. to produce a lhe file)
                        }
_signature_lookup = [ "4", "3", "2", "1" ] # these values are for the 4tops-specific #decay_signature keyword

class fourtops(PowhegV2):
    """! Default Powheg configuration for 4 tops production.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <tpelzer@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(fourtops, self).__init__(base_directory, "fourtops", **kwargs)

        # Add algorithms to the sequence
        self.add_algorithm(ExternalMadSpin(process="generate p p > t t~ t t~ [QCD]"))

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        # List of allowed decay modes
        # (The sorting of the list is just to increase readability when it's printed)
        self.allowed_decay_modes = sorted(_decay_mode_lookup.keys())

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("bmass_lhe")
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
        self.add_keyword("cmass_lhe")
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("decay_signature", "0")
        self.add_keyword("doublefsr")
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
        self.add_keyword("itmx1")
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 8)
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
        self.add_keyword("maxseeds")
        self.add_keyword("minlo")
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("ncall1", 500)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 50000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 800000)
        self.add_keyword("olddij")
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("parallelstage")
        self.add_keyword("pdfreweight")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("tmass", powheg_atlas_common.mass.t, name="mass_t", description="top quark mass in GeV")
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
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("tdec/bmass")
        self.add_keyword("tdec/cmass")
        self.add_keyword("tdec/dmass")
        self.add_keyword("tdec/elbranching")
        self.add_keyword("tdec/emass")
        self.add_keyword("tdec/mumass")
        self.add_keyword("tdec/sin2cabibbo")
        self.add_keyword("tdec/smass")
        self.add_keyword("tdec/taumass")
        self.add_keyword("tdec/twidth")
        self.add_keyword("tdec/umass")
        self.add_keyword("tdec/wmass")
        self.add_keyword("tdec/wwidth")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("topdecaymode", "t t~ > all", name="decay_mode")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 10)

    def validate_decays(self):
        """
        Validate semileptonic and topdecaymode keywords and translate them from ATLAS input to Powheg input
        """
        self.expose() # convenience call to simplify syntax
        if self.decay_mode not in self.allowed_decay_modes:
            error_message = "Decay mode '{given}' not recognised, valid choices are: '{choices}'!".format(given=self.decay_mode, choices="', '".join(self.allowed_decay_modes))
            logger.warning(error_message)
            raise ValueError(error_message)

        # Check if MadSpin decays are requested.
        # Accordingly, MadSpin will run or not run.
        if "MadSpin" in self.decay_mode:
            self.externals["MadSpin"].parameters_by_keyword("powheg_top_decays_enabled")[0].value = False
            self.externals["MadSpin"].parameters_by_keyword("MadSpin_model")[0].value = "loop_sm-no_b_mass"
            self.externals["MadSpin"].parameters_by_keyword("MadSpin_nFlavours")[0].value = 5

        if _decay_mode_lookup[self.decay_mode] in _signature_lookup: # special handling of decays with #decay_signature keyword
            self.parameters_by_keyword("decay_signature")[0].value = _decay_mode_lookup[self.decay_mode]
            self.parameters_by_keyword("topdecaymode")[0].value = "00000"
        else: # usual handling of decays with #topdecaymode keyword
            self.parameters_by_keyword("decay_signature")[0].value = "0"
            self.parameters_by_keyword("topdecaymode")[0].value = _decay_mode_lookup[self.decay_mode]
