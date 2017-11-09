/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s)
#include "tauRecTools/BuildTruthTaus.h"

// Core include(s):
#include "AthLinks/ElementLink.h"

// EDM include(s):
#include "xAODTruth/TruthVertex.h"
#include "xAODEventInfo/EventInfo.h"

// Tool include(s)
#include "MCTruthClassifier/MCTruthClassifier.h"

#define PRINTVAR(VAR)                                                   \
  std::cout<<__FILE__<<" "<<__LINE__<<":"<<#VAR<<" = "<<VAR<<"\n";

using namespace tauRecTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
BuildTruthTaus::BuildTruthTaus( const std::string& name )
  : AsgMetadataTool(name)
  , m_bIsData(false)
  , m_bIsConfigured(false)
  , m_bTruthTauAvailable(true)
  , m_xTruthTauContainerConst(0)
  , m_xTruthMuonContainerConst(0)
  , m_xTruthElectronContainerConst(0)
  , m_xTruthJetContainerConst(0)
  , m_xTruthTauContainer(0)
  , m_bIsTruthMatchedAvailable(false)
  , m_bIsTruthMatchedAvailableChecked(false)
  , m_bNewEvent(false)
  , m_xTruthParticleContainer(0)
  , m_xTruthTauAuxContainer(0)
  , m_sNewTruthTauContainerNameAux("TruthTausAux.")
  , m_bTruthMuonAvailable(true)
  , m_bTruthElectronAvailable(true)
  , m_bTruthJetAvailable(true)
  , m_iNChargedPions(0)
  , m_iNNeutralPions(0)
  , m_iNChargedOthers(0)
  , m_iNNeutralOthers(0)
  , m_iNChargedDaughters(0)
  , m_bIsHadronicTau(false)
  , m_tMCTruthClassifier("MCTruthClassifierTool", this)
{
  declareProperty( "WriteTruthTaus", m_bWriteTruthTaus = false);

  // container names
  declareProperty( "NewTruthTauContainerName", m_sNewTruthTauContainerName = "TruthTaus");
  declareProperty( "TruthTauContainerName", m_sTruthTauContainerName = "TruthTaus");
  declareProperty( "TruthMuonContainerName", m_sTruthMuonContainerName = "TruthMuons");
  declareProperty( "TruthElectronContainerName", m_sTruthElectronContainerName = "TruthElectrons");
  declareProperty( "TruthJetContainerName", m_sTruthJetContainerName = "AntiKt4TruthJets");
  declareProperty( "TruthParticlesContainerName", m_sTruthParticlesContainerName = "TruthParticles");
  declareProperty( "WriteInvisibleFourMomentum", m_bWriteInvisibleFourMomentum = false);
  declareProperty( "WriteVisibleChargedFourMomentum", m_bWriteVisibleChargedFourMomentum = false);
  declareProperty( "WriteVisibleNeutralFourMomentum", m_bWriteVisibleNeutralFourMomentum = false);
  declareProperty( "WriteDecayModeVector", m_bWriteDecayModeVector = true);

  // Tool handle
  m_tMCTruthClassifier.declarePropertyFor( this, "MCTruthClassifierTool", "The MCTruthClassifierTool" );
}

//______________________________________________________________________________
BuildTruthTaus::~BuildTruthTaus( )
{

}

//______________________________________________________________________________
StatusCode BuildTruthTaus::initialize()
{
  ATH_MSG_INFO( "Initializing BuildTruthTaus" );
  m_sNewTruthTauContainerNameAux = m_sNewTruthTauContainerName + "Aux.";

  ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tMCTruthClassifier, MCTruthClassifier));
  ATH_CHECK(m_tMCTruthClassifier.initialize());
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
xAOD::TruthParticleContainer* BuildTruthTaus::getTruthTauContainer()
{
  if (m_bIsData)
    return 0;
  if (!m_bTruthTauAvailable)
    return m_xTruthTauContainer;
  else
  {
    ATH_MSG_WARNING("TruthTau container was available from the event store and not rebuilt. Please get it from the event store");
    return 0;
  }
}

