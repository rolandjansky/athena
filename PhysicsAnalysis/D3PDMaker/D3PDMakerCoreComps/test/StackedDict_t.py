# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: StackedDict_t.py 527109 2012-11-21 21:08:05Z ssnyder $
#
# @file D3PDMakerCoreComps/test/StackedDict_t.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2012
# @brief Run tests for StackedDict.
#


from PyUtils import coverage
c = coverage.Coverage ('D3PDMakerCoreComps.StackedDict')
c.doctest_cover ()
