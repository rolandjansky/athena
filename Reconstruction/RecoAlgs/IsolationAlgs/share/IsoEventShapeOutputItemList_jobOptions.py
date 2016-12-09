from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

#AOD list, also added to the ESD
IsoAODESList = []

if rec.doWriteAOD() or rec.doWriteESD():
    if rec.doMuon() or rec.doEgamma():
        IsoAODESList += ["xAOD::EventShape#TopoClusterIsoCentralEventShape"]
        IsoAODESList += ["xAOD::EventShapeAuxInfo#TopoClusterIsoCentralEventShapeAux."]
        IsoAODESList += ["xAOD::EventShape#TopoClusterIsoForwardEventShape"]
        IsoAODESList += ["xAOD::EventShapeAuxInfo#TopoClusterIsoForwardEventShapeAux."]
        IsoAODESList += ["xAOD::EventShape#TopoClusterIsoVeryForwardEventShape"]
        IsoAODESList += ["xAOD::EventShapeAuxInfo#TopoClusterIsoVeryForwardEventShapeAux."]
        if recAlgs.doEFlow():
            IsoAODESList += ["xAOD::EventShape#ParticleFlowIsoCentralEventShape"]
            IsoAODESList += ["xAOD::EventShapeAuxInfo#ParticleFlowIsoCentralEventShapeAux."]
            IsoAODESList += ["xAOD::EventShape#ParticleFlowIsoForwardEventShape"]
            IsoAODESList += ["xAOD::EventShapeAuxInfo#ParticleFlowIsoForwardEventShapeAux."]
            IsoAODESList += ["xAOD::EventShape#NeutralParticleFlowIsoCentralEventShape"]
            IsoAODESList += ["xAOD::EventShapeAuxInfo#NeutralParticleFlowIsoCentralEventShapeAux."]
            IsoAODESList += ["xAOD::EventShape#NeutralParticleFlowIsoForwardEventShape"]
            IsoAODESList += ["xAOD::EventShapeAuxInfo#NeutralParticleFlowIsoForwardEventShapeAux."]

## Needless
#IsoESDESList = []
#IsoESDESList += IsoAODESList

