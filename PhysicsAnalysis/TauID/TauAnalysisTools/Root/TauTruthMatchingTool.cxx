/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s)
#include "TauAnalysisTools/TauTruthMatchingTool.h"

// Core include(s):
#include "AthLinks/ElementLink.h"
#include "TruthUtils/PIDHelpers.h"

// EDM include(s):
#include "xAODTau/TauxAODHelpers.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauTruthMatchingTool::TauTruthMatchingTool( const std::string& name )
  : BuildTruthTaus(name)
  , m_accPtVis("pt_vis")
  , m_accEtaVis("eta_vis")
  , m_accPhiVis("phi_vis")
  , m_accMVis("m_vis")
{
  declareProperty( "MaxDeltaR", m_dMaxDeltaR = .2);
}

//______________________________________________________________________________
TauTruthMatchingTool::~TauTruthMatchingTool( )
{

}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::initialize()
{
  ATH_MSG_INFO( "Initializing TauTruthMatchingTool" );
  if (TauTruthMatchingTool::BuildTruthTaus::initialize().isFailure())
  {
    ATH_MSG_FATAL("Failed initializing BuildTruthTaus");
    return StatusCode::FAILURE;
  };
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
const xAOD::TruthParticle* TauTruthMatchingTool::getTruth(const xAOD::TauJet& xTau)
{
  if (m_bIsData)
    return nullptr;

  if (retrieveTruthTaus().isFailure())
    return nullptr;

  if (findTruthTau(xTau).isFailure())
    ATH_MSG_WARNING("There was a failure in finding the matched truth tau");

  // if matched to a truth tau return its pointer, else return a null pointer
  static SG::AuxElement::ConstAccessor<char> accIsTruthMatched("IsTruthMatched");
  if ((bool)accIsTruthMatched(xTau))
  {
    if (m_bWriteTruthTaus or m_bTruthTauAvailable)
    {
      static SG::AuxElement::ConstAccessor< ElementLink< xAOD::TruthParticleContainer >  > accTruthTau("truthParticleLink");
      if (accTruthTau(xTau).isValid())
      {
        return *accTruthTau(xTau);
      }
      else
      {
        ATH_MSG_WARNING("ElementLink to TruthParticle is not valid.");
        return nullptr;
      }
    }
    else
    {
      static SG::AuxElement::ConstAccessor< const xAOD::TruthParticle* > accTruthTau("TruthTau");
      return accTruthTau(xTau);
    }
  }
  return nullptr;
}

//______________________________________________________________________________
const xAOD::TruthParticle* TauTruthMatchingTool::applyTruthMatch(const xAOD::TauJet& xTau)
{
  // this function only exists for historical reasons
  return getTruth(xTau);
}

//______________________________________________________________________________
std::vector<const xAOD::TruthParticle*> TauTruthMatchingTool::applyTruthMatch(const std::vector<const xAOD::TauJet*>& vTaus)
{
  std::vector<const xAOD::TruthParticle*> vTruths;
  for (auto xTau : vTaus)
    vTruths.push_back(getTruth(*xTau));
  return vTruths;
}

////////////////////////////////////////////////////////////////////////////////
//                              Wrapper functions                             //
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
TLorentzVector TauTruthMatchingTool::getTruthTauP4Vis(const xAOD::TauJet& xTau)
{
  const xAOD::TruthParticle* xTruthTau = getTruth(xTau);
  TLorentzVector vTLV;
  if (xTruthTau == nullptr)
  {
    ATH_MSG_INFO("no truth particle was found, returning TLorentzVector with all values equal to 0");
    return vTLV;
  }

  vTLV.SetPtEtaPhiM(
    m_accPtVis(*xTruthTau),
    m_accEtaVis(*xTruthTau),
    m_accPhiVis(*xTruthTau),
    m_accMVis(*xTruthTau));
  return vTLV;
}

//______________________________________________________________________________
TLorentzVector TauTruthMatchingTool::getTruthTauP4Vis(const xAOD::TruthParticle& xTruthTau)
{
  TLorentzVector vTLV;
  if (!xTruthTau.isAvailable<double>("pt_vis"))
    return vTLV;
  vTLV.SetPtEtaPhiM(
    m_accPtVis(xTruthTau),
    m_accEtaVis(xTruthTau),
    m_accPhiVis(xTruthTau),
    m_accMVis(xTruthTau));
  return vTLV;
}

//______________________________________________________________________________
TLorentzVector TauTruthMatchingTool::getTruthTauP4Prompt(const xAOD::TauJet& xTau)
{
  const xAOD::TruthParticle* xTruthTau = getTruth(xTau);
  TLorentzVector vTLV;
  if (xTruthTau == nullptr)
  {
    ATH_MSG_INFO("no truth particle was found, returning TLorentzVector with all values equal to 0");
    return vTLV;
  }

  static SG::AuxElement::ConstAccessor<double> accPtPrompt("pt_prompt");
  static SG::AuxElement::ConstAccessor<double> accEtaPrompt("eta_prompt");
  static SG::AuxElement::ConstAccessor<double> accPhiPrompt("phi_prompt");
  static SG::AuxElement::ConstAccessor<double> accMPrompt("m_prompt");
  vTLV.SetPtEtaPhiM(
    accPtPrompt(*xTruthTau),
    accEtaPrompt(*xTruthTau),
    accPhiPrompt(*xTruthTau),
    accMPrompt(*xTruthTau));
  return vTLV;
}

//______________________________________________________________________________
TLorentzVector TauTruthMatchingTool::getTruthTauP4Prompt(const xAOD::TruthParticle& xTruthTau)
{
  TLorentzVector vTLV;
  if (!xTruthTau.isAvailable<double>("pt_prompt"))
    return vTLV;

  static SG::AuxElement::ConstAccessor<double> accPtPrompt("pt_prompt");
  static SG::AuxElement::ConstAccessor<double> accEtaPrompt("eta_prompt");
  static SG::AuxElement::ConstAccessor<double> accPhiPrompt("phi_prompt");
  static SG::AuxElement::ConstAccessor<double> accMPrompt("m_prompt");
  vTLV.SetPtEtaPhiM(
    accPtPrompt(xTruthTau),
    accEtaPrompt(xTruthTau),
    accPhiPrompt(xTruthTau),
    accMPrompt(xTruthTau));
  return vTLV;
}

//______________________________________________________________________________
TLorentzVector TauTruthMatchingTool::getTruthTauP4Invis(const xAOD::TauJet& xTau)
{
  const xAOD::TruthParticle* xTruthTau = getTruth(xTau);
  TLorentzVector vTLV;
  if (xTruthTau == nullptr)
  {
    ATH_MSG_INFO("no truth particle was found, returning TLorentzVector with all values equal to 0");
    return vTLV;
  }

  static SG::AuxElement::ConstAccessor<double> accPtInvis("pt_invis");
  static SG::AuxElement::ConstAccessor<double> accEtaInvis("eta_invis");
  static SG::AuxElement::ConstAccessor<double> accPhiInvis("phi_invis");
  static SG::AuxElement::ConstAccessor<double> accMInvis("m_invis");
  vTLV.SetPtEtaPhiM(
    accPtInvis(*xTruthTau),
    accEtaInvis(*xTruthTau),
    accPhiInvis(*xTruthTau),
    accMInvis(*xTruthTau));
  return vTLV;
}

//______________________________________________________________________________
TLorentzVector TauTruthMatchingTool::getTruthTauP4Invis(const xAOD::TruthParticle& xTruthTau)
{
  TLorentzVector vTLV;
  if (!xTruthTau.isAvailable<double>("pt_invis"))
    return vTLV;

  static SG::AuxElement::ConstAccessor<double> accPtInvis("pt_invis");
  static SG::AuxElement::ConstAccessor<double> accEtaInvis("eta_invis");
  static SG::AuxElement::ConstAccessor<double> accPhiInvis("phi_invis");
  static SG::AuxElement::ConstAccessor<double> accMInvis("m_invis");
  vTLV.SetPtEtaPhiM(
    accPtInvis(xTruthTau),
    accEtaInvis(xTruthTau),
    accPhiInvis(xTruthTau),
    accMInvis(xTruthTau));
  return vTLV;
}

TauAnalysisTools::TruthMatchedParticleType TauTruthMatchingTool::getTruthParticleType(const xAOD::TauJet& xTau)
{
  const xAOD::TruthParticle* xTruthParticle = xAOD::TauHelpers::getTruthParticle(&xTau);
  if (xTruthParticle)
  {
    if (xTruthParticle->isTau())
    {
      static SG::AuxElement::ConstAccessor<char> accIsHadronicTau("IsHadronicTau");
      if ((bool)accIsHadronicTau(*xTruthParticle))
        return TruthHadronicTau;
      else
        return TruthLeptonicTau;
    }
    if (xTruthParticle->isMuon())
      return TruthMuon;
    if (xTruthParticle->isElectron())
      return TruthElectron;
  }
  // TODO: use const xAOD::Jet* xTruthJet = xAOD::TauHelpers::getLink<xAOD::Jet>(&xTau, "truthJetLink");
  // currently it is unavailable as templated class is not in icc file
  static SG::AuxElement::ConstAccessor< ElementLink< xAOD::JetContainer > > accTruthJetLink("truthJetLink");
  const ElementLink< xAOD::JetContainer > lTruthParticleLink = accTruthJetLink(xTau);
  if (lTruthParticleLink.isValid())
    return TruthJet;

  return Unknown;
}

//______________________________________________________________________________
int TauTruthMatchingTool::getNTauDecayParticles(const xAOD::TauJet& xTau, int iPdgId, bool bCompareAbsoluteValues)
{
  const xAOD::TruthParticle* xTruthTau = getTruth(xTau);
  if (xTruthTau == nullptr)
  {
    ATH_MSG_DEBUG("no truth particle was found, return 0");
    return 0;
  }
  if (!xTruthTau->isAvailable<std::vector<int>>("DecayModeVector"))
  {
    ATH_MSG_INFO("found truth particle is not a truth tau, return 0");
    return 0;
  }
  return getNTauDecayParticles(*xTruthTau,iPdgId, bCompareAbsoluteValues);
}

//______________________________________________________________________________
int TauTruthMatchingTool::getNTauDecayParticles(const xAOD::TruthParticle& xTruthTau, int iPdgId, bool bCompareAbsoluteValues)
{
  int iNum = 0;
  if (!xTruthTau.isAvailable<std::vector<int>>("DecayModeVector"))
  {
    ATH_MSG_WARNING("passed truth particle is not a truth tau, return 0");
    return 0;
  }

  static SG::AuxElement::ConstAccessor<std::vector<int> > accDecayModeVector("DecayModeVector");
  for(auto iPdgId2 : accDecayModeVector(xTruthTau))
    if (!bCompareAbsoluteValues)
    {
      if (iPdgId2 == iPdgId) iNum++;
    }
    else
    {
      if (std::abs(iPdgId2) == std::abs(iPdgId)) iNum++;
    }
  return iNum;
}

//______________________________________________________________________________
xAOD::TauJetParameters::DecayMode TauTruthMatchingTool::getDecayMode(const xAOD::TauJet& xTau)
{
  const xAOD::TruthParticle* xTruthTau = getTruth(xTau);
  if (xTruthTau == nullptr)
  {
    ATH_MSG_DEBUG("no truth particle was found, return Mode_Error");
    return xAOD::TauJetParameters::Mode_Error;
  }
  if (!xTruthTau->isAvailable<size_t>("numCharged"))
  {
    ATH_MSG_INFO("found truth particle is not a truth tau, return Mode_Error");
    return xAOD::TauJetParameters::Mode_Error;
  }
  return getDecayMode(*xTruthTau);
}

//______________________________________________________________________________
xAOD::TauJetParameters::DecayMode TauTruthMatchingTool::getDecayMode(const xAOD::TruthParticle& xTruthTau)
{
  if (!(xTruthTau.isAvailable<size_t>("numCharged")))
  {
    ATH_MSG_WARNING("passed truth particle is not a truth tau, return Mode_Error");
    return xAOD::TauJetParameters::Mode_Error;
  }

  int iCharged = getNTauDecayParticles(xTruthTau,MC::PID::PIPLUS, true) + getNTauDecayParticles(xTruthTau,MC::PID::KPLUS, true);
  int iNeutral = getNTauDecayParticles(xTruthTau,MC::PID::PI0, true);
  if (iCharged == 1)
  {
    if (iNeutral == 0) return xAOD::TauJetParameters::DecayMode::Mode_1p0n;
    if (iNeutral == 1) return xAOD::TauJetParameters::DecayMode::Mode_1p1n;
    if (iNeutral >= 2) return xAOD::TauJetParameters::DecayMode::Mode_1pXn;
  }
  else if (iCharged == 3)
  {
    if (iNeutral == 0) return xAOD::TauJetParameters::DecayMode::Mode_3p0n;
    if (iNeutral >= 1) return xAOD::TauJetParameters::DecayMode::Mode_3pXn;
  }

  if (iCharged == 2 or iCharged == 4 or iCharged == 5)
    return xAOD::TauJetParameters::DecayMode::Mode_Other;
  if (iCharged == 0 or iCharged >=6)
    return xAOD::TauJetParameters::DecayMode::Mode_NotSet;

  // if you got here, something should have gone wrong
  return xAOD::TauJetParameters::DecayMode::Mode_Error;
}

////////////////////////////////////////////////////////////////////////////////
//                                 Private Part                               //
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::findTruthTau(const xAOD::TauJet& xTau)
{
  // check if decorations were already added to the first passed tau
  if (!m_bIsTruthMatchedAvailableChecked)
  {
    m_bIsTruthMatchedAvailable = xTau.isAvailable<char>("IsTruthMatched");
    m_bIsTruthMatchedAvailableChecked = true;
    if (m_bIsTruthMatchedAvailable)
    {
      ATH_MSG_DEBUG("IsTruthMatched decoration is available on first tau processed, switched of rerun for further taus.");
      ATH_MSG_DEBUG("If a truth matching needs to be redone, please pass a shallow copy of the original tau.");
    }
  }
  if (m_bIsTruthMatchedAvailable)
    return StatusCode::SUCCESS;

  // only search for truth taus once

  // need to be reviewed, commenting out for now

  // if (!xTau.isAvailable<char>("IsTruthMatched"))
  // temporary fix for SUSY derivations
  // if (!xTau.isAvailable<char>("IsTruthMatched"))
  // || !xTau.isAvailable< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink")
  // || (xTau.auxdata<char>("IsTruthMatched") && xTau.auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink") == NULL ))
  // {
  if (m_bTruthTauAvailable)
    return checkTruthMatch(xTau, *m_xTruthTauContainerConst);
  else
    return checkTruthMatch(xTau, *m_xTruthTauContainer);
  // }

  // return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::checkTruthMatch (const xAOD::TauJet& xTau, const xAOD::TruthParticleContainer& xTruthTauContainer) const
{
  const xAOD::TruthParticle* xTruthMatch = 0;
  const xAOD::Jet* xTruthJetMatch = 0;
  TruthMatchedParticleType eTruthMatchedParticleType = Unknown;
  static SG::AuxElement::Decorator<char> decIsTruthMatched("IsTruthMatched");
  static SG::AuxElement::Decorator< ElementLink< xAOD::JetContainer > > decTruthJetLink("truthJetLink");
  
  double dPtMax = 0;
  for (auto xTruthTauIt : xTruthTauContainer)
  {
    TLorentzVector vTruthVisTLV;
    vTruthVisTLV.SetPtEtaPhiM(m_accPtVis(*xTruthTauIt),
                              m_accEtaVis(*xTruthTauIt),
                              m_accPhiVis(*xTruthTauIt),
                              m_accMVis(*xTruthTauIt));
    if (xTau.p4().DeltaR(vTruthVisTLV) <= m_dMaxDeltaR)
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

  if (!xTruthMatch and m_bTruthMuonAvailable and m_xTruthMuonContainerConst)
  {
    double dPtMax = 0;
    for (auto xTruthMuonIt : *m_xTruthMuonContainerConst)
    {
      if (xTau.p4().DeltaR(xTruthMuonIt->p4()) <= m_dMaxDeltaR)
      {
        if (xTruthMuonIt->pt()<dPtMax)
          continue;
        eTruthMatchedParticleType = TruthMuon;

        xTruthMatch = xTruthMuonIt;
        dPtMax = xTruthMuonIt->pt();
      }
    }
  }

  if (!xTruthMatch and m_bTruthElectronAvailable and m_xTruthElectronContainerConst)
  {
    double dPtMax = 0;
    for (auto xTruthElectronIt : *m_xTruthElectronContainerConst)
    {
      if (xTau.p4().DeltaR(xTruthElectronIt->p4()) <= m_dMaxDeltaR)
      {
        if (xTruthElectronIt->pt()<dPtMax)
          continue;
        eTruthMatchedParticleType = TruthElectron;
        xTruthMatch = xTruthElectronIt;
        dPtMax = xTruthElectronIt->pt();
      }
    }
  }

  if (m_bTruthJetAvailable and m_xTruthJetContainerConst)
  {
    double dPtMax = 0;
    for (auto xTruthJetIt : *m_xTruthJetContainerConst)
    {
      if (xTau.p4().DeltaR(xTruthJetIt->p4()) <= m_dMaxDeltaR)
      {
        if (xTruthJetIt->pt()<dPtMax)
          continue;
        xTruthJetMatch = xTruthJetIt;
        dPtMax = xTruthJetIt->pt();
      }
    }
  }

  if (xTruthMatch)
    decIsTruthMatched(xTau) = (char)true;
  else
    decIsTruthMatched(xTau) = (char)false;

  if (xTruthJetMatch)
  {
    ElementLink < xAOD::JetContainer > lTruthParticleLink(xTruthJetMatch, *m_xTruthJetContainerConst);
    decTruthJetLink(xTau) = lTruthParticleLink;
  }
  else
  {
    ElementLink < xAOD::JetContainer > lTruthParticleLink;
    decTruthJetLink(xTau) = lTruthParticleLink;
  }


  if (!m_bWriteTruthTaus and !m_bTruthTauAvailable)
  {
    static SG::AuxElement::Decorator< const xAOD::TruthParticle* > decTruthTau("TruthTau");
    decTruthTau(xTau) = xTruthMatch;
    return StatusCode::SUCCESS;
  }
  static SG::AuxElement::Decorator< ElementLink< xAOD::TruthParticleContainer > > decTruthTau("TruthTau");
  static SG::AuxElement::Decorator<ElementLink< xAOD::TruthParticleContainer >> decTruthParticleLink("truthParticleLink");

  // create link to the original TruthParticle
  if (xTruthMatch)
  {
    decIsTruthMatched(xTau) = (char)true;
    if (eTruthMatchedParticleType == TruthHadronicTau or eTruthMatchedParticleType == TruthLeptonicTau)
    {
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthMatch, xTruthTauContainer);
      decTruthParticleLink(xTau) = lTruthParticleLink;
      if (!m_bTruthTauAvailable)
        decTruthTau(xTau) = lTruthParticleLink;
    }
    else if (eTruthMatchedParticleType == TruthMuon)
    {
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthMatch, *m_xTruthMuonContainerConst);
      decTruthParticleLink(xTau) = lTruthParticleLink;
    }
    else if (eTruthMatchedParticleType == TruthElectron)
    {
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthMatch, *m_xTruthElectronContainerConst);
      decTruthParticleLink(xTau) = lTruthParticleLink;
    }
  }
  else
  {
    decIsTruthMatched(xTau) = (char)false;
    ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink;
    decTruthParticleLink(xTau) = lTruthParticleLink;
  }

  return StatusCode::SUCCESS;
}
