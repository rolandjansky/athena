# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: flagTestLOD_t.py 527307 2012-11-22 17:18:42Z ssnyder $
#
# @file D3PDMakerCoreComps/test/flagTestLOD_t.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2012
# @brief Run tests for flagTestLOD.
#


from PyUtils import coverage
c = coverage.Coverage ('D3PDMakerCoreComps.flagTestLOD')
c.doctest_cover ()