//______________________________________________________________________________
xAOD::TruthParticleAuxContainer* BuildTruthTaus::getTruthTauAuxContainer()
{
  if (m_bIsData)
    return 0;
  if (!m_bTruthTauAvailable)
    return m_xTruthTauAuxContainer;
  else
  {
    ATH_MSG_WARNING("TruthTau auxiliary container was available from the event store and not rebuilt. Please get it from the event store");
    return 0;
  }
}

//______________________________________________________________________________
StatusCode BuildTruthTaus::beginEvent()
{
  m_bNewEvent = true;
  if (m_bIsConfigured)
    return StatusCode::SUCCESS;

  const xAOD::EventInfo* xEventInfo = 0;
  ATH_CHECK(evtStore()->retrieve(xEventInfo,"EventInfo"));
  m_bIsData = !(xEventInfo->eventType( xAOD::EventInfo::IS_SIMULATION));
  m_bIsConfigured=true;

  return StatusCode::SUCCESS;
}

StatusCode BuildTruthTaus::retrieveTruthTaus()
{
  if (m_bIsData)
    return StatusCode::SUCCESS;

  if (m_bNewEvent)
    m_bNewEvent = false;
  else
    return StatusCode::SUCCESS;

  if ( m_bTruthTauAvailable )
  {
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_sTruthTauContainerName))
      ATH_CHECK( evtStore()->retrieve(m_xTruthTauContainerConst,m_sTruthTauContainerName));
    else
    {
      ATH_MSG_INFO("TruthTaus container with name " << m_sTruthTauContainerName << " is not available, will generate the container for each event from TruthParticles container");
      m_bTruthTauAvailable = false;
    }
  }

  if ( m_bTruthMuonAvailable )
  {
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_sTruthMuonContainerName))
      ATH_CHECK(evtStore()->retrieve(m_xTruthMuonContainerConst,m_sTruthMuonContainerName));
    else
    {
      ATH_MSG_INFO("TruthMuons container with name " << m_sTruthMuonContainerName << " is not available, won't perform matching to truth muons");
      m_bTruthMuonAvailable = false;
    }
  }

  if ( m_bTruthElectronAvailable )
  {
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_sTruthElectronContainerName))
      ATH_CHECK(evtStore()->retrieve(m_xTruthElectronContainerConst,m_sTruthElectronContainerName));
    else
    {
      ATH_MSG_INFO("TruthElectrons container with name " << m_sTruthElectronContainerName << " is not available, won't perform matching to truth electrons");
      m_bTruthElectronAvailable = false;
    }
  }

  if ( m_bTruthJetAvailable )
  {
    if (evtStore()->contains<xAOD::JetContainer>(m_sTruthJetContainerName))
      ATH_CHECK(evtStore()->retrieve(m_xTruthJetContainerConst,m_sTruthJetContainerName));
    else
    {
      ATH_MSG_INFO("TruthJets container with name " << m_sTruthJetContainerName << " is not available, won't perform matching to truth jets");
      m_bTruthJetAvailable = false;
    }
  }

  // go here if TruthTaus was not found in m_bTruthTauAvailable if-block
  if ( !m_bTruthTauAvailable )
  {
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_sTruthParticlesContainerName))
    {
      if ( evtStore()->retrieve(m_xTruthParticleContainer,m_sTruthParticlesContainerName).isSuccess() )
        return buildTruthTausFromTruthParticles();
      else
        return StatusCode::FAILURE;
    }
    else
    {
      ATH_MSG_FATAL("TruthParticles container is not available but needed for building truth taus");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
//______________________________________________________________________________
StatusCode BuildTruthTaus::buildTruthTausFromTruthParticles()
{
  if(m_xTruthTauContainer && !m_bWriteTruthTaus)
  {
    delete m_xTruthTauContainer;
    delete m_xTruthTauAuxContainer;
  }

  // Create the new containers
  m_xTruthTauContainer = new xAOD::TruthParticleContainer();
  m_xTruthTauAuxContainer = new xAOD::TruthParticleAuxContainer();
  m_xTruthTauContainer->setStore( m_xTruthTauAuxContainer );

  if (m_bWriteTruthTaus)
  {
    if ( evtStore()->record( m_xTruthTauContainer, m_sNewTruthTauContainerName ).isFailure() )
    {
      ATH_MSG_FATAL("Couldn't create truth tau container with key " << m_sNewTruthTauContainerName);
      return StatusCode::FAILURE;
    }
    if ( evtStore()->record( m_xTruthTauAuxContainer, m_sNewTruthTauContainerNameAux ).isFailure() )
    {
      ATH_MSG_FATAL("Couldn't create truth tau container with key " << m_sNewTruthTauContainerNameAux);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Recorded new TruthParticleContainer with key: " <<  m_sNewTruthTauContainerName);
  }

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
      std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> pClassification = m_tMCTruthClassifier->particleTruthClassifier(xTruthTau);
      static SG::AuxElement::Decorator<unsigned int> decClassifierParticleType("classifierParticleType");
      static SG::AuxElement::Decorator<unsigned int> decClassifierParticleOrigin("classifierParticleOrigin");
      decClassifierParticleType(*xTruthTau) = pClassification.first;
      decClassifierParticleOrigin(*xTruthTau) = pClassification.second;

      // create link to the original TruthParticle
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthParticle, *m_xTruthParticleContainer);
      static SG::AuxElement::Accessor<ElementLink< xAOD::TruthParticleContainer > > accOriginalTruthParticle("originalTruthParticle");
      accOriginalTruthParticle(*xTruthTau) = lTruthParticleLink;

      m_xTruthTauContainer->push_back(xTruthTau);
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode BuildTruthTaus::examineTruthTau(const xAOD::TruthParticle& xTruthParticle)
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

  // define truth visible kinematic variables
  m_vTruthVisTLV = TLorentzVector();
  m_vTruthVisTLVCharged = TLorentzVector();
  m_vTruthVisTLVNeutralPions = TLorentzVector();
  m_vTruthVisTLVNeutralOthers = TLorentzVector();

  // default false, if there is a hadron in decay products, it is
  // switched to true
  m_bIsHadronicTau = false;

  const xAOD::TruthVertex* xDecayVertex = xTruthParticle.decayVtx();
  if ( !xDecayVertex )
    return StatusCode::FAILURE;
  for ( size_t iOutgoingParticle = 0; iOutgoingParticle < xDecayVertex->nOutgoingParticles(); ++iOutgoingParticle )
  {
    const xAOD::TruthParticle* xTruthDaughter = xDecayVertex->outgoingParticle(iOutgoingParticle);
    if (!xTruthDaughter)
    {
      ATH_MSG_FATAL("Truth daughter of tau decay was not found in "<<m_sTruthParticlesContainerName<<" container. Please ensure that this container has the full tau decay information or produce the TruthTaus container in AtlasDerivation.\nInformation on how to do this can be found here:\nhttps://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015#Accessing_Tau_Truth_Information");
      return StatusCode::FAILURE;
    }

    // if tau decays into tau this is not a proper tau decay
    if ( xTruthDaughter->isTau() )
    {
      ATH_MSG_VERBOSE("Tau decays into a tau itself -> skip this decay");
      return StatusCode::FAILURE;
    }
  }

  examineTruthTauDecay(xTruthParticle).ignore();

  if (m_bIsHadronicTau)
    ATH_MSG_VERBOSE(m_iNChargedDaughters << " prong hadronic truth tau was found with barcode "<<xTruthParticle.barcode());
  else
    ATH_MSG_VERBOSE(m_iNChargedDaughters << " prong leptonic truth tau was found with barcode "<<xTruthParticle.barcode());
  if ( m_iNChargedDaughters%2 == 0 )
  {
    ATH_MSG_WARNING("found tau with even multiplicity: " << m_iNChargedDaughters);
    printDecay(xTruthParticle);
  }

  static SG::AuxElement::Decorator<double> decPtVis("pt_vis");
  static SG::AuxElement::Decorator<double> decEtaVis("eta_vis");
  static SG::AuxElement::Decorator<double> decPhiVis("phi_vis");
  static SG::AuxElement::Decorator<double> accMVis("m_vis");

  static SG::AuxElement::Decorator<size_t> decNumCharged("numCharged");
  static SG::AuxElement::Decorator<size_t> decNumChargedPion("numChargedPion");
  static SG::AuxElement::Decorator<size_t> decNumNeutral("numNeutral");
  static SG::AuxElement::Decorator<size_t> accNumNeutralPion("numNeutralPion");

  decPtVis(xTruthParticle) = m_vTruthVisTLV.Pt();
  decEtaVis(xTruthParticle) = m_vTruthVisTLV.Eta();
  decPhiVis(xTruthParticle) = m_vTruthVisTLV.Phi();
  accMVis(xTruthParticle) = m_vTruthVisTLV.M();

  decNumCharged(xTruthParticle) = m_iNChargedDaughters;
  decNumChargedPion(xTruthParticle) = m_iNChargedPions;
  decNumNeutral(xTruthParticle) = m_iNNeutralPions+m_iNNeutralOthers;
  accNumNeutralPion(xTruthParticle) = m_iNNeutralPions;

  static SG::AuxElement::Decorator<char> decIsHadronicTau("IsHadronicTau");
  decIsHadronicTau(xTruthParticle) = (char)m_bIsHadronicTau;

  if ( m_bWriteInvisibleFourMomentum )
  {
    TLorentzVector vTruthInvisTLV = xTruthParticle.p4() - m_vTruthVisTLV;
    static SG::AuxElement::Decorator<double> decPtInvis("pt_invis");
    static SG::AuxElement::Decorator<double> decEtaInvis("eta_invis");
    static SG::AuxElement::Decorator<double> decPhiInvis("phi_invis");
    static SG::AuxElement::Decorator<double> accMInvis("m_invis");
    decPtInvis(xTruthParticle)  = vTruthInvisTLV.Pt();
    decEtaInvis(xTruthParticle) = vTruthInvisTLV.Eta();
    decPhiInvis(xTruthParticle) = vTruthInvisTLV.Phi();
    accMInvis(xTruthParticle)   = vTruthInvisTLV.M();
  }

  if ( m_bWriteVisibleChargedFourMomentum )
  {
    static SG::AuxElement::Decorator<double> decPtVisCharged("pt_vis_charged");
    static SG::AuxElement::Decorator<double> decEtaVisCharged("eta_vis_charged");
    static SG::AuxElement::Decorator<double> decPhiVisCharged("phi_vis_charged");
    static SG::AuxElement::Decorator<double> accMVisCharged("m_vis_charged");
    decPtVisCharged(xTruthParticle)  = m_vTruthVisTLVCharged.Pt();
    decEtaVisCharged(xTruthParticle) = m_vTruthVisTLVCharged.Eta();
    decPhiVisCharged(xTruthParticle) = m_vTruthVisTLVCharged.Phi();
    accMVisCharged(xTruthParticle)   = m_vTruthVisTLVCharged.M();
  }

  if ( m_bWriteVisibleNeutralFourMomentum )
  {
    static SG::AuxElement::Decorator<double> decPtVisNeutralPions("pt_vis_neutral_pions");
    static SG::AuxElement::Decorator<double> decEtaVisNeutralPions("eta_vis_neutral_pions");
    static SG::AuxElement::Decorator<double> decPhiVisNeutralPions("phi_vis_neutral_pions");
    static SG::AuxElement::Decorator<double> accMVisNeutralPions("m_vis_neutral_pions");
    decPtVisNeutralPions(xTruthParticle)  = m_vTruthVisTLVNeutralPions.Pt();
    decEtaVisNeutralPions(xTruthParticle) = m_vTruthVisTLVNeutralPions.Eta();
    decPhiVisNeutralPions(xTruthParticle) = m_vTruthVisTLVNeutralPions.Phi();
    accMVisNeutralPions(xTruthParticle)   = m_vTruthVisTLVNeutralPions.M();
    
    static SG::AuxElement::Decorator<double> decPtVisNeutralOthers("pt_vis_neutral_others");
    static SG::AuxElement::Decorator<double> decEtaVisNeutralOthers("eta_vis_neutral_others");
    static SG::AuxElement::Decorator<double> decPhiVisNeutralOthers("phi_vis_neutral_others");
    static SG::AuxElement::Decorator<double> accMVisNeutralOthers("m_vis_neutral_others");
    decPtVisNeutralOthers(xTruthParticle)  = m_vTruthVisTLVNeutralOthers.Pt();
    decEtaVisNeutralOthers(xTruthParticle) = m_vTruthVisTLVNeutralOthers.Eta();
    decPhiVisNeutralOthers(xTruthParticle) = m_vTruthVisTLVNeutralOthers.Phi();
    accMVisNeutralOthers(xTruthParticle)   = m_vTruthVisTLVNeutralOthers.M();
  }

  if ( m_bWriteDecayModeVector )
  {
    static SG::AuxElement::Decorator<std::vector<int> > decDecayModeVector("DecayModeVector");
    decDecayModeVector(xTruthParticle) = m_vDecayMode;
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode BuildTruthTaus::examineTruthTauDecay(const xAOD::TruthParticle& xTruthParticle)
{
  // get vertex and check if it exists
  const xAOD::TruthVertex* xDecayVertex = xTruthParticle.decayVtx();
  if (!xDecayVertex)
    return StatusCode::SUCCESS;

  for ( size_t iOutgoingParticle = 0; iOutgoingParticle < xDecayVertex->nOutgoingParticles(); ++iOutgoingParticle )
  {
    const xAOD::TruthParticle* xTruthDaughter = xDecayVertex->outgoingParticle(iOutgoingParticle);
    if (!xTruthDaughter)
    {
      ATH_MSG_FATAL("Truth daughter of tau decay was not found in "<<m_sTruthParticlesContainerName<<" container. Please ensure that this container has the full tau decay information or produce the TruthTaus container in AtlasDerivation.\nInformation on how to do this can be found here:\nhttps://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015#Accessing_Tau_Truth_Information");
      return StatusCode::FAILURE;
    }

    int iAbsPdgId = xTruthDaughter->absPdgId();
    int iPdgId = xTruthDaughter->pdgId();

    // look at decay of unstable particles
    if (xTruthDaughter->status() == 2 or xTruthDaughter->status() == 11 or xTruthDaughter->status() == 10902) // 11 for HERWIG++
    {
      if ( iAbsPdgId != 111 and iAbsPdgId != 311 and iAbsPdgId != 310 and iAbsPdgId != 130 )
      {
        examineTruthTauDecay(*xTruthDaughter).ignore();
        continue;
      }
    }

    // only process stable particles
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
    if ( m_bWriteVisibleChargedFourMomentum )
      if ( xTruthDaughter->isCharged() )
        m_vTruthVisTLVCharged += xTruthDaughter->p4();
    if ( m_bWriteVisibleNeutralFourMomentum )
    {
      if ( xTruthDaughter->isNeutral() )
      {
        if (iAbsPdgId==111)
          m_vTruthVisTLVNeutralPions += xTruthDaughter->p4();
        else
          m_vTruthVisTLVNeutralOthers += xTruthDaughter->p4();
      }
    }

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

void BuildTruthTaus::printDecay(const xAOD::TruthParticle& xTruthParticle, int depth) const
{
  // loop over all decay particles, print their kinematic and other properties

  const xAOD::TruthVertex* xDecayVertex = xTruthParticle.decayVtx();
  if (!xDecayVertex)
    return;

  for ( size_t iOutgoingParticle = 0; iOutgoingParticle < xDecayVertex->nOutgoingParticles(); ++iOutgoingParticle )
  {
    const xAOD::TruthParticle* xTruthDaughter = xDecayVertex->outgoingParticle(iOutgoingParticle);
    if (!xTruthDaughter)
    {
      ATH_MSG_FATAL("Truth daughter of tau decay was not found in "<<m_sTruthParticlesContainerName<<" container. Please ensure that this container has the full tau decay information or produce the TruthTaus container in AtlasDerivation.\nInformation on how to do this can be found here:\nhttps://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015#Accessing_Tau_Truth_Information");
      return;
    }
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
