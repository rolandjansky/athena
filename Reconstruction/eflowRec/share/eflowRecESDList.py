from eflowRec.eflowRecFlags import jobproperties

eflowRecESDList = []

if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
    eflowRecESDList += [ "xAOD::PFOContainer#EOverPChargedParticleFlowObjects"]
    eflowRecESDList += [ "xAOD::PFOContainer#EOverPNeutralParticleFlowObjects"]

    eflowRecESDList += [ "xAOD::PFOAuxContainer#EOverPChargedParticleFlowObjectsAux."]
    eflowRecESDList += [ "xAOD::PFOAuxContainer#EOverPNeutralParticleFlowObjectsAux."]
else:
    eflowRecESDList += [ "xAOD::PFOContainer#JetETMissChargedParticleFlowObjects"]
    eflowRecESDList += [ "xAOD::PFOContainer#JetETMissNeutralParticleFlowObjects"]

    eflowRecESDList += [ "xAOD::PFOAuxContainer#JetETMissChargedParticleFlowObjectsAux."]
    eflowRecESDList += [ "xAOD::PFOAuxContainer#JetETMissNeutralParticleFlowObjectsAux."]

