# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2
import itertools

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class VBF_ZZ(PowhegV2):
    """
    Default Powheg configuration for opposite-sign W-boson pair plus jet pair production through vector boson fusion.

    Create a configurable object with all applicable Powheg options.

    Note that in the presence of a very sharp resonance, as is the case in a scenario with a light Higgs boson with a
    mass below 200 GeV, the resonant Higgs contribution has to be evaluated separately from the ZZ continuum, as
    described in https://arxiv.org/abs/1312.3252. In this case the entire event generation chain has to be performed
    for each of these two contributions separately in two distinct working directories, setting
    :code:`PowhegConfig.zz_res_type = 1`
    for the WW continuum and
    :code:`PowhegConfig.zz_res_type = 2`
    for the Higgs resonance contribution. After all the runs have been performed for each case, the results have to
    be added.
    If the Higgs is heavy and broad, such a splitting is not necessary, and all contributions can be evaluated at the
    same time, setting
    :code:PowhegConfig.zz_res_type = 0`.

    If you use this process for a publication, please cite:
    https://arxiv.org/abs/1312.3252
    as well as the usual Powheg(Box) publications

    @author Timothee Theveneaux-Pelzer  <tpelzer@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(VBF_ZZ, self).__init__(base_directory, "VBF_Z_Z", **kwargs)

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        ## List of allowed decay modes
        ### WARNING: "tau+ tau-" or "vt vt~" are not supported for this process
        z_decay_products = ["e+ e-", "mu+ mu-", "ve ve~", "vm vm~", "d d~", "s s~", "u u~", "c- c~", "(d d~ / s s~)", "(u u~ / c c~)" ]

        self.allowed_decay_modes = ["z z > {} {}".format(*x) for x in itertools.product(z_decay_products, z_decay_products)]
        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("alphas_from_lhapdf")
        self.add_keyword("bornktmin")
        self.add_keyword("bornonly")
        self.add_keyword("bornsuppfact", 100) # self.add_keyword("bornsuppfact", 1)
        self.add_keyword("bornzerodamp")
        self.add_keyword("bottomthr")
        self.add_keyword("bottomthrpdf")
        self.add_keyword("btildeborn")
        self.add_keyword("btildecoll")
        self.add_keyword("btildereal")
        self.add_keyword("btildevirt")
        self.add_keyword("btlscalect", 1)
        self.add_keyword("btlscalereal", 1)
        self.add_keyword("channel_type", 0) # [0:all (default), 7: all Neutral Currents, 1:ucuc-type NC, 2:usus-type NC, 3:dcdc-type NC, 4:dsds-type NC, 8: all Charged Currents, 5: usdc-type CC, 6: dcus-type CC]
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
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fakevirt")
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 2)
        self.add_keyword("foldphi")
        self.add_keyword("foldy", 2)
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("hmass")
        self.add_keyword("hwidth")
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 3)
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 6)
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
        self.add_keyword("maxseeds", 1000)
        self.add_keyword("minlo")
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("mll_gencut", 20)
        self.add_keyword("ncall1", 700000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 900000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 400000)
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
        self.add_keyword("runningscales", 1)
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
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
        self.add_keyword("vdecaymodeZ2", hidden=True)
        self.add_keyword("vdecaymodeZ1", self.allowed_decay_modes[0], name="decay_mode")
        self.add_keyword("withdamp", 1)
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("wmass")
        self.add_keyword("wwidth")
        self.add_keyword("zmass")
        self.add_keyword("zwidth")
        self.add_keyword("zz_res_type")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 2)
        self.add_keyword("zerowidth")
        # Note: the process-specific parameter 'fat jet' is not currently implemented in PowhegControl.
        #       If you need it, please contact the PowhegControl maintainer(s).


    def validate_decays(self):
        """! Validate idvecbos, vdecaymodew1 and vdecaymodew2 keywords."""
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode, self.allowed_decay_modes)
        # Calculate appropriate decay mode numbers
        _decay_products = [p for p in self.decay_mode.split("> ")[1].split(" ")]
        _decay_mode_lookup = {
            "e+ e-": 11,
            "mu+ mu-": 13,
            "ve ve~": 12,
            "vm vm~": 14,
            "d d~" : 1,
            "s s~" : 3,
            "u u~": 2,
            "c- c~": 4,
            "(d d~ / s s~)" : 7,
            "(u u~ / c c~)" : 8,
        }
        self.parameters_by_keyword("vdecaymodeZ1")[0].value = _decay_mode_lookup[" ".join(_decay_products[0:2])]
        self.parameters_by_keyword("vdecaymodeZ2")[0].value = _decay_mode_lookup[" ".join(_decay_products[2:4])]
