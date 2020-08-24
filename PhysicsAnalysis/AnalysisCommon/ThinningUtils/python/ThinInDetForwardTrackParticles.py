# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from RecExConfig.Configured import Configured
from AthenaCommon.Logging import logging

class ThinInDetForwardTrackParticles(Configured):
    
    def configure(self):
        mlog = logging.getLogger ('ThinInDetForwardTrackParticles.py::configure:')
        from InDetRecExample.InDetJobProperties import InDetFlags
        from InDetRecExample.InDetKeys import InDetKeys
        from RecExConfig.InputFilePeeker import inputFileSummary

        have_InDetForwardParticles = (inputFileSummary['eventdata_items']
                                      and  any(InDetKeys.xAODForwardTrackParticleContainer() in elements
                                             for elements in inputFileSummary['eventdata_items']))
        if not have_InDetForwardParticles and ( not InDetFlags.doForwardTracks() or not InDetFlags.doParticleCreation() ) :
            mlog.error("Not attempting to thin InDetForwardParticles, because the container %s does not seem to be available"
                       % (InDetKeys.xAODForwardTrackParticleContainer()))
            return True

        mlog.info('entering')
        try:
            from ThinningUtils.ThinningUtilsConf import ThinInDetForwardTrackParticlesAlg
            theInDetFwdThinner = ThinInDetForwardTrackParticlesAlg(
                "ThinInDetForwardTrackParticlesAlg",
                ThinInDetForwardTrackParticles = True,
                StreamName = 'StreamAOD'
            )
            print (theInDetFwdThinner)
        except Exception:
            import traceback
            mlog.error("could not get handle to ThinInDetForwardTrackParticlesAlg")
            traceback.print_exc()
            return False
        mlog.info("now adding to topSequence")
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theInDetFwdThinner
        return True
