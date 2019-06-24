#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# File: TrkVertexSeedFinderTools/share/IndexedCrossDistancesSeedFinder_test.py
# Author: scott snyder <snyder@bnl.gov>
# Data: Jun, 2019
# Brief: Unit test for IndexedCrossDistancesSeedFinder.  Incomplete!
#


from __future__ import print_function


include ('TrkVertexSeedFinderTools/VertexSeedFinderTestCommon.py')


from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import \
    Trk__VertexSeedFinderTestAlg, Trk__IndexedCrossDistancesSeedFinder
finder = Trk__IndexedCrossDistancesSeedFinder ('IndexedCrossDistancesSeedFinder',
                                               OutputLevel = INFO)
testalg1 = Trk__VertexSeedFinderTestAlg ('testalg1',
                                         OutputLevel = VERBOSE,
                                         VertexSeedFinderTool = finder,
                                         PriVert = [0.3, 0.1],
                                         Expected1 = [8.48745, 8.54651, -6.48485],
                                         Expected2 = [3.44848, 3.26226, -7.06364],
                                         Expected3 = [-0.180517, -2.12594, 14.4642])
topSequence += testalg1
