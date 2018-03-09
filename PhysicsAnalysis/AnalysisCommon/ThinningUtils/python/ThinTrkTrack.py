# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.Configured import Configured
from AthenaCommon.Logging import logging
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags

class ThinTrkTrack(Configured):    
    def configure(self):

        mlog = logging.getLogger ('TrkTrackPostExecStream.py::configure:')
        mlog.info('entering')

        try:
            from ThinningUtils.ThinningUtilsConf import ThinTrkTrackAlg
            theTrkTrackThinner = ThinTrkTrackAlg(
                "ThinTrkTrackAlg",
                doElectrons=True,
                doPhotons=True,
                doMuons=True,
                MuonsKey="Muons",
                ElectronsKey="Electrons",
                PhotonsKey="Photons",
                CombinedMuonsTrackKey="CombinedMuonTracks",
                GSFTrackKey="GSFTracks")                
            
            from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR           
            #theTrkTrackThinner.OutputLevel=DEBUG            
            print theTrkTrackThinner

        except Exception:
            import traceback
            mlog.error("could not get handle to ThinTrkTrackAlg")
            print traceback.format_exc()
            return False 

        mlog.info("now adding to topSequence")
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTrkTrackThinner
        mlog.info("Done adding to topSequence")

        return True
