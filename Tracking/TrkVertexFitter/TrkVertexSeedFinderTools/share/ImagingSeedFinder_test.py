#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# File: TrkVertexSeedFinderTools/share/ImagingSeedFinder_test.py
# Author: scott snyder <snyder@bnl.gov>
# Data: Jun, 2019
# Brief: Unit test for ImagingSeedFinder.  Incomplete!
#


from __future__ import print_function


include ('TrkVertexSeedFinderTools/VertexSeedFinderTestCommon.py')


from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import \
    Trk__VertexSeedFinderTestAlg, Trk__ImagingSeedFinder
finder = Trk__ImagingSeedFinder ('ImagingSeedFinder', OutputLevel = INFO)
testalg1 = Trk__VertexSeedFinderTestAlg ('testalg1',
                                         OutputLevel = VERBOSE,
                                         VertexSeedFinderTool = finder,
                                         Expected3 = [ 0.933924,  -0.748513,  7.17455,
                                                       -0.741989, -0.419237, -2.64877,
                                                        1.73866,  -1.80412, 11.608,
                                                       -0.124825,  0.14802, 13.4766,
                                                        0.817835,  0.747413, 11.0648])
topSequence += testalg1
