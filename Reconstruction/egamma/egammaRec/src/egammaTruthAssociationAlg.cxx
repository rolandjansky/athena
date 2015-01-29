/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaRec/egammaTruthAssociationAlg.h"

#include "StoreGate/StoreGateSvc.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "xAODTruth/TruthParticle.h"
// #include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthEventContainer.h"

#include "MCTruthClassifier/IMCTruthClassifier.h"

typedef ElementLink<xAOD::TruthParticleContainer> TruthLink_t;
typedef ElementLink<xAOD::CaloClusterContainer> ClusterLink_t;  
typedef ElementLink<xAOD::ElectronContainer> ElectronLink_t;  
typedef ElementLink<xAOD::PhotonContainer> PhotonLink_t;  
  
// =============================================================
egammaTruthAssociationAlg::egammaTruthAssociationAlg(const std::string& name, 
				   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_storeGate(0), m_egammaTruthContainer(0)
{
  declareProperty("ClusterContainerName", m_clusterContainerName, 
    "Name of the egamma cluster container");
  declareProperty("ElectronContainerName", m_electronContainerName, 
    "Name of the input electron container");
  declareProperty("FwdElectronContainerName", m_fwdElectronContainerName,
    "Name of the input fwd electron container");    
  declareProperty("PhotonContainerName", m_photonContainerName, 
    "Name of the input photon container");
  declareProperty("TruthParticleContainerName", m_truthParticleContainerName, 
    "Name of the truth particle container");
  declareProperty("TruthEventContainerName", m_truthEventContainerName, 
    "Name of the truth event container");
  declareProperty("EgammaTruthContainerName", m_egammaTruthParticleContainerName,
    "Name of the output egamma truth particle container");
  declareProperty("CreateEgammaTruthContainer", m_doEgammaTruthContainer = true,
    "Create egammaTruthContainer ?");
  declareProperty("MatchForwardElectrons", m_matchForwardElectrons = true,
    "Match forward electrons?");
  declareProperty("MatchClusters", m_matchClusters = false,
    "Match clusters?");    
  declareProperty("MinPtEgammaTruth", m_minPt = 1e3, 
    "Minimum Pt to enter egamma truth particle container");
      
  declareProperty("MCTruthClassifier", m_mcTruthClassifier, 
    "Handle of MCTruthClassifier");
}

egammaTruthAssociationAlg::~egammaTruthAssociationAlg() {}

// =============================================================
StatusCode egammaTruthAssociationAlg::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");

  CHECK( service("StoreGateSvc", m_storeGate) );
  CHECK( m_mcTruthClassifier.retrieve() );
  ATH_MSG_DEBUG("Retrieved tool " << m_mcTruthClassifier);
  ATH_MSG_DEBUG("Initialization successful");

  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaTruthAssociationAlg::finalize() {
  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaTruthAssociationAlg::execute() 
{  
  
  if (m_doEgammaTruthContainer)
  {
    // Create truth egamma container
    m_egammaTruthContainer = new xAOD::TruthParticleContainer();
    CHECK( evtStore()->record( m_egammaTruthContainer, m_egammaTruthParticleContainerName ) );
    xAOD::TruthParticleAuxContainer* egammaTruthAuxContainer = new xAOD::TruthParticleAuxContainer();
    CHECK( evtStore()->record( egammaTruthAuxContainer, m_egammaTruthParticleContainerName + "Aux." ) );
    m_egammaTruthContainer->setStore( egammaTruthAuxContainer );
    ATH_MSG_DEBUG( "Recorded TruthParticleContainer with key: " << m_egammaTruthParticleContainerName );
  
    // Add a copy of electrons and photons to the truth egamma container
    const xAOD::TruthEventContainer* truthEvtContainer{0};
    if ( evtStore()->retrieve(truthEvtContainer,m_truthEventContainerName).isFailure()
         || !truthEvtContainer->size() )
    {
      ATH_MSG_WARNING("Could not retrieve " << m_truthEventContainerName << 
       " or container empty, returning");
      return StatusCode::SUCCESS;
    }
    
    for (const auto& truthParticleLink : truthEvtContainer->front()->truthParticleLinks() )
    {
      if (!truthParticleLink.isValid()) continue;
      const xAOD::TruthParticle* truthParticle = *truthParticleLink;
      if (!isPromptEgammaParticle(truthParticle)) continue;
      getNewTruthParticle(truthParticle, truthParticleLink.getDataPtr(), m_egammaTruthContainer);
    }    
  }

  // Decorate containers with truth info, including links to truth particles
  // Decorate the truth particles with links to the reco ones
  CHECK( match<xAOD::Electron>(m_electronContainerName, "Electron") );
  CHECK( match<xAOD::Photon>(m_photonContainerName, "Photon") );
  if (m_matchClusters) {
    CHECK( match<xAOD::CaloCluster>(m_clusterContainerName, "Cluster") );
  }

  if (m_matchForwardElectrons){
    CHECK( match<xAOD::Electron>(m_fwdElectronContainerName, "Electron") );
  }

  return StatusCode::SUCCESS;
}

// ==========================================================================
StatusCode egammaTruthAssociationAlg::decorateWithTruthInfo(xAOD::IParticle *part, MCTruthInfo_t &info) const
{
  const xAOD::TruthParticle *truthParticle = m_mcTruthClassifier->getGenPart();
  
  xAOD::TruthParticleContainer *truthContainer;
  CHECK ( m_storeGate->retrieve(truthContainer, m_truthParticleContainerName) );
  
  if (truthParticle) {
    ElementLink<xAOD::TruthParticleContainer> link(truthParticle, *truthContainer);
    ATH_MSG_DEBUG("Decorating object with link to truth, index = " << link.index());
    part->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" ) = link;
  } else {
    part->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" ) = ElementLink<xAOD::TruthParticleContainer>();
  }
  part->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" ).toPersistent();
  ATH_MSG_DEBUG("truthType = " << info.first << " truthOrigin = " << info.second);
  part->auxdata<int>("truthType") = static_cast<int>( info.first );
  part->auxdata<int>("truthOrigin") = static_cast<int>( info.second );
  
  return StatusCode::SUCCESS;
}

// ==========================================================================
bool egammaTruthAssociationAlg::isPromptEgammaParticle(const xAOD::TruthParticle *truth) const
{
  if ((truth->pdgId() != 22 && abs(truth->pdgId()) != 11) || 
      truth->status() == 2 || truth->status() == 3 ||
      truth->barcode() > 200e3 ||
      truth->pt() < m_minPt) return false;
  
  MCTruthInfo_t type = m_mcTruthClassifier->particleTruthClassifier(truth);

  // Isolated electron or photon
  if(type.first==MCTruthPartClassifier::IsoElectron ||
     type.first==MCTruthPartClassifier::IsoPhoton) return true;
  
  // FSR photon
  if (type.first == MCTruthPartClassifier::NonIsoPhoton && 
      type.second == MCTruthPartClassifier::FSRPhot) return true;
  
  // Electron from photon conversion
//   if(type.first==BkgElectron&&type.second==PhotonConv)
//   {
//     type=m_mcTruthClassifier->checkOrigOfBkgElec(m_mcTruthClassifier->getGenPart());
//     if((type.first==IsoElectron)||(type.first==NonIsoPhoton&&type.second==FSRPhot))
//       return true;
//   }
//   
//   std::vector <const xAOD::TruthParticle*> theParts = m_mcTruthClassifier->getCnvPhotTrkToTruthPart();
//   std::vector<MCTruthPartClassifier::ParticleType>   cnvType = m_mcTruthClassifier->getCnvPhotPartType();
//   std::vector<MCTruthPartClassifier::ParticleOrigin> cnvOrig = m_mcTruthClassifier->getCnvPhotPartOrig();
//   for(unsigned int i=0; i<theParts.size(); i++){
//     if(cnvType[i]==BkgElectron&&cnvOrig[i]==PhotonConv){
//       std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> bkgType=m_mcTruthClassifier->checkOrigOfBkgElec(theParts[i]);
//       if(bkgType.first==IsoPhoton){ return true; }
//   
  return false;
}
    
// ==========================================================================
void egammaTruthAssociationAlg::getNewTruthParticle(const xAOD::TruthParticle *truth, const xAOD::TruthParticleContainer *oldContainer, xAOD::TruthParticleContainer *newContainer) const
{
  xAOD::TruthParticle* truthParticle = new xAOD::TruthParticle();
  newContainer->push_back( truthParticle );

  truthParticle->setPdgId(truth->pdgId());
  truthParticle->setBarcode(truth->barcode());
  truthParticle->setStatus(truth->status());
  truthParticle->setPx(truth->px());
  truthParticle->setPy(truth->py());
  truthParticle->setPz(truth->pz());
  truthParticle->setE(truth->e());
  truthParticle->setM(truth->m());
//   truthParticle->setPolarizationTheta(truth->polarizationTheta());
//   truthParticle->setPolarizationPhi(truth->polarizationPhi());
  truthParticle->setProdVtxLink( truth->prodVtxLink() );
  truthParticle->setDecayVtxLink( truth->decayVtxLink() );
  
  if (m_matchClusters) truthParticle->auxdata< ClusterLink_t >("recoClusterLink") = ClusterLink_t();
  truthParticle->auxdata< ElectronLink_t >("recoElectronLink") = ElectronLink_t();
  truthParticle->auxdata< PhotonLink_t >("recoPhotonLink") = PhotonLink_t();  
  
  truthParticle->auxdata< TruthLink_t >("truthParticleLink") = TruthLink_t(truth, *oldContainer);
  truthParticle->auxdata< TruthLink_t >("truthParticleLink").toPersistent();
  ATH_MSG_DEBUG("Decorating truth particle with link to old truth, index = " << truthParticle->auxdata< TruthLink_t >("truthParticleLink").index() );

  // MCTruthClassifier info
  MCTruthInfo_t info = m_mcTruthClassifier->particleTruthClassifier(truth);
  truthParticle->auxdata<int>("truthType") = static_cast<int>( info.first );
  truthParticle->auxdata<int>("truthOrigin") = static_cast<int>( info.second );
}

// ==========================================================================
template<class T> bool egammaTruthAssociationAlg::decorateWithRecoLink(T* part, const DataVector<T>* container, std::string name) const
{
  const xAOD::TruthParticle *truth = xAOD::EgammaHelpers::getTruthParticle(part);
  if (!truth)
  {
    ATH_MSG_DEBUG("No truth particle associated to " << name);
    return true;
  }
  xAOD::TruthParticle *truthEgamma = getEgammaTruthParticle(truth);
  
  if (!truthEgamma)
  {
    ATH_MSG_DEBUG("Truth particle associated to " << name << " not in egamma truth collection");
    return true;
  }
  
  ElementLink< DataVector<T> > link(part, *container);
  std::string linkName = "reco" + name + "Link";
  truthEgamma->auxdata< ElementLink< DataVector<T> > >(linkName) = link;
  truthEgamma->auxdata< ElementLink< DataVector<T> > >(linkName).toPersistent(); 
  ATH_MSG_DEBUG("Decorating truth egamma particle with link to " << name << ", index = " << link.index() );
  
  return true;
}

// ==========================================================================
xAOD::TruthParticle* egammaTruthAssociationAlg::getEgammaTruthParticle(const xAOD::TruthParticle *truth) const
{
  if (!truth) return 0;
  
  // Find the original truth particle for electrons from conversions
  for (unsigned int i = 0; i < 100 && truth && truth->barcode() > 200e3; ++i)
  {
    if (truth->prodVtx() && truth->prodVtx()->nIncomingParticles())
      truth = truth->prodVtx()->incomingParticle(0);
    else
      break;
  }
    
  for (const auto& egammaTruth : *m_egammaTruthContainer)
    if (truth->barcode() == egammaTruth->barcode()) 
      return egammaTruth;
  
  return 0;
} 

// ==========================================================================   
template<class T> StatusCode egammaTruthAssociationAlg::match(std::string containerName, std::string typeName) const
{
  ATH_MSG_DEBUG("Truth matching for container " << containerName);
  DataVector<T> *container;
  if( m_storeGate->retrieve(container, containerName).isFailure() )
  {
    ATH_MSG_WARNING("Could not retrieve " << containerName << " skipping");
    return StatusCode::SUCCESS;  
  }
  for (auto particle : *container)
  {
    // Get truth info and decorate reco particle
    MCTruthInfo_t info = particleTruthClassifier<T>(particle);
    CHECK( decorateWithTruthInfo(particle, info) );
    
    // Decorate the corresponding truth particle with the link to the reco
    if (m_doEgammaTruthContainer && !decorateWithRecoLink<T>(particle, container, typeName) )
      return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}  

// ==========================================================================   
template<class T> egammaTruthAssociationAlg::MCTruthInfo_t egammaTruthAssociationAlg::particleTruthClassifier(const T* particle) const
{
  return m_mcTruthClassifier->particleTruthClassifier(particle);
}

/** Template specialisation for electrons: 
  * second pass based on the cluster to find true photons **/
template<> egammaTruthAssociationAlg::MCTruthInfo_t egammaTruthAssociationAlg::particleTruthClassifier<xAOD::Electron>(const xAOD::Electron* electron) const
{
  MCTruthInfo_t info = m_mcTruthClassifier->particleTruthClassifier(electron);
  if (info.first == MCTruthPartClassifier::Unknown &&
      !xAOD::EgammaHelpers::isFwdElectron(electron) && electron->caloCluster())
  {
    ATH_MSG_DEBUG("Trying cluster-based truth classification for electron");
    info = m_mcTruthClassifier->particleTruthClassifier( electron->caloCluster() );
  }
  return info;
}
