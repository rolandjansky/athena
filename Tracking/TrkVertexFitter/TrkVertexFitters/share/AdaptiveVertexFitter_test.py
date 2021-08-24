#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
#
# File: TrkVertexFitters/share/AdaptiveVertexFitter_test.py
# Author: scott snyder <snyder@bnl.gov>
# Data: Jul, 2019
# Brief: Unit test for AdaptiveVertexFitter.  Incomplete!
#


from __future__ import print_function


from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.ID_setOn()
DetFlags.detdescr.TRT_setOff()

RunNumber = 284500

import sys
import string
import ROOT
import math
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
ServiceMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]
from AtlasGeoModel import InDetGM

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc().GlobalTag = 'OFLCOND-RUN12-SDR-35'

import MagFieldServices.SetupField

from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

theApp.EvtMax=2

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
eialg = xAODMaker__EventInfoCnvAlg ()
topSequence += eialg



# Suppress useless GeoModelSvc messages.
from AthenaCommon import Constants
GeoModelSvc().OutputLevel=Constants.WARNING

from InDetRecExample.TrackingCommon import getInDetExtrapolator, getInDetFullLinearizedTrackFactory

from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator
InDetImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name              = "InDetImpactPoint3dEstimator",
                                                          Extrapolator      = getInDetExtrapolator())
ToolSvc += InDetImpactPoint3dEstimator

from TrkVertexFitters.TrkVertexFittersConf import \
    Trk__AdaptiveVertexFitterTestAlg, Trk__AdaptiveVertexFitter
fitter = Trk__AdaptiveVertexFitter ('AdaptiveVertexFitter',
                                    ImpactPoint3dEstimator = InDetImpactPoint3dEstimator,
                                    LinearizedTrackFactory = getInDetFullLinearizedTrackFactory(),
                                    OutputLevel = INFO)
testalg1 = Trk__AdaptiveVertexFitterTestAlg ('testalg1',
                                             OutputLevel = VERBOSE,
                                             Tool = fitter)
topSequence += testalg1
