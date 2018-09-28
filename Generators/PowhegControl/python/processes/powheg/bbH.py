# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class bbH(PowhegV2):
    """! Default Powheg configuration for Higgs boson production in association with top quarks.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory  path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(bbH, self).__init__(base_directory, "bbH", **kwargs)

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
        self.add_keyword("check_bad_st2", 1)
        self.add_keyword("clobberlhe")
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("delta_mbbmin")
        self.add_keyword("doublefsr")
        self.add_keyword("evenmaxrat")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fakevirt")
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 1)
        self.add_keyword("foldphi", 1)
        self.add_keyword("foldy", 1)
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
        self.add_keyword("hdamp")
        self.add_keyword("hdecaymode")
        self.add_keyword("hfact")
        self.add_keyword("hmass")
        self.add_keyword("hwidth")
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 6)
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
        self.add_keyword("maxseeds")
        self.add_keyword("minlo")
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("msbar")
        self.add_keyword("ncall1", 8000000)
        self.add_keyword("ncall1rm", 8000000)
        self.add_keyword("ncall2", 8000000)
        self.add_keyword("ncall2rm", 8000000)
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
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("toploop")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 2)

# grep -A10 "suspicious" processes/bbH/default/log.generate
# 01:57:12 Py:PowhegControl     INFO     | Process #9: check_stat_consistency:
# 01:57:12 Py:PowhegControl     INFO     | Process #9: The program has detected inconsistent results
# 01:57:12 Py:PowhegControl     INFO     | Process #9: among different runs. The runs:
# 01:57:12 Py:PowhegControl     INFO     | Process #9: 6
# 01:57:12 Py:PowhegControl     INFO     | Process #9: look suspicious.
# 01:57:12 Py:PowhegControl     INFO     | Process #9: Inspect your runs at stage 2
# 01:57:12 Py:PowhegControl     INFO     | Process #9: The fraction of inconsistent file is < 10%
# 01:57:12 Py:PowhegControl     INFO     | Process #9: We discard the following files:
# 01:57:12 Py:PowhegControl     INFO     | Process #9: 6
# 01:57:12 Py:PowhegControl     INFO     | Process #9: and reload the others
# 01:57:12 Py:PowhegControl     INFO     | Process #9: Opened pwggrid-0001.dat
# 01:57:12 Py:PowhegControl     INFO     | Process #9: Opened pwggrid-0002.dat
# 01:57:12 Py:PowhegControl     INFO     | Process #9: Opened pwggrid-0003.dat
# 01:57:12 Py:PowhegControl     INFO     | Process #9: Opened pwggrid-0004.dat
# 01:57:12 Py:PowhegControl     INFO     | Process #9: Opened pwggrid-0005.dat
# 01:57:12 Py:PowhegControl     INFO     | Process #9: Opened pwggrid-0007.dat
# 01:57:12 Py:PowhegControl     INFO     | Process #9: Opened pwggrid-0008.dat
# 01:57:12 Py:PowhegControl     INFO     | Process #9: Opened pwggrid-0009.dat
# 01:57:12 Py:PowhegControl     INFO     | Process #9: Opened pwggrid-0010.dat


#   self.add_keyword("foldcsi", 2)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 8)
#   self.add_keyword("itmx2", 4)
#   self.add_keyword("ncall1", 2000000)
#   self.add_keyword("ncall2", 400000)
#   self.add_keyword("nubound", 50000)
#   self.add_keyword("xupbound", 2)
#   Cross-section test : 75.33% FAILED
#   Neg. weight test   : 89.74% FAILED
#   Upper bound test   : 62.36% FAILED
#     > cross-section  : 62.36%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 9.76 hours [0.016Hz]
#   ... time taken for integration:   9.36 hours
#   ... est. generation of 5k events: 3.59 hours


#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 2)
#   self.add_keyword("itmx2", 1)
#   self.add_keyword("ncall1", 500000)
#   self.add_keyword("ncall2", 50000)
#   self.add_keyword("nubound", 10000)
#   self.add_keyword("xupbound", 2)
#   Cross-section test : 70.89% FAILED
#   Neg. weight test   : 0.31% PASSED
#   Upper bound test   : 15.64% FAILED
#     > cross-section  : 15.64%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 0.89 hours [0.171Hz]
#   ... time taken for integration:   0.89 hours
#   ... est. generation of 5k events: 0.04 hours


#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 4)
#   self.add_keyword("itmx2", 1)
#   self.add_keyword("ncall1", 2000000)
#   self.add_keyword("ncall2", 200000)
#   self.add_keyword("nubound", 50000)
#   self.add_keyword("xupbound", 2)
#   Cross-section test : 12.39% FAILED
#   Neg. weight test   : 21.75% FAILED
#   Upper bound test   : 19.45% FAILED
#     > cross-section  : 19.45%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 4.59 hours [0.033Hz]
#   ... time taken for integration:   4.58 hours
#   ... est. generation of 5k events: 0.05 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 6)
#   self.add_keyword("itmx2", 2)
#   self.add_keyword("ncall1", 3000000)
#   self.add_keyword("ncall2", 2000000)
#   self.add_keyword("nubound", 500000)
#   self.add_keyword("xupbound", 2)
#   Cross-section test : 43.45% FAILED
#   Neg. weight test   : 56.32% FAILED
#   Upper bound test   : 60.73% FAILED
#     > cross-section  : 60.73%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 7.86 hours [0.019Hz]
#   ... time taken for integration:   7.76 hours
#   ... est. generation of 5k events: 0.95 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 6)
#   self.add_keyword("itmx2", 2)
#   self.add_keyword("ncall1", 4000000)
#   self.add_keyword("ncall2", 3000000)
#   self.add_keyword("nubound", 1000000)
#   self.add_keyword("xupbound", 2)
# 2 files failed

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 6)
#   self.add_keyword("itmx2", 4)
#   self.add_keyword("ncall1", 4000000)
#   self.add_keyword("ncall2", 3000000)
#   self.add_keyword("nubound", 1000000)
#   self.add_keyword("xupbound", 2)
# 2 files failed

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 6)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 5000000)
#   self.add_keyword("ncall2", 5000000)
#   self.add_keyword("nubound", 1000000)
#   self.add_keyword("xupbound", 2)
# 4 files failed

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 8)
#   self.add_keyword("itmx2", 8)
#   self.add_keyword("ncall1", 2000000)
#   self.add_keyword("ncall2", 200000)
#   self.add_keyword("nubound", 50000)
#   self.add_keyword("xupbound", 2)
# 2 files failed

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 8)
#   self.add_keyword("itmx2", 8)
#   self.add_keyword("ncall1", 500000)
#   self.add_keyword("ncall2", 2000000)
#   self.add_keyword("nubound", 50000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 59.92% FAILED
#  Neg. weight test   : 53.04% FAILED
#  Upper bound test   : 67.60% FAILED
#    > cross-section  : 67.60%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 9.97 hours [0.153Hz]
#  ... time taken for integration:   9.47 hours
#  ... est. generation of 5k events: 0.45 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 10)
#   self.add_keyword("itmx2", 10)
#   self.add_keyword("ncall1", 500000)
#   self.add_keyword("ncall2", 2000000)
#   self.add_keyword("nubound", 50000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 49.32% FAILED
#  Neg. weight test   : 82.42% FAILED
#  Upper bound test   : 53.85% FAILED
#    > cross-section  : 53.85%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 12.85 hours [0.119Hz]
#  ... time taken for integration:   11.09 hours
#  ... est. generation of 5k events: 1.60 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 10)
#   self.add_keyword("itmx2", 10)
#   self.add_keyword("ncall1", 2000000)
#   self.add_keyword("ncall2", 2000000)
#   self.add_keyword("nubound", 50000)
#   self.add_keyword("xupbound", 2)
# timed out

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 10)
#   self.add_keyword("itmx2", 10)
#   self.add_keyword("ncall1", 1000000)
#   self.add_keyword("ncall2", 2000000)
#   self.add_keyword("nubound", 50000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 44.07% FAILED
#  Neg. weight test   : 19.87% FAILED
#  Upper bound test   : 57.85% FAILED
#    > cross-section  : 57.85%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 20.33 hours [0.075Hz]
#  ... time taken for integration:   16.25 hours
#  ... est. generation of 5k events: 3.70 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 10)
#   self.add_keyword("itmx2", 10)
#   self.add_keyword("ncall1", 1500000)
#   self.add_keyword("ncall2", 1500000)
#   self.add_keyword("nubound", 50000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 58.72% FAILED
#  Neg. weight test   : 21.39% FAILED
#  Upper bound test   : 54.45% FAILED
#    > cross-section  : 54.45%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 25.26 hours [0.060Hz]
#  ... time taken for integration:   15.61 hours
#  ... est. generation of 5k events: 8.77 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 10)
#   self.add_keyword("itmx2", 10)
#   self.add_keyword("ncall1", 1700000)
#   self.add_keyword("ncall2", 1700000)
#   self.add_keyword("nubound", 50000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 78.82% FAILED
#  Neg. weight test   : 7.72% FAILED
#  Upper bound test   : 70.93% FAILED
#    > cross-section  : 70.93%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 12.77 hours [0.120Hz]
#  ... time taken for integration:   11.97 hours
#  ... est. generation of 5k events: 0.73 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 8)
#   self.add_keyword("itmx2", 8)
#   self.add_keyword("ncall1", 2000000)
#   self.add_keyword("ncall2", 2000000)
#   self.add_keyword("nubound", 50000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 47.67% FAILED
#  Neg. weight test   : 75.76% FAILED
#  Upper bound test   : 60.25% FAILED
#    > cross-section  : 60.25%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 14.34 hours [0.107Hz]
#  ... time taken for integration:   12.72 hours
#  ... est. generation of 5k events: 1.47 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 8)
#   self.add_keyword("itmx2", 8)
#   self.add_keyword("ncall1", 3000000)
#   self.add_keyword("ncall2", 3000000)
#   self.add_keyword("nubound", 50000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 62.74% FAILED
#  Neg. weight test   : 14.26% FAILED
#  Upper bound test   : 63.24% FAILED
#    > cross-section  : 63.24%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 16.58 hours [0.092Hz]
#  ... time taken for integration:   15.84 hours
#  ... est. generation of 5k events: 0.67 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 8)
#   self.add_keyword("itmx2", 8)
#   self.add_keyword("ncall1", 4000000)
#   self.add_keyword("ncall2", 4000000)
#   self.add_keyword("nubound", 50000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 50.82% FAILED
#  Neg. weight test   : 81.79% FAILED
#  Upper bound test   : 71.85% FAILED
#    > cross-section  : 71.85%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 25.68 hours [0.059Hz]
#  ... time taken for integration:   18.18 hours
#  ... est. generation of 5k events: 6.82 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 6)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 5000000)
#   self.add_keyword("ncall2", 5000000)
#   self.add_keyword("nubound", 100000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 75.33% FAILED
#  Neg. weight test   : 89.74% FAILED
#  Upper bound test   : 59.55% FAILED
#    > cross-section  : 59.55%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 7.65 hours [0.200Hz]
#  ... time taken for integration:   5.52 hours
#  ... est. generation of 5k events: 1.94 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 6)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 8000000)
#   self.add_keyword("ncall2", 8000000)
#   self.add_keyword("nubound", 1000000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 45.94% FAILED
#  Neg. weight test   : 52.67% FAILED
#  Upper bound test   : 56.40% FAILED
#    > cross-section  : 56.40%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 38.24 hours [0.040Hz]
#  ... time taken for integration:   34.09 hours
#  ... est. generation of 5k events: 3.78 hours

