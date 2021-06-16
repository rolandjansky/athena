# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
Read an xAOD with track Particle
"""

# Set input AOD files
if not "inputFiles" in dir():
    inputFiles = ["myESD.q221.pool.root"]

import AthenaPoolCnvSvc.ReadAthenaPool
from PoolSvc.PoolSvcConf import PoolSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import VERBOSE, INFO

svcMgr += PoolSvc()
PoolSvc = svcMgr.PoolSvc
EventSelector = svcMgr.EventSelector
EventSelector.InputCollections = inputFiles

############### The output collection #######################

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "val DATAFILE='TrackComparison.root' TYPE='ROOT' OPT='RECREATE'" ]

##############################################################


# Set up topSequence and condSeq
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
      
from ActsTrkFitting.ActsTrkFittingConf import ActsCompareTrackAlg     
compareTrack = ActsCompareTrackAlg(name = "ActsCompareTrackAlg",
                                   ATLASTrackName = "ResolvedTracks_TrackParticle",
                                   ActsTrackName = "ReFitted_Tracks_TrackParticle")

compareTrack.OutputLevel = VERBOSE

topSequence += compareTrack

theApp.EvtMax = -1
