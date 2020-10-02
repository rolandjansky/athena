#! /usr/bin/env python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import unittest
import ROOT
ROOT.gROOT.SetBatch()

import math, string, os, sys
import subprocess

## Main test class
class TestSUSYTools(unittest.TestCase):

    #Files and commands
#    theSample = os.environ['ASG_TEST_FILE_MC']

    PRW_files = []
    PRW_ilcs  = []

    #guess the MC campaign for the prw file, if necessary
    PRW_files.append('/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/mc16_13TeV/pileup_mc16e_dsid410470_FS.root')
    PRW_ilcs.append('/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190219/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root')

    def vec_str(self,iterable):
        vs = ROOT.std.vector( ROOT.std.string )()
        for item in iterable:
            vs.push_back(item)
        return vs

    @classmethod
    def setUpClass(cls):
        cls.event = ROOT.xAOD.TEvent()
        #cls.tool_st = ROOT.ST.SUSYObjDef_xAOD('ST_what')

    #Default Initialization
    def test_initialization_data(self):
        st = ROOT.ST.SUSYObjDef_xAOD('ST_data')
        st.setDataSource(0)
        self.assertTrue(st.setProperty('PRWConfigFiles', self.vec_str(self.PRW_files)).isSuccess())
        self.assertTrue(st.setProperty('PRWLumiCalcFiles', self.vec_str(self.PRW_ilcs)).isSuccess())
        self.assertTrue(st.initialize().isSuccess())
        del st

    def test_initialization_mc_full(self):
        st = ROOT.ST.SUSYObjDef_xAOD('ST_full')
        st.setDataSource(1)        
        self.assertTrue(st.setProperty('PRWConfigFiles', self.vec_str(self.PRW_files)).isSuccess())
        self.assertTrue(st.setProperty('PRWLumiCalcFiles', self.vec_str(self.PRW_ilcs)).isSuccess())
        self.assertTrue(st.initialize().isSuccess())
        del st

    def test_initialization_mc_fast(self):
        st = ROOT.ST.SUSYObjDef_xAOD('ST_fast')
        st.setDataSource(2)        
        self.assertTrue(st.setProperty('PRWConfigFiles', self.vec_str(self.PRW_files)).isSuccess())
        self.assertTrue(st.setProperty('PRWLumiCalcFiles', self.vec_str(self.PRW_ilcs)).isSuccess())
        self.assertTrue(st.initialize().isSuccess())
        del st


if __name__ == '__main__':

    ## Load pkgs
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")

    ## Call the tests
    #    unittest.main()
    suite = unittest.TestLoader().loadTestsFromTestCase(TestSUSYTools)
    result = unittest.TextTestRunner(verbosity=0).run(suite).wasSuccessful()
    sys.exit( not result ) #convert True -> 0 


