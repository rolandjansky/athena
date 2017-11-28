/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s)
#include "TauAnalysisTools/DiTauTruthMatchingTool.h"

// Core include(s):
#include "AthLinks/ElementLink.h"
#include "TruthUtils/PIDHelpers.h"

// EDM include(s):
// #include "xAODTau/TauxAODHelpers.h"

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
  if (m_bIsData)
    return;

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
  if (!m_bIsTruthMatchedAvailableChecked)
  {
    m_bIsTruthMatchedAvailable = xDiTau.isAvailable<char>("IsTruthMatched");
    m_bIsTruthMatchedAvailableChecked = true;
    if (m_bIsTruthMatchedAvailable)
    {
      ATH_MSG_DEBUG("IsTruthMatched decoration is available on first tau processed, switched of rerun for further taus.");
      ATH_MSG_DEBUG("If a truth matching needs to be redone, please pass a shallow copy of the original tau.");
    }
  }
  if (m_bIsTruthMatchedAvailable)
    return StatusCode::SUCCESS;

  if (m_bTruthTauAvailable)
    return checkTruthMatch(xDiTau, *m_xTruthTauContainerConst);
  else
    return checkTruthMatch(xDiTau, *m_xTruthTauContainer);
}

//______________________________________________________________________________
StatusCode DiTauTruthMatchingTool::checkTruthMatch (const xAOD::DiTauJet& xDiTau, const xAOD::TruthParticleContainer& xTruthTauContainer) const
{
  std::vector<const xAOD::TruthParticle*> vTruthMatch;
  std::vector<TruthMatchedParticleType> vTruthMatchedParticleType;

  xAOD::TruthParticleContainer xRemainingTruthTaus = xTruthTauContainer;

  static SG::AuxElement::Decorator<char> decIsTruthMatched("IsTruthMatched");
  static SG::AuxElement::Decorator<char> decIsTruthHadronic("IsTruthHadronic");
  static SG::AuxElement::ConstAccessor<int> accNSubjets("n_subjets");

  // set default values for each subjet
  for (int i = 0; i < accNSubjets(xDiTau); ++i)
  {
    const xAOD::TruthParticle* xTruthMatch = 0;
    TruthMatchedParticleType eTruthMatchedParticleType = Unknown;

    vTruthMatch.push_back(xTruthMatch);
    vTruthMatchedParticleType.push_back(eTruthMatchedParticleType);
  }

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

  // the ditau candidate should have at least 2 subjets to be truth matched
  if ( accNSubjets(xDiTau) < 2) {
    decIsTruthMatched(xDiTau) = (char)false;
    decIsTruthHadronic(xDiTau) = (char)false;
    return StatusCode::SUCCESS;;
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
        ElementLink <xAOD::TruthParticleContainer> lTruthParticleLink(xTruthMatch, *m_xTruthMuonContainerConst);
        vTruthLinks.push_back(lTruthParticleLink);
      }
      else if (eTruthMatchedParticleType  == TruthElectron)
      {
        ElementLink <xAOD::TruthParticleContainer> lTruthParticleLink(xTruthMatch, *m_xTruthElectronContainerConst);
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

  decIsTruthMatched(xDiTau) = (char)bTruthMatched;
  if (bTruthMatched)
  {
    // ditau is hadronic if two leading subjets are truth matched with hadronic decay
    decIsTruthHadronic(xDiTau) = (char)(vTruthMatchedParticleType[0]==TruthHadronicTau && vTruthMatchedParticleType[1]==TruthHadronicTau );
  }
  else 
    decIsTruthHadronic(xDiTau) = (char)false;

  if (decIsTruthHadronic(xDiTau))
  {
    static SG::AuxElement::Decorator<double> decTruthLeadPt("TruthVisLeadPt");
    static SG::AuxElement::Decorator<double> decTruthSubleadPt("TruthVisSubleadPt");
    static SG::AuxElement::Decorator<double> decTruthDeltaR("TruthVisDeltaR");
    
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
        eTruthMatchedParticleType = TruthLeptonicTau;

      xTruthMatch = xTruthTauIt;
      dPtMax = vTruthVisTLV.Pt();
      break;
    }
  }

  if (!xTruthMatch and m_xTruthMuonContainerConst)
  {
    double dPtMax = 0;
    for (auto xTruthMuonIt : *m_xTruthMuonContainerConst)
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

  if (!xTruthMatch and m_xTruthElectronContainerConst)
  {
    double dPtMax = 0;
    for (auto xTruthElectronIt : *m_xTruthElectronContainerConst)
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
