# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from RecExConfig.Configured import Configured
from AthenaCommon.Logging import logging
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags

class ThinTrkTrack(Configured):    
    def configure(self):

        mlog = logging.getLogger ('TrkTrackPostExecStream.py::configure:')
        mlog.info('entering')

        try:
            from RecExConfig.ObjKeyStore import cfgKeyStore
            from ThinningUtils.ThinningUtilsConf import ThinTrkTrackAlg
            theTrkTrackThinner = ThinTrkTrackAlg(
                "ThinTrkTrackAlg",
                doElectrons=True,
                doPhotons=True,
                doMuons=cfgKeyStore.isInInput ('TrackCollection',
                                               'CombinedMuonTracks'),
                MuonsKey="Muons",
                ElectronsKey="Electrons",
                PhotonsKey="Photons",
                CombinedMuonsTrackKey="CombinedMuonTracks",
                GSFTrackKey="GSFTracks",
                StreamName='StreamAOD')
            
            from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR           
            #theTrkTrackThinner.OutputLevel=DEBUG            
            print (theTrkTrackThinner)

        except Exception:
            import traceback
            mlog.error("could not get handle to ThinTrkTrackAlg")
            traceback.print_exc()
            return False 

        mlog.info("now adding to topSequence")
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTrkTrackThinner
        mlog.info("Done adding to topSequence")

        return True
