/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthCollectionMakerTau.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dirk Duschinger (dirk.duschinger@cern.ch)
// Create truth tau collection decorated with tau decay specific variables

#include "DerivationFrameworkMCTruth/TruthCollectionMakerTau.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"

// Constructor
DerivationFramework::TruthCollectionMakerTau::TruthCollectionMakerTau(const std::string& t,
								      const std::string& n,
								      const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_particlesKey("TruthParticles"),
  m_collectionName(""),
  m_classifier("MCTruthClassifier/MCTruthClassifier"),
  m_runClassifier(true),
  m_writeVisibleChargedFourMomentum(false),
  m_writeVisibleNeutralFourMomentum(false),
  m_writeDecayModeVector(true)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("ParticlesKey", m_particlesKey);
  declareProperty("NewCollectionName", m_collectionName);
  declareProperty("MCTruthClassifier", m_classifier);
  declareProperty("RunClassifier", m_runClassifier);
  declareProperty("WriteVisibleChargedFourMomentum", m_writeVisibleChargedFourMomentum);
  declareProperty("WriteVisibleNeutralFourMomentum", m_writeVisibleNeutralFourMomentum);
  declareProperty("WriteDecayModeVector", m_writeDecayModeVector);
}

// Destructor
DerivationFramework::TruthCollectionMakerTau::~TruthCollectionMakerTau() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TruthCollectionMakerTau::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");
  if ( m_runClassifier) ATH_CHECK(m_classifier.retrieve() );

  if ( m_particlesKey=="" ) {
    ATH_MSG_FATAL("No truth particle collection provided to use as a basis for new collections");
    return StatusCode::FAILURE;
  } else {ATH_MSG_INFO("Using " << m_particlesKey << " as the source collections for new truth collections");}
    
  if ( m_collectionName=="" ) {
    ATH_MSG_FATAL("No key provided for the new truth particle collection");
    return StatusCode::FAILURE;
  } else {ATH_MSG_INFO("New truth particle collection key: " << m_collectionName );}

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TruthCollectionMakerTau::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

