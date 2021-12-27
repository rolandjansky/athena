# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from RecExConfig.Configured import Configured
from AthenaCommon.Logging import logging

class ThinGeantTruth(Configured):
    
    def configure(self):
        mlog = logging.getLogger ('ThinGeantTruth.py::configure:')
        mlog.info('entering')
        try:
            from ThinningUtils.ThinningUtilsConf import ThinGeantTruthAlg
            theGeantTruthThinner = ThinGeantTruthAlg(
                "ThinGeantTruthAlg",
                StreamName = 'StreamAOD'
            )
            print (theGeantTruthThinner)
        except Exception:
            import traceback
            mlog.error("could not get handle to ThinGeantTruthAlg")
            traceback.print_exc()
            return False 
        mlog.info("now adding to topSequence")
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theGeantTruthThinner
        return True
