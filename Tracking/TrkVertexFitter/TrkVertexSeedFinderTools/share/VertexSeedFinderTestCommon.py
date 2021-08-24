#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
#
# File: TrkVertexSeedFinderTools/share/VertexSeedFinderTestCommon.py
# Author: scott snyder <snyder@bnl.gov>
# Data: Jun, 2019
# Brief: Common configuration for unit tests.
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

from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlg import ConfiguredTrackingGeometryCondAlg
TrkGeoCondAlg = ConfiguredTrackingGeometryCondAlg('AtlasTrackingGeometryCondAlg')
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
condSeq+= TrkGeoCondAlg


# Suppress useless GeoModelSvc messages.
from AthenaCommon import Constants
GeoModelSvc().OutputLevel=Constants.WARNING
