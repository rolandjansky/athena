from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

# ESD List
IsoESDList = []

if rec.doWriteAOD() or rec.doWriteESD():
    if rec.doMuon() or rec.doEgamma():
        IsoESDList += ["xAOD::EventShape#TopoClusterIsoCentralEventShape"]
        IsoESDList += ["xAOD::EventShapeAuxInfo#TopoClusterIsoCentralEventShapeAux."]
        IsoESDList += ["xAOD::EventShape#TopoClusterIsoForwardEventShape"]
        IsoESDList += ["xAOD::EventShapeAuxInfo#TopoClusterIsoForwardEventShapeAux."]
        if recAlgs.doEFlow():
            IsoESDList += ["xAOD::EventShape#ParticleFlowIsoCentralEventShape"]
            IsoESDList += ["xAOD::EventShapeAuxInfo#ParticleFlowIsoCentralEventShapeAux."]
            IsoESDList += ["xAOD::EventShape#ParticleFlowIsoForwardEventShape"]
            IsoESDList += ["xAOD::EventShapeAuxInfo#ParticleFlowIsoForwardEventShapeAux."]
            IsoESDList += ["xAOD::EventShape#NeutralParticleFlowIsoCentralEventShape"]
            IsoESDList += ["xAOD::EventShapeAuxInfo#NeutralParticleFlowIsoCentralEventShapeAux."]
            IsoESDList += ["xAOD::EventShape#NeutralParticleFlowIsoForwardEventShape"]
            IsoESDList += ["xAOD::EventShapeAuxInfo#NeutralParticleFlowIsoForwardEventShapeAux."]
# AOD List
IsoAODList = [i.replace('Aux.', 'Aux.-DensityArea.-DensitySigma')
              for i in IsoESDList]
