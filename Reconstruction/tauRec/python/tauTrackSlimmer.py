# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# 
# AUTHOR: A.Kaczmarska (using egamma example)
# CREATED: July 12 2007
#

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
import traceback
import EventKernel.ParticleDataType

from RecExConfig.Configured import Configured

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaServices.Configurables import ThinningSvc
if not hasattr(svcMgr, 'ThinningSvc'):
  svcMgr += ThinningSvc(OutputLevel=INFO)
svcMgr.ThinningSvc.Streams += ['StreamAOD']


class tauTrackSlimmer ( Configured ) :
    #_outputType = "TrackCollection"
    #_outputKey = "Tracks"

    #_output = { _outputType: _outputKey }

    def configure(self):
        mlog = logging.getLogger ('tauTrackSlimmer.py::configure:')
        mlog.info('entering')
    

        # configure TauTrackSlimmer here:
        try:
            from tauRec.tauRecConf import TauTrackSlimmer
            theTauTrackSlimmer = TauTrackSlimmer(
                "tauTrackSlimmer",
                TauContainer="TauJets",
                FilterTaus=True,
                maxNTrack = 4,
                maxCharge = 2,
                maxEmRadius = 0.2,
                maxIsoFrac = 0.5
                )
        except Exception:
            mlog.error("could not get handle to tauTrackSlimmer")
            print traceback.format_exc()
            return False
        self._TauBuilderHandle = theTauTrackSlimmer
        
        # take care of the slimming Tool
        try:
            from TrkTrackSlimmingTool.TrkTrackSlimmingToolConf import Trk__TrackSlimmingTool as ConfigurableTrackSlimmingTool
            theTrackSlimmer=ConfigurableTrackSlimmingTool(
                name = "tauTrackSlimmerTool",
                KeepParameters = True,
                KeepOutliers   = True
                )
        except Exception:
            mlog.error("could not get handle to  tauTrackSlimmer")
            print traceback.format_exc()
            return False
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theTrackSlimmer

        #output:
        from RecExConfig.ObjKeyStore import objKeyStore
        # output to AOD (cluster container only!):
        #DR objKeyStore.addStreamAOD(self.outputType(),self.outputKey())

        # add to topsequence 
        mlog.info("now adding to topSequence")

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTauTrackSlimmer


        return True

##     def tauTrackSlimmerHandle(self):
##         return self._tauTrackSlimmerHandle

##     def outputType(self):
##         return self._outputType

##     def outputKey(self) :
##         return self._outputKey
    
