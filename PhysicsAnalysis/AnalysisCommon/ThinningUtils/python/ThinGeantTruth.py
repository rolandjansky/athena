# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
                ThinGeantTruth = True
            )
            from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
            if commonGeoFlags.Run()=="RUN4":
                theGeantTruthThinner.EtaMaxEGammaTruth = 4.1
                theGeantTruthThinner.FwdElectronsKey = "ForwardElectrons"
            print theGeantTruthThinner
        except Exception:
            mlog.error("could not get handle to ThinGeantTruthAlg")
            print traceback.format_exc()
            return False 
        mlog.info("now adding to topSequence")
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theGeantTruthThinner
        return True
