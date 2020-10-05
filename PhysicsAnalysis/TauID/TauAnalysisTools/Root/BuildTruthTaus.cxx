/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s)
#include "TauAnalysisTools/BuildTruthTaus.h"

// Core include(s):
#include "AthLinks/ElementLink.h"

// EDM include(s):
#include "xAODTruth/TruthVertex.h"
#include "xAODEventInfo/EventInfo.h"

// Tool include(s)
#include "MCTruthClassifier/MCTruthClassifier.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
BuildTruthTaus::BuildTruthTaus( const std::string& name )
  : AsgMetadataTool(name)
  , m_bTruthTauAvailable(true)
  , m_sNewTruthTauContainerNameAux("TruthTausAux.")
  , m_bTruthMuonAvailable(true)
  , m_bTruthElectronAvailable(true)
  , m_bTruthJetAvailable(true)
  , m_tMCTruthClassifier("MCTruthClassifier", this)
{
  declareProperty( "WriteTruthTaus", m_bWriteTruthTaus = false);
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
}

//______________________________________________________________________________
BuildTruthTaus::~BuildTruthTaus()
{
}

//______________________________________________________________________________
StatusCode BuildTruthTaus::initialize()
{
  ATH_MSG_INFO( "Initializing BuildTruthTaus" );
  m_sNewTruthTauContainerNameAux = m_sNewTruthTauContainerName + "Aux.";

  // The following properties are only available in athena
#ifndef XAOD_ANALYSIS
  ATH_CHECK(m_tMCTruthClassifier.setProperty("ParticleCaloExtensionTool", ""));
  ATH_CHECK(m_tMCTruthClassifier.setProperty("TruthInConeTool", ""));
#endif
  
  ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tMCTruthClassifier, MCTruthClassifier));
  ATH_CHECK(m_tMCTruthClassifier.initialize());

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
xAOD::TruthParticleContainer* BuildTruthTaus::getTruthTauContainer()
{
  if (!m_bTruthTauAvailable)
    return m_truthTausEvent.m_xTruthTauContainer;
  else
  {
    ATH_MSG_WARNING("TruthTau container was available from the event store and not rebuilt. Please get it from the event store");
    return nullptr;
  }
}

//______________________________________________________________________________
xAOD::TruthParticleAuxContainer* BuildTruthTaus::getTruthTauAuxContainer()
{
  if (!m_bTruthTauAvailable)
    return m_truthTausEvent.m_xTruthTauAuxContainer;
  else
  {
    ATH_MSG_WARNING("TruthTau auxiliary container was available from the event store and not rebuilt. Please get it from the event store");
    return nullptr;
  }
}

//______________________________________________________________________________
StatusCode BuildTruthTaus::beginEvent()
{
  m_truthTausEvent.m_valid = false;

  return StatusCode::SUCCESS;
}


StatusCode BuildTruthTaus::retrieveTruthTaus()
{
  return retrieveTruthTaus (m_truthTausEvent);
}


StatusCode BuildTruthTaus::retrieveTruthTaus(ITruthTausEvent& truthTausEvent) const
{
  return retrieveTruthTaus (dynamic_cast<TruthTausEvent&> (truthTausEvent));
}


