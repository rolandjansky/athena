# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os
from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class ggF_HH(PowhegV2):
    """! Default Powheg configuration for gluon-fusion Higgs boson production with quark mass and EW effects.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(ggF_HH, self).__init__(base_directory, "ggHH", **kwargs)

        # For some reason, this process calls a python script which looks for .grid files in $PYTHONPATH
        # By appending the folder that they live in to PYTHONPATH it is able to find them
        os.environ["PYTHONPATH"] += ":" + self.executable.replace("pwhg_main", "Virtual")

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
        self.add_keyword("cHHH")
        self.add_keyword("clobberlhe")
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("doublefsr")
        self.add_keyword("evenmaxrat")
        self.add_keyword("facscfact")
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("fixedscale", description="Set renormalisation and factorisation scales to 2*m_H")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 2)
        self.add_keyword("foldphi", 2)
        self.add_keyword("foldy", 5)
        self.add_keyword("fullrwgt")
        self.add_keyword("gfermi")
        self.add_keyword("hdamp")
        self.add_keyword("hdecaymode")
        self.add_keyword("hfact")
        self.add_keyword("hmass")
        self.add_keyword("icsimax", 2)
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 2)
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 4)
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax", 2)
        self.add_keyword("lhans1")
        self.add_keyword("lhans2")
        self.add_keyword("lhapdf6maxsets")
        self.add_keyword("lhrwgt_descr")
        self.add_keyword("lhrwgt_group_combine")
        self.add_keyword("lhrwgt_group_name")
        self.add_keyword("lhrwgt_id")
        self.add_keyword("LOevents")
        self.add_keyword("manyseeds")
        self.add_keyword("max_io_bufsize")
        self.add_keyword("maxseeds")
        self.add_keyword("minlo", frozen=True)
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("mtdep", 3)
        self.add_keyword("ncall1", 10000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 15000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 20000)
        self.add_keyword("olddij")
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("parallelstage")
        self.add_keyword("pdfreweight")
        self.add_keyword("ptHHcut_CT")
        self.add_keyword("ptHHcut")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("radregion")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("renscfact")
        self.add_keyword("rescue_reals")
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
        self.add_keyword("topmass")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("Wmass")
        self.add_keyword("Wwidth")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound")
        self.add_keyword("zerowidth")
        self.add_keyword("Zmass")
        self.add_keyword("Zwidth")

#         self.add_keyword("foldcsi", 2)
#         self.add_keyword("foldphi", 2)
#         self.add_keyword("foldy", 5)
#         self.add_keyword("icsimax", 3)
#         self.add_keyword("itmx1", 5)
#         self.add_keyword("itmx2", 5)
#         self.add_keyword("iymax", 3)
#         self.add_keyword("mtdep", 3)
#         self.add_keyword("ncall1", 10000)
#         self.add_keyword("ncall2", 10000)
#         self.add_keyword("nubound", 20000)
#   Cross-section test : 0.19% PASSED
#   Neg. weight test   : 0.43% PASSED
#   Upper bound test   : 2.01% FAILED
#     > cross-section  : 2.01%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 5.94 hours [0.026Hz]
#   ... time taken for integration:   5.83 hours
#   ... est. generation of 5k events: 1.08 hours

#         self.add_keyword("foldcsi", 2)
#         self.add_keyword("foldphi", 2)
#         self.add_keyword("foldy", 5)
#         self.add_keyword("icsimax", 3)
#         self.add_keyword("itmx1", 3)
#         self.add_keyword("itmx2", 6)
#         self.add_keyword("iymax", 3)
#         self.add_keyword("mtdep", 3)
#         self.add_keyword("ncall1", 10000)
#         self.add_keyword("ncall2", 15000)
#         self.add_keyword("nubound", 20000)
#   Cross-section test : 0.20% PASSED
#   Neg. weight test   : 0.61% PASSED
#   Upper bound test   : 0.18% PASSED
#     > cross-section  : 0.18%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 9.87 hours [0.015Hz]
#   ... time taken for integration:   9.70 hours
#   ... est. generation of 5k events: 1.51 hours

#         self.add_keyword("foldcsi", 2)
#         self.add_keyword("foldphi", 2)
#         self.add_keyword("foldy", 5)
#         self.add_keyword("icsimax", 2)
#         self.add_keyword("itmx1", 2)
#         self.add_keyword("itmx2", 4)
#         self.add_keyword("iymax", 2)
#         self.add_keyword("mtdep", 3)
#         self.add_keyword("ncall1", 10000)
#         self.add_keyword("ncall2", 15000)
#         self.add_keyword("nubound", 20000)
#  Cross-section test : 0.12% PASSED
#  Neg. weight test   : 0.91% PASSED
#  Upper bound test   : 0.91% PASSED
#    > cross-section  : 0.91%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 550 events took 7.51 hours [0.020Hz]
#  ... time taken for integration:   7.36 hours
#  ... est. generation of 5k events: 1.38 hours

#         self.add_keyword("foldcsi", 2)
#         self.add_keyword("foldphi", 2)
#         self.add_keyword("foldy", 5)
#         self.add_keyword("icsimax", 2)
#         self.add_keyword("itmx1", 1)
#         self.add_keyword("itmx2", 4)
#         self.add_keyword("iymax", 2)
#         self.add_keyword("mtdep", 3)
#         self.add_keyword("ncall1", 10000)
#         self.add_keyword("ncall2", 15000)
#         self.add_keyword("nubound", 20000)
#  Cross-section test : 0.23% PASSED
#  Neg. weight test   : 1.44% FAILED
#  Upper bound test   : 0.91% PASSED
#    > cross-section  : 0.91%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 550 events took 6.39 hours [0.024Hz]
#  ... time taken for integration:   6.24 hours
#  ... est. generation of 5k events: 1.32 hours


