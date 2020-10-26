# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from ..powheg_V2 import PowhegV2
from textwrap import dedent


class yj(PowhegV2):
    """
    Powheg configuration for direct photon production, defined as having the leading-order processes
    q q~ -> y g
    q g  -> y q
    q~ g -> y q~

    Publication (please cite if using this process): https://arxiv.org/abs/1610.02275

    For a study using this process, see https://arxiv.org/abs/1709.04154

    The enhancement of photon radiation is described on page 137ff of this PhD thesis:
    https://www.uni-muenster.de/imperia/md/content/physik_tp/theses/klasen/koenig_phd.pdf

    @author Stefan Richter  <stefan.richter@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(yj, self).__init__(base_directory, "directphoton", **kwargs)

        # Add the writing of an initial reweighting file needed to get the Sudakov reweighting factor
        # into the nominal event weight.
        self.validation_functions.append("write_enhancedradfac_reweighting_file")

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("alphaem_inv")
        self.add_keyword("alphas_from_lhapdf")
        self.add_keyword("bornktmin", 50.0)
        self.add_keyword("bornonly")
        # According to the authors' example located at (e.g.)
        # /cvmfs/atlas.cern.ch/repo/sw/Generators/powheg/ATLASOTF-00-04-02/POWHEG-BOX-V2/directphoton/testrun-lhc/powheg.input-save
        # bornsuppfact=4000 is used "to get photons over a wide range of 100 GeV
        # to over 1 TeV, not for low energy photons":
        self.add_keyword("bornsuppfact", 4000)
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
        self.add_keyword("doublefsr", 1)
        self.add_keyword("emvirtual")
        self.add_keyword("enhancedradfac", 50)
        self.add_keyword("evenmaxrat")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("fixedscale")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 2)
        self.add_keyword("foldphi", 1)
        self.add_keyword("foldy", 5)
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("icsimax", 1)
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 5)
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 5)
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr", 1)
        self.add_keyword("iupperisr")
        self.add_keyword("iymax", 5)
        self.add_keyword("jacsing")
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
        self.add_keyword("ncall1", 500000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 500000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 500000)
        self.add_keyword("olddij")
        self.add_keyword("par_2gsupp", 4)
        self.add_keyword("par_diexp", 4)
        self.add_keyword("par_dijexp", 4)
        self.add_keyword("parallelstage")
        #self.add_keyword("pdfreweight")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("radregion")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("renscfact", self.default_scales[1])
        # Reweighting needs to already be run during the initial event
        # generation for this process. The reason is that the additional event
        # weight multiplied by a special Sudakov reweighting factor for enhanced
        # photon radiation sampling (needed to get good signal statistics) needs
        # to be written into the LHE file to be able to use it later as the
        # nominal event weight. Any additional multiweights also use this weight
        # as the reference weight. The "bare" weight not including the Sudakov
        # reweighting factor is unphysical and should not be used. To enable the
        # reweighting, a rwl_file provided (which is written
        # automatically when this constructor is called).
        # Note that rwl_add has to be set to 0! This parameter is for adding
        # further weights to an LHE file from a previous run
        self.add_keyword("rwl_add", 0)
        self.add_keyword("rwl_file", 'reweighting_needed_for_enhancedradfac.xml')
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
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 2)

    def write_enhancedradfac_reweighting_file(self):
        """Writes reweighting XML file needed for initial event generation."""
        with open('reweighting_needed_for_enhancedradfac.xml', 'w') as xmlfile:
            contents = dedent('''\
            <initrwgt>
            <weightgroup name='nominal' combine='None'>
            <weight id='0'>default</weight>
            </weightgroup>
            </initrwgt>
            ''')
            xmlfile.write(contents)
