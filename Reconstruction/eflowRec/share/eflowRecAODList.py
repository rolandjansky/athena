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

if True == jobproperties.eflowRecFlags.provideShowerSubtractedClusters:
    eflowRecAODList += [ "xAOD::CaloClusterContainer#PFCaloCluster"]
    eflowRecAODList += [ "xAOD::CaloClusterAuxContainer#PFCaloClusterAux."]
    
#Add new flow elements
eflowRecAODList += [ "xAOD::FlowElementContainer#JetETMissChargedFlowElements"]
eflowRecAODList += [ "xAOD::FlowElementAuxContainer#JetETMissChargedFlowElementsAux."]

eflowRecAODList += [ "xAOD::FlowElementContainer#JetETMissNeutralFlowElements"]
eflowRecAODList += [ "xAOD::FlowElementAuxContainer#JetETMissNeutralFlowElementsAux."+FlowElementNeutralExcludeList]

eflowRecAODList += [ "xAOD::FlowElementContainer#JetETMissLCNeutralFlowElements"]
eflowRecAODList += [ "xAOD::ShallowAuxContainer#JetETMissLCNeutralFlowElementsAux."]