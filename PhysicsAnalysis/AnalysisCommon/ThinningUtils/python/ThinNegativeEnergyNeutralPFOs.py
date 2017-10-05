# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.Configured import Configured
from AthenaCommon.Logging import logging

class ThinNegativeEnergyNeutralPFOs(Configured):
    
    def configure(self):
        mlog = logging.getLogger ('ThinNegativeEnergyNeutralPFOs.py::configure:')
        mlog.info('entering')
        try:
            from ThinningUtils.ThinningUtilsConf import ThinNegativeEnergyNeutralPFOsAlg
            theNegativeEnergyNeutralPFOsThinner = ThinNegativeEnergyNeutralPFOsAlg(
                "ThinNegativeEnergyNeutralPFOsAlg",
                ThinNegativeEnergyNeutralPFOs = True
            )
            print theNegativeEnergyNeutralPFOsThinner

            CHSnPFOsThinAlg = ThinNegativeEnergyNeutralPFOsAlg(
                "ThinNegativeEnergyCHSNeutralPFOsAlg",
                NeutralPFOsKey="CHSNeutralParticleFlowObjects",
                ThinNegativeEnergyNeutralPFOs = True
                )
            print CHSnPFOsThinAlg

        except Exception:
            mlog.error("could not get handle to ThinNegativeEnergyNeutralPFOsAlg")
            print traceback.format_exc()
            return False 
        mlog.info("now adding to topSequence")
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theNegativeEnergyNeutralPFOsThinner
        topSequence += CHSnPFOsThinAlg
        return True
