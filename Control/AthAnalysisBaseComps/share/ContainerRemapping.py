include.block ("RecExCommon/ContainerRemapping.py")

from AthenaCommon.AppMgr import ServiceMgr

# Instantiate the address remapping service:
if not hasattr( ServiceMgr, "AddressRemappingSvc" ):
    ServiceMgr += CfgMgr.AddressRemappingSvc()
    pass
if not hasattr( ServiceMgr, "ProxyProviderSvc" ):
    ServiceMgr += CfgMgr.ProxyProviderSvc()
    pass
ServiceMgr.ProxyProviderSvc.ProviderNames += [ "AddressRemappingSvc" ]

# Declare the name conversion rules:
ServiceMgr.AddressRemappingSvc.TypeKeyOverwriteMaps += [
    "xAOD::ElectronContainer#ElectronCollection->"
    "xAOD::ElectronContainer#Electrons",
    "xAOD::ElectronAuxContainer#ElectronCollectionAux.->"
    "xAOD::ElectronAuxContainer#ElectronsAux.",
    "xAOD::ElectronContainer#FwdElectrons->"
    "xAOD::ElectronContainer#ForwardElectrons",
    "xAOD::ElectronAuxContainer#FwdElectronsAux.->"
    "xAOD::ElectronAuxContainer#ForwardElectronsAux.",
    "xAOD::PhotonContainer#PhotonCollection->"
    "xAOD::PhotonContainer#Photons",
    "xAOD::PhotonAuxContainer#PhotonCollectionAux.->"
    "xAOD::PhotonAuxContainer#PhotonsAux.",
    "xAOD::CaloClusterContainer#egClusterCollection->"
    "xAOD::CaloClusterContainer#egammaClusters",
    "xAOD::CaloClusterAuxContainer#egClusterCollectionAux.->"
    "xAOD::CaloClusterAuxContainer#egammaClustersAux.",
    "xAOD::CaloClusterContainer#LArClusterEMFrwd->"
    "xAOD::CaloClusterContainer#ForwardElectronClusters",
    "xAOD::CaloClusterAuxContainer#LArClusterEMFrwdAux.->"
    "xAOD::CaloClusterAuxContainer#ForwardElectronClustersAux.",

    "xAOD::TrackParticleContainer#InDetTrackParticlesForward->"
    "xAOD::TrackParticleContainer#InDetForwardTrackParticles",
    "xAOD::TrackParticleAuxContainer#InDetTrackParticlesForwardAux.->"
    "xAOD::TrackParticleAuxContainer#InDetForwardTrackParticlesAux.",
    "xAOD::TrackParticleContainer#InDetTrackParticlesLowBeta->"
    "xAOD::TrackParticleContainer#InDetLowBetaTrackParticles",
    "xAOD::TrackParticleAuxContainer#InDetTrackParticlesLowBetaAux.->"
    "xAOD::TrackParticleAuxContainer#InDetLowBetaTrackParticlesAux.",

    "xAOD::TauJetContainer#TauRecContainer->"
    "xAOD::TauJetContainer#TauJets",
    "xAOD::TauJetAuxContainer#TauRecContainerAux.->"
    "xAOD::TauJetAuxContainer#TauJetsAux.",
    "xAOD::CaloClusterContainer#TauPi0ClusterContainer->"
    "xAOD::CaloClusterContainer#TauPi0Clusters",
    "xAOD::CaloClusterAuxContainer#TauPi0ClusterContainerAux.->"
    "xAOD::CaloClusterAuxContainer#TauPi0ClustersAux.",
    "xAOD::VertexContainer#TauSecondaryVertexContainer->"
    "xAOD::VertexContainer#TauSecondaryVertices",
    "xAOD::VertexAuxContainer#TauSecondaryVertexContainerAux.->"
    "xAOD::VertexAuxContainer#TauSecondaryVerticesAux.",
    "xAOD::PFOContainer#TauShotPFOContainer->"
    "xAOD::PFOContainer#TauShotParticleFlowObjects",
    "xAOD::PFOAuxContainer#TauShotPFOContainerAux.->"
    "xAOD::PFOAuxContainer#TauShotParticleFlowObjectsAux.",
    "xAOD::PFOContainer#TauPi0ChargedPFOContainer->"
    "xAOD::PFOContainer#TauChargedParticleFlowObjects",
    "xAOD::PFOAuxContainer#TauPi0ChargedPFOContainerAux.->"
    "xAOD::PFOAuxContainer#TauChargedParticleFlowObjectsAux.",
    "xAOD::PFOContainer#TauPi0NeutralPFOContainer->"
    "xAOD::PFOContainer#TauNeutralParticleFlowObjects",
    "xAOD::PFOAuxContainer#TauPi0NeutralPFOContainerAux.->"
    "xAOD::PFOAuxContainer#TauNeutralParticleFlowObjectsAux.",

    "xAOD::PFOContainer#chargedJetETMissPFO_eflowRec->"
    "xAOD::PFOContainer#JetETMissChargedParticleFlowObjects",
    "xAOD::PFOAuxContainer#chargedJetETMissPFO_eflowRecAux.->"
    "xAOD::PFOAuxContainer#JetETMissChargedParticleFlowObjectsAux.",
    "xAOD::PFOContainer#neutralJetETMissPFO_eflowRec->"
    "xAOD::PFOContainer#JetETMissNeutralParticleFlowObjects",
    "xAOD::PFOAuxContainer#neutralJetETMissPFO_eflowRecAux.->"
    "xAOD::PFOAuxContainer#JetETMissNeutralParticleFlowObjectsAux.",

    "xAOD::CaloClusterContainer#CaloCalTopoCluster->"
    "xAOD::CaloClusterContainer#CaloCalTopoClusters",
    "xAOD::CaloClusterAuxContainer#CaloCalTopoClusterAux.->"
    "xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.",

    "xAOD::TruthEventContainer#TruthEvent->"
    "xAOD::TruthEventContainer#TruthEvents",
    "xAOD::TruthEventAuxContainer#TruthEventAux.->"   
    "xAOD::TruthEventAuxContainer#TruthEventsAux.",   
    "xAOD::TruthParticleContainer#TruthParticle->"
    "xAOD::TruthParticleContainer#TruthParticles",
    "xAOD::TruthParticleAuxContainer#TruthParticleAux.->"   
    "xAOD::TruthParticleAuxContainer#TruthParticlesAux.",   
    "xAOD::TruthVertexContainer#TruthVertex->"
    "xAOD::TruthVertexContainer#TruthVertices",
    "xAOD::TruthVertexAuxContainer#TruthVertexAux.->"   
    "xAOD::TruthVertexAuxContainer#TruthVerticesAux."   
    ]
svcMgr.AddressRemappingSvc.OutputLevel=WARNING #make quiet all the silly INFO printout
svcMgr.AddressRemappingSvc.SkipBadRemappings=True #for DxAOD running .. shouldn't harm in xAOD running either though
