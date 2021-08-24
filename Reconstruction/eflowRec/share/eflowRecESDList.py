from eflowRec.eflowRecFlags import jobproperties

eflowRecESDList = []

include("eflowRec/eflowRec_ESDandAOD_ExcludeList.py")

if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
    eflowRecESDList += [ "xAOD::FlowElementContainer#EOverPChargedParticleFlowObjects"]
    eflowRecESDList += [ "xAOD::FlowElementContainer#EOverPNeutralParticleFlowObjects"]

    eflowRecESDList += [ "xAOD::FlowElementAuxContainer#EOverPChargedParticleFlowObjectsAux."]
    eflowRecESDList += [ "xAOD::FlowElementAuxContainer#EOverPNeutralParticleFlowObjectsAux."+excludeList]
else:
    eflowRecESDList += [ "xAOD::FlowElementContainer#JetETMissChargedParticleFlowObjects"]
    eflowRecESDList += [ "xAOD::FlowElementAuxContainer#JetETMissChargedParticleFlowObjectsAux."]

    eflowRecESDList += [ "xAOD::FlowElementContainer#JetETMissNeutralParticleFlowObjects"]
    eflowRecESDList += [ "xAOD::FlowElementAuxContainer#JetETMissNeutralParticleFlowObjectsAux."+FlowElementNeutralExcludeList]

    eflowRecESDList += [ "xAOD::FlowElementContainer#JetETMissLCNeutralParticleFlowObjects"]
    eflowRecESDList += [ "xAOD::ShallowAuxContainer#JetETMissLCNeutralParticleFlowObjectsAux."]

if True == jobproperties.eflowRecFlags.provideShowerSubtractedClusters:
    eflowRecESDList += [ "xAOD::CaloClusterContainer#PFCaloCluster"]
    eflowRecESDList += [ "xAOD::CaloClusterAuxContainer#PFCaloClusterAux."]