// Selection and collection creation
StatusCode DerivationFramework::TruthCollectionMakerTau::addBranches() const
{
  ATH_MSG_VERBOSE("addBranches() ...");

  // Retrieve truth collections
  const xAOD::TruthParticleContainer* m_xTruthParticleContainer;
  if ( evtStore()->retrieve(m_xTruthParticleContainer,m_particlesKey).isFailure() ) {
    ATH_MSG_ERROR("No TruthParticle collection with name " << m_particlesKey << " found in StoreGate!");
    return StatusCode::FAILURE;
  }
    
  // Create the new containers
  xAOD::TruthParticleContainer* newParticleCollection = new xAOD::TruthParticleContainer();
  CHECK( evtStore()->record( newParticleCollection, m_collectionName ) );
  xAOD::TruthParticleAuxContainer* newParticleAuxCollection = new xAOD::TruthParticleAuxContainer();
  CHECK( evtStore()->record( newParticleAuxCollection, m_collectionName + "Aux." ) );
  newParticleCollection->setStore( newParticleAuxCollection );
  ATH_MSG_DEBUG( "Recorded new TruthParticleContainer with key: " <<  m_collectionName);
  
  if ( !m_xTruthParticleContainer )
    return Status::SUCCESS;
  
  for (auto xTruthParticle : *m_xTruthParticleContainer)
  {
    if ( xTruthParticle->isTau() )
    {
      xAOD::TruthParticle* xTruthTau = new xAOD::TruthParticle();
      xTruthTau->makePrivateStore( *xTruthParticle );
	
      if ( examineTruthTau(*xTruthTau).isFailure() )
      {
	delete xTruthTau;
      	continue;
      }

      // Run classification
      if ( m_runClassifier ) {
  	std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> classification = m_classifier->particleTruthClassifier(xTruthTau);			
  	unsigned int particleType = classification.first;
  	unsigned int particleOrigin = classification.second;
  	xTruthTau->auxdecor<unsigned int>("classifierParticleType") = particleType;
  	xTruthTau->auxdecor<unsigned int>("classifierParticleOrigin") = particleOrigin;
      } 

      // create link to the original TruthParticle
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthParticle, *m_xTruthParticleContainer);
      xTruthTau->auxdata< ElementLink< xAOD::TruthParticleContainer > >("originalTruthParticle" ) = lTruthParticleLink;
	
      newParticleCollection->push_back(xTruthTau);
    }
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode DerivationFramework::TruthCollectionMakerTau::examineTruthTau(const xAOD::TruthParticle& xTruthParticle) const
{
  // skip this tau if it has no decay vertex, should not happen
  if ( !xTruthParticle.hasDecayVtx() )
    return StatusCode::FAILURE;
  
  ATH_MSG_VERBOSE("looking for charged daughters of a truth tau");

  m_iNChargedPions = 0;
  m_iNNeutralPions = 0;
  m_iNChargedOthers = 0;
  m_iNNeutralOthers = 0;
  m_iNChargedDaughters = 0;
  m_vDecayMode.clear();

  // define truth kinematic variables
  m_vTruthVisTLV = TLorentzVector();
  m_vTruthPromptTLV = TLorentzVector();
  m_vTruthVisTLVCharged = TLorentzVector();
  m_vTruthVisTLVNeutral = TLorentzVector();

  // default false, if there is a hadron in decay products, it is
  // switched to true
  m_bIsHadronicTau = false;

  const xAOD::TruthVertex* xDecayVertex = xTruthParticle.decayVtx();
  if ( !xDecayVertex )
    return StatusCode::FAILURE;
  for ( size_t iOutgoingParticle = 0; iOutgoingParticle < xDecayVertex->nOutgoingParticles(); ++iOutgoingParticle )
  {
    const xAOD::TruthParticle* xTruthDaughter = xDecayVertex->outgoingParticle(iOutgoingParticle);
    // if tau decays into tau this is not a proper tau decay 
    if ( xTruthDaughter->isTau() )
    {
      ATH_MSG_VERBOSE("Tau decays into a tau itself -> skip this decay");
      return StatusCode::FAILURE;
    }
  }
  
  examineTruthTauDecay(xTruthParticle).ignore();
  
  getPromptTLV(xTruthParticle).ignore();
  
  ATH_MSG_VERBOSE(m_iNChargedDaughters << " prong truth tau was found with barcode "<<xTruthParticle.barcode());
  if ( m_iNChargedDaughters%2 == 0 )
  {
    ATH_MSG_WARNING("found tau with even multiplicity: " << m_iNChargedDaughters);
    printDecay(xTruthParticle);
  }
  
  xTruthParticle.auxdecor<double>("pt_vis")  = m_vTruthVisTLV.Pt();
  xTruthParticle.auxdecor<double>("eta_vis") = m_vTruthVisTLV.Eta();
  xTruthParticle.auxdecor<double>("phi_vis") = m_vTruthVisTLV.Phi();
  xTruthParticle.auxdecor<double>("m_vis")   = m_vTruthVisTLV.M();

  xTruthParticle.auxdecor<double>("pt_prompt")  = m_vTruthPromptTLV.Pt();
  xTruthParticle.auxdecor<double>("eta_prompt") = m_vTruthPromptTLV.Eta();
  xTruthParticle.auxdecor<double>("phi_prompt") = m_vTruthPromptTLV.Phi();
  xTruthParticle.auxdecor<double>("m_prompt")   = m_vTruthPromptTLV.M();

  xTruthParticle.auxdecor<size_t>("numCharged") = m_iNChargedDaughters;
  xTruthParticle.auxdecor<size_t>("numChargedPion") = m_iNChargedPions;
  xTruthParticle.auxdecor<size_t>("numNeutral") = m_iNNeutralPions+m_iNNeutralOthers;
  xTruthParticle.auxdecor<size_t>("numNeutralPion") = m_iNNeutralPions;

  xTruthParticle.auxdecor<char>("IsHadronicTau") = (char)m_bIsHadronicTau;
    
  if ( m_writeVisibleChargedFourMomentum )
  {
    xTruthParticle.auxdecor<double>("pt_vis_charged")  = m_vTruthVisTLVCharged.Pt();
    xTruthParticle.auxdecor<double>("eta_vis_charged") = m_vTruthVisTLVCharged.Eta();
    xTruthParticle.auxdecor<double>("phi_vis_charged") = m_vTruthVisTLVCharged.Phi();
    xTruthParticle.auxdecor<double>("m_vis_charged")   = m_vTruthVisTLVCharged.M();
  }
  if ( m_writeVisibleNeutralFourMomentum )
  {
    xTruthParticle.auxdecor<double>("pt_vis_neutral")  = m_vTruthVisTLVNeutral.Pt();
    xTruthParticle.auxdecor<double>("eta_vis_neutral") = m_vTruthVisTLVNeutral.Eta();
    xTruthParticle.auxdecor<double>("phi_vis_neutral") = m_vTruthVisTLVNeutral.Phi();
    xTruthParticle.auxdecor<double>("m_vis_neutral")   = m_vTruthVisTLVNeutral.M();
  }

  if ( m_writeDecayModeVector )
    xTruthParticle.auxdecor<std::vector<int>>("DecayModeVector") = m_vDecayMode;

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode DerivationFramework::TruthCollectionMakerTau::examineTruthTauDecay(const xAOD::TruthParticle& xTruthParticle) const
{
  // get vertex and check if it exists
  const xAOD::TruthVertex* xDecayVertex = xTruthParticle.decayVtx();
  if (!xDecayVertex)
    return StatusCode::SUCCESS;
    
  for ( size_t iOutgoingParticle = 0; iOutgoingParticle < xDecayVertex->nOutgoingParticles(); ++iOutgoingParticle )
  {
    const xAOD::TruthParticle* xTruthDaughter = xDecayVertex->outgoingParticle(iOutgoingParticle);
    int iAbsPdgId = xTruthDaughter->absPdgId();
    int iPdgId = xTruthDaughter->pdgId();

    // look at decay of unstable particles
    if (xTruthDaughter->status() == 2 or xTruthDaughter->status() == 11 or xTruthDaughter->status() == 10902) // 11 for HERWIG++
    {
      if ( iAbsPdgId != 111 and iAbsPdgId != 311 and iAbsPdgId != 310 and iAbsPdgId != 130 ) // don't move on for neutral pions and kaons
      {
	examineTruthTauDecay(*xTruthDaughter).ignore();
	continue;
      }
    }

    // only process stable particles and neutral pions and kaons
    if (xTruthDaughter->status() != 1 and xTruthDaughter->status() != 2)
      continue;
    
    // add pdgID to vector for decay mode classification
    m_vDecayMode.push_back(iPdgId);
    
    // if tau decays leptonically, indicated by an electron/muon neutrino then
    // it is not a hadronic decay
    if ( xTruthDaughter->isHadron() )
      m_bIsHadronicTau = true;
    
    // ignore neutrinos for further progress
    if ( xTruthDaughter->isNeutrino() )
    {
      ATH_MSG_VERBOSE("found neutrino decay particle with PdgId "<<iPdgId);
      continue;
    }

    // add momentum of non-neutrino particle to visible momentum
    m_vTruthVisTLV += xTruthDaughter->p4();
    if ( m_writeVisibleChargedFourMomentum )
      if ( xTruthDaughter->isCharged() )
	m_vTruthVisTLVCharged += xTruthDaughter->p4();
    if ( m_writeVisibleNeutralFourMomentum )
      if ( xTruthDaughter->isNeutral() )
	m_vTruthVisTLVNeutral += xTruthDaughter->p4();
    
    // only count charged decay particles
    if ( xTruthDaughter->isCharged() )
    {
      ATH_MSG_VERBOSE("found charged decay particle with PdgId "<<iPdgId);
      m_iNChargedDaughters++;
      // count charged pions
      if (iAbsPdgId==211) m_iNChargedPions++;
      else m_iNChargedOthers++;
    }
    else
    {
      ATH_MSG_VERBOSE("found neutral decay particle with PdgId "<<iPdgId);
      // count neutral pions
      if (iAbsPdgId==111) m_iNNeutralPions++;
      else m_iNNeutralOthers++;
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode DerivationFramework::TruthCollectionMakerTau::getPromptTLV(const xAOD::TruthParticle& xTruthParticle) const
{
  // get vertex and check if it exists
  const xAOD::TruthVertex* xProdVertex = xTruthParticle.prodVtx();
  if (!xProdVertex)
    return StatusCode::SUCCESS;

  // if there are more than one incoming particles than the parent is not well defined 
  if (xProdVertex->nIncomingParticles() != 1)
  {
    m_vTruthPromptTLV = xTruthParticle.p4();
    return StatusCode::SUCCESS;
  }
  else
  {
    // if parent is a tau then proceed to this particle for finding the prompt
    // TLV, if it is not a tau, then the current particle is the prompt tau 
    const xAOD::TruthParticle* xTruthParent = xProdVertex->incomingParticle(0);
    
    if (xTruthParent->isTau())
    {
      return getPromptTLV(*xTruthParent);
    }
    else
    {
      m_vTruthPromptTLV = xTruthParticle.p4();
      return StatusCode::SUCCESS;
    }
  }
  
  return StatusCode::SUCCESS;
}

void DerivationFramework :: TruthCollectionMakerTau :: printDecay(const xAOD::TruthParticle& xTruthParticle, int depth) const
{
  // loop over all decay particles, print their kinematic and other properties
  
  const xAOD::TruthVertex* xDecayVertex = xTruthParticle.decayVtx();
  if (!xDecayVertex)
    return;
  
  for ( size_t iOutgoingParticle = 0; iOutgoingParticle < xDecayVertex->nOutgoingParticles(); ++iOutgoingParticle )
  {
    const xAOD::TruthParticle* xTruthDaughter = xDecayVertex->outgoingParticle(iOutgoingParticle);
    ATH_MSG_WARNING("depth "<<depth
		    <<" e "<<xTruthDaughter->e()
		    <<" eta "<<xTruthDaughter->p4().Eta()
		    <<" phi "<<xTruthDaughter->p4().Phi()
		    <<" pdgid "<<xTruthDaughter->pdgId()
		    <<" status "<<xTruthDaughter->status()
		    <<" barcode "<<xTruthDaughter->barcode());
    printDecay(*xTruthDaughter, depth+1);
  }
}
