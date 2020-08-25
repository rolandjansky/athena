#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
#
# File: DataModelRunTests/share/loadReadDicts.py
# Author: snyder@bnl.gov
# Date: Oct 2018
# Purpose: Helper to load dictionaries for DataModelTests.
#

# Arrange to get dictionaries loaded for read tests.
# Do this as an algorithm so we can defer it to initialize().
# In some cases, loading DSOs during initial python processing
# can cause component loading to fail.


from AthenaPython.PyAthenaComps import Alg, StatusCode
class LoadReadDicts (Alg):
    def initialize (self):
        import ROOT
        ROOT.gROOT.SetBatch(True)
        import cppyy
        cppyy.load_library("libDataModelTestDataCommonDict")
        cppyy.load_library("libDataModelTestDataReadDict")
        ROOT.DMTest.B
        ROOT.gROOT.GetClass('DMTest::HAuxContainer_v1')
        ROOT.gROOT.GetClass('DataVector<DMTest::H_v1>')
        ROOT.gROOT.GetClass('DMTest::HView_v1')
        ROOT.DMTest.setConverterLibrary ('libDataModelTestDataReadCnvPoolCnv.so')
        ROOT.DMTest.setTrigConverterLibrary ('libDataModelTestDataReadSerCnv.so')
        return StatusCode.Success

topSequence += LoadReadDicts ('LoadReadDicts')
