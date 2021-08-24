#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
#
# File: TrkVertexSeedFinderTools/share/CrossDistancesSeedFinder_test.py
# Author: scott snyder <snyder@bnl.gov>
# Data: Jun, 2019
# Brief: Unit test for CrossDistancesSeedFinder.  Incomplete!
#


from __future__ import print_function


include ('TrkVertexSeedFinderTools/VertexSeedFinderTestCommon.py')

from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import \
    Trk__VertexSeedFinderTestAlg, Trk__CrossDistancesSeedFinder
finder = Trk__CrossDistancesSeedFinder ('CrossDistancesSeedFinder', OutputLevel = INFO)
testalg1 = Trk__VertexSeedFinderTestAlg ('testalg1',
                                         OutputLevel = VERBOSE,
                                         VertexSeedFinderTool = finder,
                                         Expected1 = [8.58978, 8.67507, -6.4581],
                                         Expected2 = [3.44328, 3.25688, -7.88419],
                                         Expected3 = [1.23398, -1.18171, 11.9419])
topSequence += testalg1
