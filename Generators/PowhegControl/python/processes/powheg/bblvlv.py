# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ..powheg_RES import PowhegRES


class bblvlv(PowhegRES):
    """! Default Powheg configuration for top pair and Wt production including non-resonant and interference effects.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(bblvlv, self).__init__(base_directory, "b_bbar_4l", **kwargs)

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("allrad", 1)
        self.add_keyword("alpha")
        self.add_keyword("bmass")
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
        self.add_keyword("chklimseed")
        self.add_keyword("clobberlhe")
        self.add_keyword("colltest")
        self.add_keyword("complexGFermi")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("doublefsr")
        self.add_keyword("enhancereg")
        self.add_keyword("evenmaxrat")
        self.add_keyword("ewscheme")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedscale")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 5)
        self.add_keyword("foldphi", 5)
        self.add_keyword("foldy", 5)
        self.add_keyword("for_reweighting", 1)
        self.add_keyword("fullrwgt")
        self.add_keyword("hdamp", 172.5)
        self.add_keyword("hfact")
        self.add_keyword("hmass")
        self.add_keyword("hwidth")
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1")
        self.add_keyword("itmx1btl")
        self.add_keyword("itmx1btlbrn")
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 6)
        self.add_keyword("itmx2btl")
        self.add_keyword("itmx2btlbrn")
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
        self.add_keyword("mint_density_map")
        self.add_keyword("mintupbratlim")
        self.add_keyword("MSbarscheme")
        self.add_keyword("ncall1", 120000)
        self.add_keyword("ncall1btl")
        self.add_keyword("ncall1btlbrn")
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 180000)
        self.add_keyword("ncall2btl")
        self.add_keyword("ncall2btlbrn")
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("nores")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 100000)
        self.add_keyword("olpreset")
        self.add_keyword("olverbose")
        self.add_keyword("openloops-stability")
        self.add_keyword("openloopsreal")
        self.add_keyword("openloopsvirtual")
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
        self.add_keyword("regridfix")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("smartsig")
        self.add_keyword("softmismch")
        self.add_keyword("softonly")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("tmass_phsp")
        self.add_keyword("tmass")
        self.add_keyword("twidth")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails", -1)  # disable cross-section output to avoid Fortran crash
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("user_reshists_sep")
        self.add_keyword("verytinypars")
        self.add_keyword("virtonly")
        self.add_keyword("whichpwhgevent")
        self.add_keyword("withbtilde")
        self.add_keyword("withdamp", 1)
        self.add_keyword("withnegweights")
        self.add_keyword("withremnants")
        self.add_keyword("withsubtr")
        self.add_keyword("wmass")
        self.add_keyword("wwidth")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 2)
        self.add_keyword("zerowidth")
        self.add_keyword("zmass")
        self.add_keyword("zwidth")

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 10)
#   self.add_keyword("ncall1", 150000)
#   self.add_keyword("ncall2", 1000000)
#   self.add_keyword("nubound", 200000)
#   self.add_keyword("xupbound", 4)
#   Cross-section test : 0.19% PASSED
#   Neg. weight test   : 19.21% FAILED
#   Upper bound test   : 0.36% PASSED
#     > cross-section  : 0.36%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 9.36 hours [0.016Hz]
#   ... time taken for integration:   9.28 hours
#   ... est. generation of 5k events: 0.68 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 8)
#   self.add_keyword("ncall1", 100000)
#   self.add_keyword("ncall2", 600000)
#   self.add_keyword("nubound", 200000)
#   self.add_keyword("xupbound", 4)
#   Cross-section test : 0.36% PASSED
#   Neg. weight test   : 19.18% FAILED
#   Upper bound test   : 0.36% PASSED
#     > cross-section  : 0.36%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 5.95 hours [0.026Hz]
#   ... time taken for integration:   5.91 hours
#   ... est. generation of 5k events: 0.32 hours

#   self.add_keyword("foldcsi", 2)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 80000)
#   self.add_keyword("ncall2", 400000)
#   self.add_keyword("nubound", 150000)
#   self.add_keyword("xupbound", 4)
#   Cross-section test : 0.47% PASSED
#   Neg. weight test   : 16.04% FAILED
#   Upper bound test   : 0.18% PASSED
#     > cross-section  : 0.18%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 3.28 hours [0.047Hz]
#   ... time taken for integration:   3.22 hours
#   ... est. generation of 5k events: 0.61 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 1)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 60000)
#   self.add_keyword("ncall2", 300000)
#   self.add_keyword("nubound", 100000)
#   self.add_keyword("xupbound", 4)
#   Cross-section test : 0.65% PASSED
#   Neg. weight test   : 15.13% FAILED
#   Upper bound test   : 0.36% PASSED
#     > cross-section  : 0.36%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 2.90 hours [0.053Hz]
#   ... time taken for integration:   2.88 hours
#   ... est. generation of 5k events: 0.26 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 1)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 60000)
#   self.add_keyword("ncall2", 300000)
#   self.add_keyword("nubound", 100000)
#   self.add_keyword("xupbound", 4)
#   Cross-section test : 0.76% PASSED
#   Neg. weight test   : 12.78% FAILED
#   Upper bound test   : 0.18% PASSED
#     > cross-section  : 0.18%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 2.22 hours [0.069Hz]
#   ... time taken for integration:   2.19 hours
#   ... est. generation of 5k events: 0.24 hours

#   self.add_keyword("foldcsi", 1)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 60000)
#   self.add_keyword("ncall2", 200000)
#   self.add_keyword("nubound", 80000)
#   self.add_keyword("xupbound", 4)
#   Cross-section test : 0.87% PASSED
#   Neg. weight test   : 9.08% FAILED
#   Upper bound test   : 0.36% PASSED
#     > cross-section  : 0.36%
#     > radiation      : 0.00%
#   Output EVNT files  : 0     FAILED
#   Multi-core generation of 550 events took 2.39 hours [0.064Hz]
#   ... time taken for integration:   2.36 hours
#   ... est. generation of 5k events: 0.27 hours

#   self.add_keyword("foldcsi", 2)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 60000)
#   self.add_keyword("ncall2", 180000)
#   self.add_keyword("nubound", 60000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 0.77% PASSED
#  Neg. weight test   : 5.48% FAILED
#  Upper bound test   : 0.36% PASSED
#    > cross-section  : 0.36%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 550 events took 3.17 hours [0.048Hz]
#  ... time taken for integration:   3.14 hours
#  ... est. generation of 5k events: 0.28 hours


#   self.add_keyword("foldcsi", 5)
#   self.add_keyword("foldphi", 2)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 60000)
#   self.add_keyword("ncall2", 120000)
#   self.add_keyword("nubound", 60000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 1.05% FAILED
#  Neg. weight test   : 2.39% FAILED
#  Upper bound test   : 1.09% FAILED
#    > cross-section  : 1.09%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 550 events took 4.04 hours [0.038Hz]
#  ... time taken for integration:   3.98 hours
#  ... est. generation of 5k events: 0.57 hours

#   self.add_keyword("foldcsi", 5)
#   self.add_keyword("foldphi", 5)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 70000)
#   self.add_keyword("ncall2", 130000)
#   self.add_keyword("nubound", 60000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 0.79% PASSED
#  Neg. weight test   : 1.92% FAILED
#  Upper bound test   : 0.36% PASSED
#    > cross-section  : 0.36%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 550 events took 10.19 hours [0.015Hz]
#  ... time taken for integration:   10.05 hours
#  ... est. generation of 5k events: 1.28 hours

#   self.add_keyword("foldcsi", 5)
#   self.add_keyword("foldphi", 5)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 3)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 80000)
#   self.add_keyword("ncall2", 120000)
#   self.add_keyword("nubound", 60000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 11.28% FAILED
#  Neg. weight test   : 2.90% FAILED
#  Upper bound test   : nan% FAILED
#    > cross-section  : nan%
#    > radiation      : nan%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 69.44 hours [0.022Hz]
#  ... time taken for integration:   63.29 hours
#  ... est. generation of 5k events: 5.59 hours

#   self.add_keyword("foldcsi", 5)
#   self.add_keyword("foldphi", 5)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 80000)
#   self.add_keyword("ncall2", 120000)
#   self.add_keyword("nubound", 60000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 0.65% PASSED
#  Neg. weight test   : 2.08% FAILED
#  Upper bound test   : 0.69% PASSED
#    > cross-section  : 0.69%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 20.69 hours [0.074Hz]
#  ... time taken for integration:   18.21 hours
#  ... est. generation of 5k events: 2.25 hours

#   self.add_keyword("foldcsi", 5)
#   self.add_keyword("foldphi", 5)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 90000)
#   self.add_keyword("ncall2", 130000)
#   self.add_keyword("nubound", 60000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 0.57% PASSED
#  Neg. weight test   : 2.15% FAILED
#  Upper bound test   : 0.62% PASSED
#    > cross-section  : 0.62%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 11.61 hours [0.132Hz]
#  ... time taken for integration:   10.27 hours
#  ... est. generation of 5k events: 1.23 hours

#   self.add_keyword("foldcsi", 5)
#   self.add_keyword("foldphi", 5)
#   self.add_keyword("foldy", 2)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 120000)
#   self.add_keyword("ncall2", 180000)
#   self.add_keyword("nubound", 100000)
#   self.add_keyword("xupbound", 2)
#  Cross-section test : 0.51% PASSED
#  Neg. weight test   : 2.40% FAILED
#  Upper bound test   : 0.47% PASSED
#    > cross-section  : 0.47%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 15.57 hours [0.098Hz]
#  ... time taken for integration:   13.89 hours
#  ... est. generation of 5k events: 1.53 hours

#   self.add_keyword("foldcsi", 5)
#   self.add_keyword("foldphi", 5)
#   self.add_keyword("foldy", 5)
#   self.add_keyword("itmx1", 1)
#   self.add_keyword("itmx2", 6)
#   self.add_keyword("ncall1", 120000)
#   self.add_keyword("ncall2", 180000)
#   self.add_keyword("nubound", 100000)
#   self.add_keyword("xupbound", 2)

