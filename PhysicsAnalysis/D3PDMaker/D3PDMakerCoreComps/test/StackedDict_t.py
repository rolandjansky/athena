# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# @file D3PDMakerCoreComps/test/StackedDict_t.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2012
# @brief Run tests for StackedDict.
#


from PyUtils import coverage
c = coverage.Coverage ('D3PDMakerCoreComps.StackedDict')
print ('D3ODMakerCoreComps/StackedDict_t')
c.doctest_cover ()
