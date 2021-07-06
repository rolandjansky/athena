/**
 * @file DiTauTruthMatchingTool.cxx
 * @brief Class for ditau truth matching
 * @date 2021-02-18
 * 
 * @copyright Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 * 
 */

// Local include(s)
#include "TauAnalysisTools/DiTauTruthMatchingTool.h"

// Core include(s):
#include "AthLinks/ElementLink.h"
#include "TruthUtils/PIDHelpers.h"

// EDM include(s):
// #include "xAODTau/TauxAODHelpers.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

#include "MCTruthClassifier/MCTruthClassifier.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
DiTauTruthMatchingTool::DiTauTruthMatchingTool( const std::string& name )
  : BuildTruthTaus(name)
  , m_accPtVis("pt_vis")
  , m_accEtaVis("eta_vis")
  , m_accPhiVis("phi_vis")
  , m_accMVis("m_vis")
{
  declareProperty( "MaxDeltaR", m_dMaxDeltaR = .2);
}

//______________________________________________________________________________
DiTauTruthMatchingTool::~DiTauTruthMatchingTool( )
{

}

//______________________________________________________________________________
StatusCode DiTauTruthMatchingTool::initialize()
{
  ATH_MSG_INFO( "Initializing DiTauTruthMatchingTool" );
  if (DiTauTruthMatchingTool::BuildTruthTaus::initialize().isFailure())
  {
    ATH_MSG_FATAL("Failed initializing BuildTruthTaus");
    return StatusCode::FAILURE;
  };
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
void DiTauTruthMatchingTool::getTruth(const xAOD::DiTauJet& xDiTau)
{
  if (retrieveTruthTaus().isFailure())
    return;

  if (findTruthTau(xDiTau).isFailure())
    ATH_MSG_WARNING("There was a failure in finding the matched truth tau");

  return;
}

//______________________________________________________________________________
void DiTauTruthMatchingTool::applyTruthMatch(const xAOD::DiTauJet& xDiTau)
{
  // this function only exists to have the same interface functions like 
  // TauTruthMatchingTool
  getTruth(xDiTau);
  return;
}

//______________________________________________________________________________
void DiTauTruthMatchingTool::applyTruthMatch(const std::vector<const xAOD::DiTauJet*>& vDiTaus)
{
  for (auto xDiTau : vDiTaus)
    getTruth(*xDiTau);
  return;
}


////////////////////////////////////////////////////////////////////////////////
//                                 Private Part                               //
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
StatusCode DiTauTruthMatchingTool::findTruthTau(const xAOD::DiTauJet& xDiTau)
{
  // check if decorations were already added to the first passed tau
  if (!m_bIsTruthMatchedAvailable.isValid()) {
    bool avail = xDiTau.isAvailable<char>("IsTruthMatched");
    m_bIsTruthMatchedAvailable.set (avail);
  }
  if (*m_bIsTruthMatchedAvailable.ptr())
    return StatusCode::SUCCESS;

  if (m_bTruthTauAvailable)
    return checkTruthMatch(xDiTau, *m_truthTausEvent.m_xTruthTauContainerConst);
  else
    return checkTruthMatch(xDiTau, *m_truthTausEvent.m_xTruthTauContainer);
}

//______________________________________________________________________________
StatusCode DiTauTruthMatchingTool::checkTruthMatch (const xAOD::DiTauJet& xDiTau, const xAOD::TruthParticleContainer& xTruthTauContainer) const
{
  std::vector<const xAOD::TruthParticle*> vTruthMatch;
  std::vector<TruthMatchedParticleType> vTruthMatchedParticleType;

  xAOD::TruthParticleContainer xRemainingTruthTaus = xTruthTauContainer;

  static SG::AuxElement::Decorator<char> decIsTruthMatched("IsTruthMatched");
  static SG::AuxElement::Decorator<char> decIsTruthHadronic("IsTruthHadronic");
  static const SG::AuxElement::Decorator<char> decIsTruthHadMu("IsTruthHadMu");
  static const SG::AuxElement::Decorator<char> decIsTruthHadEl("IsTruthHadEl");
  static SG::AuxElement::ConstAccessor<int> accNSubjets("n_subjets");

  // set default values for each subjet
  for (int i = 0; i < accNSubjets(xDiTau); ++i)
  {
    const xAOD::TruthParticle* xTruthMatch = 0;
    TruthMatchedParticleType eTruthMatchedParticleType = Unknown;

    vTruthMatch.push_back(xTruthMatch);
    vTruthMatchedParticleType.push_back(eTruthMatchedParticleType);
  }

  // truthmatching for subjets:
  for (int i = 0; i < accNSubjets(xDiTau); ++i)
  {
    TLorentzVector vSubjetTLV;
    vSubjetTLV.SetPtEtaPhiE(xDiTau.subjetPt(i),
                        xDiTau.subjetEta(i),
                        xDiTau.subjetPhi(i),
                        xDiTau.subjetE(i));
    if ( truthMatch(vSubjetTLV,
                    xRemainingTruthTaus,
                    vTruthMatch.at(i),
                    vTruthMatchedParticleType.at(i)).isFailure() )
    {
      ATH_MSG_WARNING("There was a failure in matching truth taus with subjet " << i);
      return StatusCode::FAILURE;
    }
    if (vTruthMatch.at(i) && 
       (vTruthMatchedParticleType.at(i) == TruthHadronicTau || vTruthMatchedParticleType.at(i) == TruthLeptonicTau))
    {
      xRemainingTruthTaus.erase( std::find(xRemainingTruthTaus.begin(),
                                           xRemainingTruthTaus.end(),
                                           vTruthMatch.at(i)) );
    }
  }
  
  bool bTruthMatched = true;

  // create link to the original TruthParticle
  std::vector< ElementLink < xAOD::TruthParticleContainer > > vTruthLinks;
  for (int i = 0; i < accNSubjets(xDiTau); ++i)
  {
    const xAOD::TruthParticle* xTruthMatch = vTruthMatch.at(i);
    TruthMatchedParticleType eTruthMatchedParticleType = vTruthMatchedParticleType.at(i);
    if (xTruthMatch)
    {
      if (eTruthMatchedParticleType == TruthHadronicTau or eTruthMatchedParticleType == TruthLeptonicTau)
      {
        ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthMatch, xTruthTauContainer);
        vTruthLinks.push_back(lTruthParticleLink);  
      }
      else if (eTruthMatchedParticleType == TruthMuon)
      {
        ElementLink <xAOD::TruthParticleContainer> lTruthParticleLink(xTruthMatch, *m_truthTausEvent.m_xTruthMuonContainerConst);
        vTruthLinks.push_back(lTruthParticleLink);
      }
      else if (eTruthMatchedParticleType  == TruthElectron)
      {
        ElementLink <xAOD::TruthParticleContainer> lTruthParticleLink(xTruthMatch, *m_truthTausEvent.m_xTruthElectronContainerConst);
        vTruthLinks.push_back(lTruthParticleLink);
      }
    }
    else
    {
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink;
      vTruthLinks.push_back(lTruthParticleLink);  

      // ditau is not truth matched if one of the two leading subjets is not truth matched
      if (i == 0 || i == 1) bTruthMatched = false;
    }
  }
  
  xDiTau.auxdecor<std::vector<ElementLink<xAOD::TruthParticleContainer>>>("truthParticleLinks") = vTruthLinks;
  if (!m_bTruthTauAvailable)
  {
    xDiTau.auxdecor<std::vector<ElementLink<xAOD::TruthParticleContainer>>>("TruthTaus") = vTruthLinks;
  }
  
  ElementLink<xAOD::TruthParticleContainer> lTruthLeptonLink;
  static const SG::AuxElement::Decorator<unsigned int> decClassifierParticleType("classifierParticleTypeTruthLepton");
  static const SG::AuxElement::Decorator<unsigned int> decClassifierParticleOrigin("classifierParticleOriginTruthLepton");
  static const SG::AuxElement::Decorator<ElementLink<xAOD::TruthParticleContainer>> decTruthLeptonLink("truthLeptonLink");
  
  int mcTruthType = MCTruthPartClassifier::ParticleType::Unknown;
  int mcTruthOrigin = MCTruthPartClassifier::ParticleOrigin::NonDefined;
  static const SG::AuxElement::ConstAccessor<int> accTruthType("truthType");
  static const SG::AuxElement::ConstAccessor<int> accTruthOrigin("truthOrigin");
  if(xDiTau.isAvailable<ElementLink<xAOD::ElectronContainer>>("elLink") &&
     xDiTau.isAvailable<ElementLink<xAOD::MuonContainer>>("muonLink"))
    ATH_MSG_ERROR("Links to reco electron and reco muon available for one ditau candidate.");
  if(xDiTau.isAvailable<ElementLink<xAOD::ElectronContainer>>("elLink")){
    static const SG::AuxElement::ConstAccessor<ElementLink<xAOD::ElectronContainer>> accElLink("elLink");
    const xAOD::Electron* pElectron = *accElLink(xDiTau);
    if ((pElectron->isAvailable<int>("truthType") && pElectron->isAvailable<int>("truthOrigin")))
    {
      mcTruthType = accTruthType(*pElectron);
      mcTruthOrigin = accTruthOrigin(*pElectron);
    }
    lTruthLeptonLink = checkTruthLepton(pElectron);
  }
  if(xDiTau.isAvailable<ElementLink<xAOD::MuonContainer>>("muonLink")){
    static const SG::AuxElement::ConstAccessor<ElementLink<xAOD::MuonContainer>> accMuLink("muonLink");
    const xAOD::Muon* pMuon = *accMuLink(xDiTau);
    if (pMuon->isAvailable<int>("truthType") && pMuon->isAvailable<int>("truthOrigin"))
    {
      mcTruthType = accTruthType(*pMuon);
      mcTruthOrigin = accTruthOrigin(*pMuon);
    }
    lTruthLeptonLink = checkTruthLepton(pMuon);
  }

  decIsTruthHadEl(xDiTau) = (char)(mcTruthType == MCTruthPartClassifier::ParticleType::IsoElectron && accNSubjets(xDiTau) != 0 && vTruthMatchedParticleType[0] == TruthHadronicTau);
  decIsTruthHadMu(xDiTau) = (char)(mcTruthType == MCTruthPartClassifier::ParticleType::IsoMuon && accNSubjets(xDiTau) != 0 && vTruthMatchedParticleType[0] == TruthHadronicTau);
  decClassifierParticleType(xDiTau) = mcTruthType;
  decClassifierParticleOrigin(xDiTau) = mcTruthOrigin;
  decTruthLeptonLink(xDiTau) = lTruthLeptonLink;

  // the ditau candidate should have at least 2 subjets to be truth matched
  if ( accNSubjets(xDiTau) < 2) {
    decIsTruthMatched(xDiTau) = (char)false;
    decIsTruthHadronic(xDiTau) = (char)false;
    return StatusCode::SUCCESS;
  }

  decIsTruthMatched(xDiTau) = (char)bTruthMatched;
  if (bTruthMatched)
  {

    // ditau is hadronic if two leading subjets are truth matched with hadronic decay
    decIsTruthHadronic(xDiTau) = (char)(vTruthMatchedParticleType[0]==TruthHadronicTau
                                        && vTruthMatchedParticleType[1]==TruthHadronicTau );
  }
  else 
    decIsTruthHadronic(xDiTau) = (char)false;
  
  if (decIsTruthHadronic(xDiTau))
  {
    static const SG::AuxElement::Decorator<double> decTruthLeadPt("TruthVisLeadPt");
    static const SG::AuxElement::Decorator<double> decTruthSubleadPt("TruthVisSubleadPt");
    static const SG::AuxElement::Decorator<double> decTruthDeltaR("TruthVisDeltaR");

    TLorentzVector tlvTruthTau1;
    TLorentzVector tlvTruthTau2;
    tlvTruthTau1.SetPtEtaPhiE(m_accPtVis(*(*vTruthLinks.at(0))),
                              m_accEtaVis(*(*vTruthLinks.at(0))),
                              m_accPhiVis(*(*vTruthLinks.at(0))),
                              m_accMVis(*(*vTruthLinks.at(0))));
    tlvTruthTau2.SetPtEtaPhiE(m_accPtVis(*(*vTruthLinks.at(1))),
                              m_accEtaVis(*(*vTruthLinks.at(1))),
                              m_accPhiVis(*(*vTruthLinks.at(1))),
                              m_accMVis(*(*vTruthLinks.at(1))));

    decTruthLeadPt(xDiTau) = std::max(tlvTruthTau1.Pt(), tlvTruthTau2.Pt());
    decTruthSubleadPt(xDiTau) = std::min(tlvTruthTau1.Pt(), tlvTruthTau2.Pt());
    decTruthDeltaR(xDiTau) = tlvTruthTau1.DeltaR(tlvTruthTau2);
  }
  
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
ElementLink<xAOD::TruthParticleContainer> DiTauTruthMatchingTool::checkTruthLepton(const xAOD::IParticle* pLepton) const{
  ElementLink<xAOD::TruthParticleContainer> truthParticleLink;
  if(!pLepton->isAvailable<ElementLink<xAOD::TruthParticleContainer>>("truthParticleLink")){
    return truthParticleLink;
  }
  static SG::AuxElement::ConstAccessor<ElementLink<xAOD::TruthParticleContainer>> accTruthParticleLink("truthParticleLink");
  truthParticleLink = accTruthParticleLink(*pLepton);
  return truthParticleLink;
}

//______________________________________________________________________________
StatusCode DiTauTruthMatchingTool::truthMatch(const TLorentzVector& vSubjetTLV,
                                              const xAOD::TruthParticleContainer& xTruthTauContainer,
                                              const xAOD::TruthParticle* &xTruthMatch,
                                              TruthMatchedParticleType &eTruthMatchedParticleType) const
{

  double dPtMax = 0;
  for (auto xTruthTauIt : xTruthTauContainer)
  {
    TLorentzVector vTruthVisTLV;
    vTruthVisTLV.SetPtEtaPhiM(m_accPtVis(*xTruthTauIt),
                              m_accEtaVis(*xTruthTauIt),
                              m_accPhiVis(*xTruthTauIt),
                              m_accMVis(*xTruthTauIt));
    if (vSubjetTLV.DeltaR(vTruthVisTLV) <= m_dMaxDeltaR)
    {
      if (vTruthVisTLV.Pt()<dPtMax)
        continue;
      static SG::AuxElement::ConstAccessor<char> accIsHadronicTau("IsHadronicTau");
      if ((bool)accIsHadronicTau(*xTruthTauIt))
        eTruthMatchedParticleType = TruthHadronicTau;
      else
        continue; // don't let leptonic taus steal truthmatch just by chance

      xTruthMatch = xTruthTauIt;
      dPtMax = vTruthVisTLV.Pt();
      break;
    }
  }

  if (!xTruthMatch and m_truthTausEvent.m_xTruthMuonContainerConst)
  {
    double dPtMax = 0;
    for (auto xTruthMuonIt : *m_truthTausEvent.m_xTruthMuonContainerConst)
    {
      if (vSubjetTLV.DeltaR(xTruthMuonIt->p4()) <= m_dMaxDeltaR)
      {
        if (xTruthMuonIt->pt()<dPtMax)
          continue;
        eTruthMatchedParticleType = TruthMuon;

        xTruthMatch = xTruthMuonIt;
        dPtMax = xTruthMuonIt->pt();
      }
    }
  }

  if (!xTruthMatch and m_truthTausEvent.m_xTruthElectronContainerConst)
  {
    double dPtMax = 0;
    for (auto xTruthElectronIt : *m_truthTausEvent.m_xTruthElectronContainerConst)
    {
      if (vSubjetTLV.DeltaR(xTruthElectronIt->p4()) <= m_dMaxDeltaR)
      {
        if (xTruthElectronIt->pt()<dPtMax)
          continue;
        eTruthMatchedParticleType = TruthElectron;
        xTruthMatch = xTruthElectronIt;
        dPtMax = xTruthElectronIt->pt();
      }
    }
  }

  return StatusCode::SUCCESS;
}
