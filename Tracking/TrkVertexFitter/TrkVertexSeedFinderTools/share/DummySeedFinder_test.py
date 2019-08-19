#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# File: TrkVertexSeedFinderTools/share/DummySeedFinder_test.py
# Author: scott snyder <snyder@bnl.gov>
# Data: Jun, 2019
# Brief: Unit test for DummySeedFinder.  Incomplete!
#


from __future__ import print_function


include ('TrkVertexSeedFinderTools/VertexSeedFinderTestCommon.py')


from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import \
    Trk__VertexSeedFinderTestAlg, Trk__DummySeedFinder
finder = Trk__DummySeedFinder ('DummySeedFinder', OutputLevel = INFO)
testalg1 = Trk__VertexSeedFinderTestAlg ('testalg1',
                                         OutputLevel = VERBOSE,
                                         VertexSeedFinderTool = finder,
                                         Expected1 = [0, 0, 0],
                                         Expected2 = [0, 0, 0],
                                         Expected3 = [0, 0, 0])
topSequence += testalg1