StatusCode BuildTruthTaus::retrieveTruthTaus(TruthTausEvent& truthTausEvent) const
{
  if (truthTausEvent.m_valid)
    return StatusCode::SUCCESS;
  truthTausEvent.m_valid = true;

  if ( m_bTruthTauAvailable )
  {
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_sTruthTauContainerName))
      ATH_CHECK( evtStore()->retrieve(truthTausEvent.m_xTruthTauContainerConst,m_sTruthTauContainerName));
    else
    {
      ATH_MSG_INFO("TruthTaus container with name " << m_sTruthTauContainerName << " is not available, will generate the container for each event from TruthParticles container");
      m_bTruthTauAvailable = false;
    }
  }

  if ( m_bTruthMuonAvailable )
  {
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_sTruthMuonContainerName))
      ATH_CHECK(evtStore()->retrieve(truthTausEvent.m_xTruthMuonContainerConst,m_sTruthMuonContainerName));
    else
    {
      ATH_MSG_INFO("TruthMuons container with name " << m_sTruthMuonContainerName << " is not available, won't perform matching to truth muons");
      m_bTruthMuonAvailable = false;
    }
  }

  if ( m_bTruthElectronAvailable )
  {
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_sTruthElectronContainerName))
      ATH_CHECK(evtStore()->retrieve(truthTausEvent.m_xTruthElectronContainerConst,m_sTruthElectronContainerName));
    else
    {
      ATH_MSG_INFO("TruthElectrons container with name " << m_sTruthElectronContainerName << " is not available, won't perform matching to truth electrons");
      m_bTruthElectronAvailable = false;
    }
  }

  if ( m_bTruthJetAvailable )
  {
    if (evtStore()->contains<xAOD::JetContainer>(m_sTruthJetContainerName))
      ATH_CHECK(evtStore()->retrieve(truthTausEvent.m_xTruthJetContainerConst,m_sTruthJetContainerName));
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
      if ( evtStore()->retrieve(truthTausEvent.m_xTruthParticleContainer,m_sTruthParticlesContainerName).isSuccess() )
        return buildTruthTausFromTruthParticles(truthTausEvent);
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
StatusCode
BuildTruthTaus::buildTruthTausFromTruthParticles(TruthTausEvent& truthTausEvent) const
{
  if(truthTausEvent.m_xTruthTauContainer && !m_bWriteTruthTaus)
  {
    delete truthTausEvent.m_xTruthTauContainer;
    delete truthTausEvent.m_xTruthTauAuxContainer;
  }

  // Create the new containers
  truthTausEvent.m_xTruthTauContainer = new xAOD::TruthParticleContainer();
  truthTausEvent.m_xTruthTauAuxContainer = new xAOD::TruthParticleAuxContainer();
  truthTausEvent.m_xTruthTauContainer->setStore( truthTausEvent.m_xTruthTauAuxContainer );

  if (m_bWriteTruthTaus)
  {
    if ( evtStore()->record( truthTausEvent.m_xTruthTauContainer, m_sNewTruthTauContainerName ).isFailure() )
    {
      ATH_MSG_FATAL("Couldn't create truth tau container with key " << m_sNewTruthTauContainerName);
      return StatusCode::FAILURE;
    }
    if ( evtStore()->record( truthTausEvent.m_xTruthTauAuxContainer, m_sNewTruthTauContainerNameAux ).isFailure() )
    {
      ATH_MSG_FATAL("Couldn't create truth tau container with key " << m_sNewTruthTauContainerNameAux);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Recorded new TruthParticleContainer with key: " <<  m_sNewTruthTauContainerName);
  }

  for (auto xTruthParticle : *truthTausEvent.m_xTruthParticleContainer)
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
      static const SG::AuxElement::Decorator<unsigned int> decClassifierParticleType("classifierParticleType");
      static const SG::AuxElement::Decorator<unsigned int> decClassifierParticleOrigin("classifierParticleOrigin");
      decClassifierParticleType(*xTruthTau) = pClassification.first;
      decClassifierParticleOrigin(*xTruthTau) = pClassification.second;

      // create link to the original TruthParticle
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthParticle, *truthTausEvent.m_xTruthParticleContainer);
      static const SG::AuxElement::Accessor<ElementLink< xAOD::TruthParticleContainer > > accOriginalTruthParticle("originalTruthParticle");
      accOriginalTruthParticle(*xTruthTau) = lTruthParticleLink;

      truthTausEvent.m_xTruthTauContainer->push_back(xTruthTau);
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode BuildTruthTaus::examineTruthTau(const xAOD::TruthParticle& xTruthParticle) const
{
  // skip this tau if it has no decay vertex, should not happen
  if ( !xTruthParticle.hasDecayVtx() )
    return StatusCode::FAILURE;

  ATH_MSG_VERBOSE("looking for charged daughters of a truth tau");

  TauTruthInfo truthInfo;

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

  examineTruthTauDecay(xTruthParticle, truthInfo).ignore();

  if (truthInfo.m_bIsHadronicTau)
    ATH_MSG_VERBOSE(truthInfo.m_iNChargedDaughters << " prong hadronic truth tau was found with barcode "<<xTruthParticle.barcode());
  else
    ATH_MSG_VERBOSE(truthInfo.m_iNChargedDaughters << " prong leptonic truth tau was found with barcode "<<xTruthParticle.barcode());
  if ( truthInfo.m_iNChargedDaughters%2 == 0 )
  {
    ATH_MSG_WARNING("found tau with even multiplicity: " << truthInfo.m_iNChargedDaughters);
    printDecay(xTruthParticle);
  }

  static const SG::AuxElement::Decorator<double> decPtVis("pt_vis");
  static const SG::AuxElement::Decorator<double> decEtaVis("eta_vis");
  static const SG::AuxElement::Decorator<double> decPhiVis("phi_vis");
  static const SG::AuxElement::Decorator<double> accMVis("m_vis");

  static const SG::AuxElement::Decorator<size_t> decNumCharged("numCharged");
  static const SG::AuxElement::Decorator<size_t> decNumChargedPion("numChargedPion");
  static const SG::AuxElement::Decorator<size_t> decNumNeutral("numNeutral");
  static const SG::AuxElement::Decorator<size_t> accNumNeutralPion("numNeutralPion");

  decPtVis(xTruthParticle) = truthInfo.m_vTruthVisTLV.Pt();
  decEtaVis(xTruthParticle) = truthInfo.m_vTruthVisTLV.Eta();
  decPhiVis(xTruthParticle) = truthInfo.m_vTruthVisTLV.Phi();
  accMVis(xTruthParticle) = truthInfo.m_vTruthVisTLV.M();

  decNumCharged(xTruthParticle) = truthInfo.m_iNChargedDaughters;
  decNumChargedPion(xTruthParticle) = truthInfo.m_iNChargedPions;
  decNumNeutral(xTruthParticle) = truthInfo.m_iNNeutralPions+truthInfo.m_iNNeutralOthers;
  accNumNeutralPion(xTruthParticle) = truthInfo.m_iNNeutralPions;

  static const SG::AuxElement::Decorator<char> decIsHadronicTau("IsHadronicTau");
  decIsHadronicTau(xTruthParticle) = (char)truthInfo.m_bIsHadronicTau;

  if ( m_bWriteInvisibleFourMomentum )
  {
    TLorentzVector vTruthInvisTLV = xTruthParticle.p4() - truthInfo.m_vTruthVisTLV;
    static const SG::AuxElement::Decorator<double> decPtInvis("pt_invis");
    static const SG::AuxElement::Decorator<double> decEtaInvis("eta_invis");
    static const SG::AuxElement::Decorator<double> decPhiInvis("phi_invis");
    static const SG::AuxElement::Decorator<double> accMInvis("m_invis");
    decPtInvis(xTruthParticle)  = vTruthInvisTLV.Pt();
    decEtaInvis(xTruthParticle) = vTruthInvisTLV.Eta();
    decPhiInvis(xTruthParticle) = vTruthInvisTLV.Phi();
    accMInvis(xTruthParticle)   = vTruthInvisTLV.M();
  }

  if ( m_bWriteVisibleChargedFourMomentum )
  {
    static const SG::AuxElement::Decorator<double> decPtVisCharged("pt_vis_charged");
    static const SG::AuxElement::Decorator<double> decEtaVisCharged("eta_vis_charged");
    static const SG::AuxElement::Decorator<double> decPhiVisCharged("phi_vis_charged");
    static const SG::AuxElement::Decorator<double> accMVisCharged("m_vis_charged");
    decPtVisCharged(xTruthParticle)  = truthInfo.m_vTruthVisTLVCharged.Pt();
    decEtaVisCharged(xTruthParticle) = truthInfo.m_vTruthVisTLVCharged.Eta();
    decPhiVisCharged(xTruthParticle) = truthInfo.m_vTruthVisTLVCharged.Phi();
    accMVisCharged(xTruthParticle)   = truthInfo.m_vTruthVisTLVCharged.M();
  }

  if ( m_bWriteVisibleNeutralFourMomentum )
  {
    static const SG::AuxElement::Decorator<double> decPtVisNeutral("pt_vis_neutral");
    static const SG::AuxElement::Decorator<double> decEtaVisNeutral("eta_vis_neutral");
    static const SG::AuxElement::Decorator<double> decPhiVisNeutral("phi_vis_neutral");
    static const SG::AuxElement::Decorator<double> accMVisNeutral("m_vis_neutral");
    decPtVisNeutral(xTruthParticle)  = truthInfo.m_vTruthVisTLVNeutral.Pt();
    decEtaVisNeutral(xTruthParticle) = truthInfo.m_vTruthVisTLVNeutral.Eta();
    decPhiVisNeutral(xTruthParticle) = truthInfo.m_vTruthVisTLVNeutral.Phi();
    accMVisNeutral(xTruthParticle)   = truthInfo.m_vTruthVisTLVNeutral.M();
  }

  if ( m_bWriteDecayModeVector )
  {
    static const SG::AuxElement::Decorator<std::vector<int> > decDecayModeVector("DecayModeVector");
    decDecayModeVector(xTruthParticle) = truthInfo.m_vDecayMode;
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode BuildTruthTaus::examineTruthTauDecay (const xAOD::TruthParticle& xTruthParticle,
                                                 TauTruthInfo& truthInfo) const
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
        examineTruthTauDecay(*xTruthDaughter, truthInfo).ignore();
        continue;
      }
    }

    // only process stable particles
    if (xTruthDaughter->status() != 1 and xTruthDaughter->status() != 2)
      continue;

    // add pdgID to vector for decay mode classification
    truthInfo.m_vDecayMode.push_back(iPdgId);

    // if tau decays leptonically, indicated by an electron/muon neutrino then
    // it is not a hadronic decay
    if ( xTruthDaughter->isHadron() )
      truthInfo.m_bIsHadronicTau = true;

    // ignore neutrinos for further progress
    if ( xTruthDaughter->isNeutrino() )
    {
      ATH_MSG_VERBOSE("found neutrino decay particle with PdgId "<<iPdgId);
      continue;
    }

    // add momentum of non-neutrino particle to visible momentum
    truthInfo.m_vTruthVisTLV += xTruthDaughter->p4();
    if ( m_bWriteVisibleChargedFourMomentum )
      if ( xTruthDaughter->isCharged() )
        truthInfo.m_vTruthVisTLVCharged += xTruthDaughter->p4();
    if ( m_bWriteVisibleNeutralFourMomentum )
      if ( xTruthDaughter->isNeutral() )
        truthInfo.m_vTruthVisTLVNeutral += xTruthDaughter->p4();

    // only count charged decay particles
    if ( xTruthDaughter->isCharged() )
    {
      ATH_MSG_VERBOSE("found charged decay particle with PdgId "<<iPdgId);
      truthInfo.m_iNChargedDaughters++;
      // count charged pions
      if (iAbsPdgId==211) truthInfo.m_iNChargedPions++;
      else truthInfo.m_iNChargedOthers++;
    }
    else
    {
      ATH_MSG_VERBOSE("found neutral decay particle with PdgId "<<iPdgId);
      // count neutral pions
      if (iAbsPdgId==111) truthInfo.m_iNNeutralPions++;
      else truthInfo.m_iNNeutralOthers++;
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
