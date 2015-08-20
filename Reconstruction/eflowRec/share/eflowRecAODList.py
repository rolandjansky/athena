from eflowRec.eflowRecFlags import jobproperties

eflowRecAODList = []

if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
    eflowRecAODList += [ "xAOD::PFOContainer#EOverPChargedParticleFlowObjects"]
    eflowRecAODList += [ "xAOD::PFOContainer#EOverPNeutralParticleFlowObjects"]

    eflowRecAODList += [ "xAOD::PFOAuxContainer#EOverPChargedParticleFlowObjectsAux."]
    eflowRecAODList += [ "xAOD::PFOAuxContainer#EOverPNeutralParticleFlowObjectsAux."]    
else:
    eflowRecAODList += [ "xAOD::PFOContainer#JetETMissChargedParticleFlowObjects"]
    eflowRecAODList += [ "xAOD::PFOContainer#JetETMissNeutralParticleFlowObjects"]

    eflowRecAODList += [ "xAOD::PFOAuxContainer#JetETMissChargedParticleFlowObjectsAux."]
    eflowRecAODList += [ "xAOD::PFOAuxContainer#JetETMissNeutralParticleFlowObjectsAux."]
