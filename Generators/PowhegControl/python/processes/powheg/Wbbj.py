# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...parameters import atlas_common
from ..powheg_V2 import PowhegV2

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class Wbbj(PowhegV2):
    """! Default Powheg configuration for W-boson plus b-quark pair production with an associated jet.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory  path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(Wbbj, self).__init__(base_directory, "Wbbj", **kwargs)

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        ## List of allowed decay modes
        self.allowed_decay_modes = ["w- > e- ve~", "w- > mu- vm~", "w- > tau- vt~", "w+ > e+ ve", "w+ > mu+ vm", "w+ > tau+ vt"]

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("atlas_scale")
        self.add_keyword("bmass")
        self.add_keyword("bornktmin", 1.0)
        self.add_keyword("bornonly")
        self.add_keyword("bornsuppfact", 5.0)
        self.add_keyword("bornsuppfactW")
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
        self.add_keyword("dummyvirtual")
        self.add_keyword("evenmaxrat")
        self.add_keyword("expm2bb")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 2)
        self.add_keyword("foldphi", 2)
        self.add_keyword("foldy")
        self.add_keyword("fullrwgt")
        self.add_keyword("hdamp", 175)
        self.add_keyword("hfact")
        self.add_keyword("icsimax", 3)
        self.add_keyword("idvecbos", hidden=True)
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 3)
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 10)
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax", 3)
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("lhapdf6maxsets")
        self.add_keyword("lhrwgt_descr")
        self.add_keyword("lhrwgt_group_combine")
        self.add_keyword("lhrwgt_group_name")
        self.add_keyword("lhrwgt_id")
        self.add_keyword("LOevents")
        self.add_keyword("m2bb")
        self.add_keyword("manyseeds")
        self.add_keyword("max_w_mass")
        self.add_keyword("max_io_bufsize")
        self.add_keyword("maxseeds")
        self.add_keyword("min_w_mass")
        self.add_keyword("minlo", 1)
        self.add_keyword("minloscfac")
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("MSbarscheme")
        self.add_keyword("ncall1", 150000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 650000)
        self.add_keyword("ncall2rm")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 450000)
        self.add_keyword("olddij")
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("parallelstage")
        self.add_keyword("pdfreweight")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("q2mergefac")
        self.add_keyword("radregion")
        self.add_keyword("raisingscales", 0)
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
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("sudakovbb")
        self.add_keyword("sudmw2mb")
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
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 6)

    def validate_decays(self):
        """! Validate idvecbos and vdecaymode keywords."""
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode, self.allowed_decay_modes)
        # Calculate appropriate decay mode numbers
        self.parameters_by_keyword("idvecbos")[0].value = 24 * [-1, +1][self.decay_mode.startswith("w+")]
        __decay_mode_lookup = {"e- ve~": 1, "mu- vm~": 2, "tau- vt~": 3, "e+ ve": 1, "mu+ vm": 2, "tau+ vt": 3}
        self.parameters_by_keyword("vdecaymode")[0].value = __decay_mode_lookup[self.decay_mode.split("> ")[1]]

#        self.add_keyword("icimax", 1)
#        self.add_keyword("iymax", 1)
#        self.add_keyword("ncall1", 60000)
#        self.add_keyword("ncall2", 500000)
#        self.add_keyword("nubound", 300000)
#  Cross-section test : 0.48% PASSED
#  Neg. weight test   : 18.46% FAILED
#  Upper bound test   : 1.11% FAILED
#    > cross-section  : 1.11%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 9.15 hours [0.167Hz]
#  ... time taken for integration:   8.79 hours
#  ... est. generation of 5k events: 0.32 hours

#        self.add_keyword("icimax", 1)
#        self.add_keyword("iymax", 1)
#        self.add_keyword("ncall1", 100000)
#        self.add_keyword("ncall2", 550000)
#        self.add_keyword("nubound", 350000)
#  Cross-section test : 0.36% PASSED
#  Neg. weight test   : 18.39% FAILED
#  Upper bound test   : 1.16% FAILED
#    > cross-section  : 1.16%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 7.77 hours [0.197Hz]
#  ... time taken for integration:   7.41 hours
#  ... est. generation of 5k events: 0.33 hours


#        self.add_keyword("icimax", 2)
#        self.add_keyword("iymax", 2)
#        self.add_keyword("ncall1", 100000)
#        self.add_keyword("ncall2", 550000)
#        self.add_keyword("nubound", 350000)
#  Cross-section test : 0.36% PASSED
#  Neg. weight test   : 18.39% FAILED
#  Upper bound test   : 1.09% FAILED
#    > cross-section  : 1.09%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 8.78 hours [0.174Hz]
#  ... time taken for integration:   8.35 hours
#  ... est. generation of 5k events: 0.39 hours

#        self.add_keyword("icimax", 3)
#        self.add_keyword("iymax", 3)
#        self.add_keyword("ncall1", 110000)
#        self.add_keyword("ncall2", 560000)
#        self.add_keyword("nubound", 360000)
#  Cross-section test : 0.75% PASSED
#  Neg. weight test   : 18.76% FAILED
#  Upper bound test   : 1.05% FAILED
#    > cross-section  : 1.05%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 7.87 hours [0.194Hz]
#  ... time taken for integration:   6.97 hours
#  ... est. generation of 5k events: 0.82 hours

#        self.add_keyword("icimax", 3)
#        self.add_keyword("iymax", 3)
#        self.add_keyword("ncall1", 110000)
#        self.add_keyword("ncall2", 600000)
#        self.add_keyword("nubound", 400000)
#  Cross-section test : 0.70% PASSED
#  Neg. weight test   : 18.76% FAILED
#  Upper bound test   : 1.04% FAILED
#    > cross-section  : 1.04%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 6.78 hours [0.225Hz]
#  ... time taken for integration:   6.43 hours
#  ... est. generation of 5k events: 0.32 hours

#        self.add_keyword("icimax", 3)
#        self.add_keyword("iymax", 3)
#        self.add_keyword("ncall1", 150000)
#        self.add_keyword("ncall2", 650000)
#        self.add_keyword("nubound", 450000)
#  Cross-section test : 0.38% PASSED
#  Neg. weight test   : 18.59% FAILED
#  Upper bound test   : 1.00% PASSED
#    > cross-section  : 1.00%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 6.37 hours [0.240Hz]
#  ... time taken for integration:   6.03 hours
#  ... est. generation of 5k events: 0.30 hours

#        self.add_keyword("foldcsi", 2)
#        self.add_keyword("foldphi", 1)
#        self.add_keyword("foldy", 1)
#        self.add_keyword("icimax", 3)
#        self.add_keyword("iymax", 3)
#        self.add_keyword("ncall1", 150000)
#        self.add_keyword("ncall2", 650000)
#        self.add_keyword("nubound", 450000)
#  Cross-section test : 0.96% PASSED
#  Neg. weight test   : 15.65% FAILED
#  Upper bound test   : 0.89% PASSED
#    > cross-section  : 0.89%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 7.08 hours [0.216Hz]
#  ... time taken for integration:   6.76 hours
#  ... est. generation of 5k events: 0.29 hours

#        self.add_keyword("foldcsi", 2)
#        self.add_keyword("foldphi", 2)
#        self.add_keyword("foldy", 1)
#        self.add_keyword("icimax", 3)
#        self.add_keyword("iymax", 3)
#        self.add_keyword("ncall1", 150000)
#        self.add_keyword("ncall2", 650000)
#        self.add_keyword("nubound", 450000)
