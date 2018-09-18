# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ..powheg_V2 import PowhegV2


class Hjj(PowhegV2):
    """! Default Powheg configuration for Higgs boson production plus two jets.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(Hjj, self).__init__(base_directory, "HJJ", **kwargs)

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("alphas_from_lhapdf")
        self.add_keyword("bornktmin", 0.26)
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
        self.add_keyword("bwcutoff")
        self.add_keyword("charmthr")
        self.add_keyword("charmthrpdf")
        self.add_keyword("check_bad_st2")
        self.add_keyword("ckkwscalup")
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
        self.add_keyword("foldphi", 2)
        self.add_keyword("foldy", 5)
        self.add_keyword("fullphsp")
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("higgsfixedwidth")
        self.add_keyword("hmass")
        self.add_keyword("hwidth")
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1")
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 4)
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
        self.add_keyword("minlo", 1)
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("ncall1", 150000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 250000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 250000)
        self.add_keyword("olddij")
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp", 2)
        self.add_keyword("par_dijexp", 2)
        self.add_keyword("parallelstage")
        self.add_keyword("pdfreweight")
        self.add_keyword("polecheck")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("radregion")
        self.add_keyword("raisingscales")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("runningscales")
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
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 6)

#   self.add_keyword("foldcsi", 5)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 1)
#   self.add_keyword("ncall1", 100000)
#   self.add_keyword("ncall2", 100000)
#   self.add_keyword("nubound", 100000)
#   self.add_keyword("xupbound", 4)
#   Cross-section test : 1.44% FAILED
#   Neg. weight test   : 14.61% FAILED
#   Upper bound test   : 5.27% FAILED
#   1.73hrs on 10 cores

#   self.add_keyword("foldcsi", 5)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 1)
#   self.add_keyword("ncall1", 100000)
#   self.add_keyword("ncall2", 150000)
#   self.add_keyword("nubound", 150000)
#   self.add_keyword("xupbound", 5)
#   Cross-section test : 2.04% FAILED
#   Neg. weight test   : 14.61% FAILED
#   Upper bound test   : 1.82% FAILED
#     > cross-section  : 1.82%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 2.89 hours [0.053Hz] on 10 cores
#   ... time taken for integration:   2.62 hours
#   ... est. generation of 5k events: 2.44 hours

#   self.add_keyword("foldcsi", 5)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 1)
#   self.add_keyword("ncall1", 170000)
#   self.add_keyword("ncall2", 170000)
#   self.add_keyword("nubound", 100000)
#   self.add_keyword("xupbound", 5)
#   Cross-section test : 0.87% PASSED
#   Neg. weight test   : 13.97% FAILED
#   Upper bound test   : 2.18% FAILED
#     > cross-section  : 2.18%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 2.61 hours [0.059Hz]
#   ... time taken for integration:   2.52 hours
#   ... est. generation of 5k events: 0.82 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 1)
#   self.add_keyword("ncall1", 170000)
#   self.add_keyword("ncall2", 170000)
#   self.add_keyword("nubound", 150000)
#   self.add_keyword("xupbound", 5)
#   Cross-section test : 1.02% FAILED
#   Neg. weight test   : 30.84% FAILED
#   Upper bound test   : 3.27% FAILED
#     > cross-section  : 3.27%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 0.65 hours [0.236Hz]
#   ... time taken for integration:   0.63 hours
#   ... est. generation of 5k events: 0.21 hours

#   self.add_keyword("foldcsi", 2)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 1)
#   self.add_keyword("ncall1", 170000)
#   self.add_keyword("ncall2", 200000)
#   self.add_keyword("nubound", 200000)
#   self.add_keyword("xupbound", 5)
#   Cross-section test : 1.09% FAILED
#   Neg. weight test   : 28.86% FAILED
#   Upper bound test   : 2.91% FAILED
#     > cross-section  : 2.91%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 0.85 hours [0.181Hz]
#   ... time taken for integration:   0.82 hours
#   ... est. generation of 5k events: 0.23 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 2)
#   self.add_keyword("ncall1", 200000)
#   self.add_keyword("ncall2", 200000)
#   self.add_keyword("nubound", 200000)
#   self.add_keyword("xupbound", 6)
#   Cross-section test : 0.85% PASSED
#   Neg. weight test   : 30.11% FAILED
#   Upper bound test   : 2.18% FAILED
#     > cross-section  : 2.18%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 1.48 hours [0.103Hz]
#   ... time taken for integration:   1.43 hours
#   ... est. generation of 5k events: 0.53 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 3)
#   self.add_keyword("ncall1", 200000)
#   self.add_keyword("ncall2", 250000)
#   self.add_keyword("nubound", 250000)
#   self.add_keyword("xupbound", 6)
#   Cross-section test : 0.67% PASSED
#   Neg. weight test   : 25.70% FAILED
#   Upper bound test   : 1.27% FAILED
#     > cross-section  : 1.27%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 2.17 hours [0.071Hz]
#   ... time taken for integration:   2.11 hours
#   ... est. generation of 5k events: 0.51 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 3)
#   self.add_keyword("ncall1", 150000)
#   self.add_keyword("ncall2", 300000)
#   self.add_keyword("nubound", 300000)
#   self.add_keyword("xupbound", 8)
#   Cross-section test : 0.68% PASSED
#   Neg. weight test   : 31.54% FAILED
#   Upper bound test   : 1.64% FAILED
#     > cross-section  : 1.64%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 1.78 hours [0.086Hz]
#   ... time taken for integration:   1.73 hours
#   ... est. generation of 5k events: 0.45 hours

#   self.add_keyword("foldcsi", 2)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 3)
#   self.add_keyword("ncall1", 150000)
#   self.add_keyword("ncall2", 300000)
#   self.add_keyword("nubound", 300000)
#   self.add_keyword("xupbound", 8)
#   Cross-section test : 0.60% PASSED
#   Neg. weight test   : 28.53% FAILED
#   Upper bound test   : 1.82% FAILED
#     > cross-section  : 1.82%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 2.18 hours [0.070Hz]
#   ... time taken for integration:   2.12 hours
#   ... est. generation of 5k events: 0.52 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 4)
#   self.add_keyword("ncall1", 100000)
#   self.add_keyword("ncall2", 400000)
#   self.add_keyword("nubound", 300000)
#   self.add_keyword("xupbound", 8)
#   Cross-section test : 0.82% PASSED
#   Neg. weight test   : 31.89% FAILED
#   Upper bound test   : 3.09% FAILED
#     > cross-section  : 3.09%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 3.15 hours [0.048Hz]
#   ... time taken for integration:   3.10 hours
#   ... est. generation of 5k events: 0.46 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 5)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 4)
#   self.add_keyword("ncall1", 180000)
#   self.add_keyword("ncall2", 300000)
#   self.add_keyword("nubound", 300000)
#   self.add_keyword("xupbound", 6)
#   Cross-section test : 0.64% PASSED
#   Neg. weight test   : 19.68% FAILED
#   Upper bound test   : 0.73% PASSED
#     > cross-section  : 0.73%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 7.88 hours [0.019Hz]
#   ... time taken for integration:   7.73 hours
#   ... est. generation of 5k events: 1.35 hours

#   self.add_keyword("foldcsi", 2)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 5)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 4)
#   self.add_keyword("ncall1", 180000)
#   self.add_keyword("ncall2", 290000)
#   self.add_keyword("nubound", 290000)
#   self.add_keyword("xupbound", 6)
#   Cross-section test : 0.48% PASSED
#   Neg. weight test   : 13.28% FAILED
#   Upper bound test   : 0.73% PASSED
#     > cross-section  : 0.73%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 14.04 hours [0.011Hz]
#   ... time taken for integration:   13.76 hours
#   ... est. generation of 5k events: 2.53 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 10)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 4)
#   self.add_keyword("ncall1", 180000)
#   self.add_keyword("ncall2", 290000)
#   self.add_keyword("nubound", 290000)
#   self.add_keyword("xupbound", 6)
#   Cross-section test : 0.44% PASSED
#   Neg. weight test   : 16.68% FAILED
#   Upper bound test   : 1.64% FAILED
#     > cross-section  : 1.64%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 13.44 hours [0.011Hz]
#   ... time taken for integration:   13.13 hours
#   ... est. generation of 5k events: 2.78 hours

#   self.add_keyword("foldcsi", 2)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 5)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 4)
#   self.add_keyword("ncall1", 170000)
#   self.add_keyword("ncall2", 290000)
#   self.add_keyword("nubound", 290000)
#   self.add_keyword("xupbound", 6)
#   Cross-section test : 0.43% PASSED
#   Neg. weight test   : 10.88% FAILED
#   Upper bound test   : 0.73% PASSED
#     > cross-section  : 0.73%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 24.57 hours [0.006Hz]
#   ... time taken for integration:   24.08 hours
#   ... est. generation of 5k events: 4.38 hours

#   self.add_keyword("foldcsi", 2)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 5)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 4)
#   self.add_keyword("ncall1", 150000)
#   self.add_keyword("ncall2", 250000)
#   self.add_keyword("nubound", 250000)
#   self.add_keyword("xupbound", 6)
#   Cross-section test : 0.48% PASSED
#   Neg. weight test   : 10.84% FAILED
#   Upper bound test   : 0.36% PASSED
#     > cross-section  : 0.36%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 17.15 hours [0.009Hz]
#   ... time taken for integration:   16.75 hours
#   ... est. generation of 5k events: 3.69 hours

#   self.add_keyword("foldcsi", 2)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 5)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 4)
#   self.add_keyword("ncall1", 100000)
#   self.add_keyword("ncall2", 200000)
#   self.add_keyword("nubound", 200000)
#   self.add_keyword("xupbound", 6)
#  Cross-section test : 0.96% PASSED
#  Neg. weight test   : 12.18% FAILED
#  Upper bound test   : 1.09% FAILED
#    > cross-section  : 1.09%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 550 events took 15.81 hours [0.010Hz]
#  ... time taken for integration:   15.39 hours
#  ... est. generation of 5k events: 3.80 hours

#   self.add_keyword("foldcsi", 2)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 5)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 4)
#   self.add_keyword("ncall1", 100000)
#   self.add_keyword("ncall2", 250000)
#   self.add_keyword("nubound", 200000)
#   self.add_keyword("xupbound", 6)
# failed

#   self.add_keyword("foldcsi", 2)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 5)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 4)
#   self.add_keyword("ncall1", 150000)
#   self.add_keyword("ncall2", 250000)
#   self.add_keyword("nubound", 250000)
#   self.add_keyword("xupbound", 6)
