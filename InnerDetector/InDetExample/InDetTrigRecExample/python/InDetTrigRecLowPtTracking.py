# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" LowPt versions of the tracking algorithms

forwards to the NewTracking classes with an extra lowPt parameter set to True

"""

__author__ = "J. Masik"
__version__= "$Revision: 1.1 $"
__doc__    = "InDetTrigRecLowPtTracking.py"




from InDetTrigRecExample.InDetTrigConfigRecPreProcessing \
     import SiTrigSpacePointFinder_EF

class SiTrigSpacePointFinderLowPt_EF(SiTrigSpacePointFinder_EF):
  def __init__(self, name = "SiTrigSpacePointFinderLowPt_MinBias_EF", type="minBias"):
    SiTrigSpacePointFinder_EF.__init__(self, name, type, lowPt=True)


from InDetTrigRecExample.InDetTrigConfigRecNewTracking \
     import SiTrigTrackFinder_EF, TrigAmbiguitySolver_EF

class SiTrigTrackFinderLowPt_EF(SiTrigTrackFinder_EF):
  def __init__(self, name="SiTrigTrackFinderLowPt_MinBias_EF", type="minBias"):
    SiTrigTrackFinder_EF.__init__(self, name, type, lowPt=True)

class TrigAmbiguitySolverLowPt_EF(TrigAmbiguitySolver_EF):
  def __init__(self, name="TrigAmbiguitySolverLowPt_MinBias_EF", type="minBias"):
    TrigAmbiguitySolver_EF.__init__(self, name, type, lowPt=True)

