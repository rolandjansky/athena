# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.Configured import Configured
from AthenaCommon.Logging import logging

class ThinNegativeEnergyCaloClusters(Configured):
    
    def configure(self):
        mlog = logging.getLogger ('ThinNegativeEnergyCaloClusters.py::configure:')
        mlog.info('entering')
        try:
            from ThinningUtils.ThinningUtilsConf import ThinNegativeEnergyCaloClustersAlg
            theNegativeEnergyCaloClustersThinner = ThinNegativeEnergyCaloClustersAlg(
                "ThinNegativeEnergyCaloClustersAlg",
                ThinNegativeEnergyCaloClusters = True
            )
            print theNegativeEnergyCaloClustersThinner
        except Exception:
            mlog.error("could not get handle to ThinNegativeEnergyCaloClustersAlg")
            print traceback.format_exc()
            return False 
        mlog.info("now adding to topSequence")
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theNegativeEnergyCaloClustersThinner
        return True
