#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# File: TrkVertexSeedFinderTools/share/MCTrueSeedFinder_test.py
# Author: scott snyder <snyder@bnl.gov>
# Data: Jun, 2019
# Brief: Unit test for MCTrueSeedFinder.  Incomplete!
#


from __future__ import print_function


include ('TrkVertexSeedFinderTools/VertexSeedFinderTestCommon.py')


from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import \
    Trk__VertexSeedFinderTestAlg, Trk__MCTrueSeedFinder
finder = Trk__MCTrueSeedFinder ('MCTrueSeedFinder', OutputLevel = INFO)
testalg1 = Trk__VertexSeedFinderTestAlg ('testalg1',
                                         OutputLevel = VERBOSE,
                                         VertexSeedFinderTool = finder,
                                         McEventCollectionKey = 'G4Truth',
                                         Expected3 = [1, 2, 12,
                                                      0.3, -0.7, -3,
                                                      0.6, 0.2, 7])
topSequence += testalg1
