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
                                         Expected3 = [-0.180517, -2.12594, 14.4642],
                                         Expected1PhiModes = [0.525193, 0.785398, 0.967545],
                                         Expected1RModes = [3.61433, 12.4693, 7.05322],
                                         Expected1ZModes = [-7.88629, -6.5, -5.61236],
                                         Expected1Weights = [0.137994, 6.88709, 0.341206],
                                         Expected1Indices = [0, 1, 2],
                                         Expected1CorrDist = [-9.9, -9.9],
                                         )
topSequence += testalg1
