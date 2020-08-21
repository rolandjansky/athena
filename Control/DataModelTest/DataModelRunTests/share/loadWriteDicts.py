#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
#
# File: DataModelRunTests/share/loadWriteDicts.py
# Author: snyder@bnl.gov
# Date: Oct 2018
# Purpose: Helper to load dictionaries for DataModelTests.
#

# Arrange to get dictionaries loaded for write tests.
# Do this as an algorithm so we can defer it to initialize().
# In some cases, loading DSOs during initial python processing
# can cause component loading to fail.


from AthenaPython.PyAthenaComps import Alg, StatusCode
class LoadWriteDicts (Alg):
    def initialize (self):
        import ROOT
        import cppyy
        cppyy.load_library("libDataModelTestDataCommonDict")
        cppyy.load_library("libDataModelTestDataWriteDict")
        cppyy.load_library("libDataModelTestDataWriteCnvDict")
        ROOT.DMTest.B
        ROOT.DMTest.setConverterLibrary ('libDataModelTestDataWriteCnvPoolCnv.so')
        ROOT.DMTest.setTrigConverterLibrary ('libDataModelTestDataWriteSerCnv.so')
        return StatusCode.Success

topSequence += LoadWriteDicts ('LoadWriteDicts')

