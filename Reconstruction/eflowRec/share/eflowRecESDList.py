from eflowRec.eflowRecFlags import jobproperties

eflowRecESDList = []

include("eflowRec/eflowRec_ESDandAOD_ExcludeList.py")

if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
    eflowRecESDList += [ "xAOD::PFOContainer#EOverPChargedParticleFlowObjects"]
    eflowRecESDList += [ "xAOD::PFOContainer#EOverPNeutralParticleFlowObjects"]

    eflowRecESDList += [ "xAOD::PFOAuxContainer#EOverPChargedParticleFlowObjectsAux."]
    eflowRecESDList += [ "xAOD::PFOAuxContainer#EOverPNeutralParticleFlowObjectsAux."+excludeList]
else:
    eflowRecESDList += [ "xAOD::PFOContainer#JetETMissChargedParticleFlowObjects"]
    eflowRecESDList += [ "xAOD::PFOContainer#JetETMissNeutralParticleFlowObjects"]

    eflowRecESDList += [ "xAOD::PFOAuxContainer#JetETMissChargedParticleFlowObjectsAux."]
    eflowRecESDList += [ "xAOD::PFOAuxContainer#JetETMissNeutralParticleFlowObjectsAux."+excludeList]

if True == jobproperties.eflowRecFlags.provideShowerSubtractedClusters:
    eflowRecESDList += [ "xAOD::CaloClusterContainer#PFCaloCluster"]
    eflowRecESDList += [ "xAOD::CaloClusterAuxContainer#PFCaloClusterAux."]

#Add new flow elements
eflowRecESDList += [ "xAOD::FlowElementContainer#JetETMissChargedFlowElements"]
eflowRecESDList += [ "xAOD::FlowElementAuxContainer#JetETMissChargedFlowElementsAux."]

eflowRecESDList += [ "xAOD::FlowElementContainer#JetETMissNeutralFlowElements"]
eflowRecESDList += [ "xAOD::FlowElementAuxContainer#JetETMissNeutralFlowElementsAux."+FlowElementNeutralExcludeList]

eflowRecESDList += [ "xAOD::FlowElementContainer#JetETMissLCNeutralFlowElements"]
eflowRecESDList += [ "xAOD::ShallowAuxContainer#JetETMissLCNeutralFlowElementsAux."]