#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# File: TrkVertexSeedFinderTools/share/TrackDensitySeedFinder_test.py
# Author: scott snyder <snyder@bnl.gov>
# Data: Jun, 2019
# Brief: Unit test for TrackDensitySeedFinder.  Incomplete!
#


from __future__ import print_function


include ('TrkVertexSeedFinderTools/VertexSeedFinderTestCommon.py')


from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import \
    Trk__VertexSeedFinderTestAlg, Trk__TrackDensitySeedFinder
finder = Trk__TrackDensitySeedFinder ('TrackDensitySeedFinder', OutputLevel = INFO)
testalg1 = Trk__VertexSeedFinderTestAlg ('testalg1',
                                         OutputLevel = VERBOSE,
                                         VertexSeedFinderTool = finder,
                                         Expected1 = [0, 0, -7.01305],
                                         Expected2 = [1.7, 1.3, -13.013],
                                         Expected3 = [0, 0, 11.9761])
topSequence += testalg1
