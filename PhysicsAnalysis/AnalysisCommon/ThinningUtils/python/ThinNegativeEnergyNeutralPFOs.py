# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

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
                ThinNegativeEnergyNeutralPFOs = True,
                StreamName = 'StreamAOD'
            )
            from RecExConfig.ObjKeyStore import cfgKeyStore
            if cfgKeyStore.isInInput('xAOD::FlowElementContainer', 'JetETMissNeutralFlowElements',):
                theNegativeEnergyNeutralPFOsThinner.NeutralPFOsFEKey = "JetETMissNeutralFlowElements"
            print (theNegativeEnergyNeutralPFOsThinner)

            CHSnPFOsThinAlg = None


            from JetRec.JetRecFlags import jetFlags
            if (jetFlags.useTracks or
                cfgKeyStore.isInInput ('xAOD::PFOContainer',
                                       'CHSNeutralParticleFlowObjects')):
                CHSnPFOsThinAlg = ThinNegativeEnergyNeutralPFOsAlg(
                    "ThinNegativeEnergyCHSNeutralPFOsAlg",
                    NeutralPFOsKey="CHSNeutralParticleFlowObjects",
                    ThinNegativeEnergyNeutralPFOs = True,
                    StreamName = 'StreamAOD'
                    )
                print (CHSnPFOsThinAlg)

        except Exception:
            mlog.error("could not get handle to ThinNegativeEnergyNeutralPFOsAlg")
            import traceback
            traceback.print_exc()
            return False 
        mlog.info("now adding to topSequence")
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theNegativeEnergyNeutralPFOsThinner
        if CHSnPFOsThinAlg:
            topSequence += CHSnPFOsThinAlg
        return True
