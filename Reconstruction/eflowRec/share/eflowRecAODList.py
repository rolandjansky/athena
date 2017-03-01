from eflowRec.eflowRecFlags import jobproperties

eflowRecAODList = []

include("eflowRec/eflowRec_ESDandAOD_ExcludeList.py")

if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
    eflowRecAODList += [ "xAOD::PFOContainer#EOverPChargedParticleFlowObjects"]
    eflowRecAODList += [ "xAOD::PFOContainer#EOverPNeutralParticleFlowObjects"]

    eflowRecAODList += [ "xAOD::PFOAuxContainer#EOverPChargedParticleFlowObjectsAux."]
    eflowRecAODList += [ "xAOD::PFOAuxContainer#EOverPNeutralParticleFlowObjectsAux."+excludeList]    
else:
    eflowRecAODList += [ "xAOD::PFOContainer#JetETMissChargedParticleFlowObjects"]
    eflowRecAODList += [ "xAOD::PFOContainer#JetETMissNeutralParticleFlowObjects"]

    eflowRecAODList += [ "xAOD::PFOAuxContainer#JetETMissChargedParticleFlowObjectsAux."]
    eflowRecAODList += [ "xAOD::PFOAuxContainer#JetETMissNeutralParticleFlowObjectsAux."+excludeList]


    
