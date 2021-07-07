from eflowRec.eflowRecFlags import jobproperties

eflowRecAODList = []

include("eflowRec/eflowRec_ESDandAOD_ExcludeList.py")

if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
    eflowRecAODList += [ "xAOD::FlowElementContainer#EOverPChargedParticleFlowObjects"]
    eflowRecAODList += [ "xAOD::FlowElementContainer#EOverPNeutralParticleFlowObjects"]

    eflowRecAODList += [ "xAOD::FlowElementAuxContainer#EOverPChargedParticleFlowObjectsAux."]
    eflowRecAODList += [ "xAOD::FlowElementAuxContainer#EOverPNeutralParticleFlowObjectsAux."+excludeList]    
else:
    eflowRecAODList += [ "xAOD::FlowElementContainer#JetETMissChargedParticleFlowObjects"]
    eflowRecAODList += [ "xAOD::FlowElementAuxContainer#JetETMissChargedParticleFlowObjectsAux."]

    eflowRecAODList += [ "xAOD::FlowElementContainer#JetETMissNeutralParticleFlowObjects"]
    eflowRecAODList += [ "xAOD::FlowElementAuxContainer#JetETMissNeutralParticleFlowObjectsAux."+FlowElementNeutralExcludeList]

    eflowRecAODList += [ "xAOD::FlowElementContainer#JetETMissLCNeutralParticleFlowObjects"]
    eflowRecAODList += [ "xAOD::ShallowAuxContainer#JetETMissLCNeutralParticleFlowObjectsAux."]

if True == jobproperties.eflowRecFlags.provideShowerSubtractedClusters:
    eflowRecAODList += [ "xAOD::CaloClusterContainer#PFCaloCluster"]
    eflowRecAODList += [ "xAOD::CaloClusterAuxContainer#PFCaloClusterAux."]
    
