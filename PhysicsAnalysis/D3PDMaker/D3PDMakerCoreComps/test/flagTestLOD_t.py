# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# @file D3PDMakerCoreComps/test/flagTestLOD_t.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2012
# @brief Run tests for flagTestLOD.
#


from PyUtils import coverage
print ('D3PDMakerCoreComps/flagTestLOD_t')
c = coverage.Coverage ('D3PDMakerCoreComps.flagTestLOD')
c.doctest_cover ()
