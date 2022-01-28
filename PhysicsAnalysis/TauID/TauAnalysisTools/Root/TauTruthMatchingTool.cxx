/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s)
#include "TauAnalysisTools/TauTruthMatchingTool.h"
#include "TauAnalysisTools/HelperFunctions.h"

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
  declareProperty( "MaxDeltaR", m_dMaxDeltaR = 0.2);
}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::initialize()
{
  ATH_MSG_INFO( "Initializing TauTruthMatchingTool" );
  if (TauTruthMatchingTool::BuildTruthTaus::initialize().isFailure())
  {
    ATH_MSG_FATAL("Failed initializing BuildTruthTaus");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
std::unique_ptr<TauTruthMatchingTool::ITruthTausEvent> TauTruthMatchingTool::getEvent() const
{
  auto truthTausEvent = std::make_unique<TruthTausEvent>();
  if (retrieveTruthTaus(*truthTausEvent).isFailure()) {
    truthTausEvent.reset();
  }
  return truthTausEvent;
}

//______________________________________________________________________________
const xAOD::TruthParticle* TauTruthMatchingTool::getTruth(const xAOD::TauJet& xTau)
{
  return getTruth (xTau, m_truthTausEvent);
}


const xAOD::TruthParticle* TauTruthMatchingTool::getTruth(const xAOD::TauJet& xTau,
							  ITruthTausEvent& itruthTausEvent) const
{
  TruthTausEvent& truthTausEvent = dynamic_cast<TruthTausEvent&> (itruthTausEvent);

  if (retrieveTruthTaus(truthTausEvent).isFailure())
    return nullptr;

  if (findTruthTau(xTau, truthTausEvent).isFailure())
    ATH_MSG_WARNING("There was a failure in finding the matched truth tau");

  static const SG::AuxElement::ConstAccessor<char> accIsTruthMatched("IsTruthMatched");
  static const SG::AuxElement::ConstAccessor< ElementLink< xAOD::TruthParticleContainer >  > accTruthParticleLink("truthParticleLink");

  // derivations may drop IsTruthMatched, redecorate using truthParticleLink (this assumes links to truth leptons are preserved, i.e. TruthTaus, TruthElectron, TruthMuon)
  if ( !(*m_bIsTruthMatchedAvailable.ptr()) && (*m_bIsTruthParticleLinkAvailable.ptr())) {
    ATH_MSG_DEBUG("TauJetContainer has truthParticleLink available while IsTruthMatched not available. Re-evaluate IsTruthMatched");
    static const SG::AuxElement::Decorator<char> decIsTruthMatched("IsTruthMatched");
    if (accTruthParticleLink(xTau)) {
      decIsTruthMatched(xTau) = (char)true;
    } else {
      decIsTruthMatched(xTau) = (char)false;
    }
  }
  
  // if matched to a truth particle return its pointer, else return a null pointer
  if ((bool)accIsTruthMatched(xTau))
    {
    if (m_bWriteTruthTaus or m_bTruthTauAvailable)
    {
      if (accTruthParticleLink(xTau).isValid())
      {
        return *accTruthParticleLink(xTau);
      }
      else
      {
        ATH_MSG_WARNING("ElementLink to TruthParticle is not valid.");
        return nullptr;
      }
    }
    else
    {
      static const SG::AuxElement::ConstAccessor< const xAOD::TruthParticle* > accTruthTau("TruthTau");
      return accTruthTau(xTau);
    }
  }
  return nullptr;
}

//______________________________________________________________________________
// FIXME: remove this method at the next occasion
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
TLorentzVector TauTruthMatchingTool::getTruthTauP4Vis(const xAOD::TruthParticle& xTruthTau) const
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

  static const SG::AuxElement::ConstAccessor<double> accPtPrompt("pt_prompt");
  static const SG::AuxElement::ConstAccessor<double> accEtaPrompt("eta_prompt");
  static const SG::AuxElement::ConstAccessor<double> accPhiPrompt("phi_prompt");
  static const SG::AuxElement::ConstAccessor<double> accMPrompt("m_prompt");
  vTLV.SetPtEtaPhiM(
    accPtPrompt(*xTruthTau),
    accEtaPrompt(*xTruthTau),
    accPhiPrompt(*xTruthTau),
    accMPrompt(*xTruthTau));
  return vTLV;
}

//______________________________________________________________________________
TLorentzVector TauTruthMatchingTool::getTruthTauP4Prompt(const xAOD::TruthParticle& xTruthTau) const
{
  TLorentzVector vTLV;
  if (!xTruthTau.isAvailable<double>("pt_prompt"))
    return vTLV;

  static const SG::AuxElement::ConstAccessor<double> accPtPrompt("pt_prompt");
  static const SG::AuxElement::ConstAccessor<double> accEtaPrompt("eta_prompt");
  static const SG::AuxElement::ConstAccessor<double> accPhiPrompt("phi_prompt");
  static const SG::AuxElement::ConstAccessor<double> accMPrompt("m_prompt");
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

  static const SG::AuxElement::ConstAccessor<double> accPtInvis("pt_invis");
  static const SG::AuxElement::ConstAccessor<double> accEtaInvis("eta_invis");
  static const SG::AuxElement::ConstAccessor<double> accPhiInvis("phi_invis");
  static const SG::AuxElement::ConstAccessor<double> accMInvis("m_invis");
  vTLV.SetPtEtaPhiM(
    accPtInvis(*xTruthTau),
    accEtaInvis(*xTruthTau),
    accPhiInvis(*xTruthTau),
    accMInvis(*xTruthTau));
  return vTLV;
}

//______________________________________________________________________________
TLorentzVector TauTruthMatchingTool::getTruthTauP4Invis(const xAOD::TruthParticle& xTruthTau) const
{
  TLorentzVector vTLV;
  if (!xTruthTau.isAvailable<double>("pt_invis"))
    return vTLV;

  static const SG::AuxElement::ConstAccessor<double> accPtInvis("pt_invis");
  static const SG::AuxElement::ConstAccessor<double> accEtaInvis("eta_invis");
  static const SG::AuxElement::ConstAccessor<double> accPhiInvis("phi_invis");
  static const SG::AuxElement::ConstAccessor<double> accMInvis("m_invis");
  vTLV.SetPtEtaPhiM(
    accPtInvis(xTruthTau),
    accEtaInvis(xTruthTau),
    accPhiInvis(xTruthTau),
    accMInvis(xTruthTau));
  return vTLV;
}

TauAnalysisTools::TruthMatchedParticleType TauTruthMatchingTool::getTruthParticleType(const xAOD::TauJet& xTau)
{
  return TauAnalysisTools::getTruthParticleType(xTau);
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
int TauTruthMatchingTool::getNTauDecayParticles(const xAOD::TruthParticle& xTruthTau, int iPdgId, bool bCompareAbsoluteValues) const
{
  int iNum = 0;
  if (!xTruthTau.isAvailable<std::vector<int>>("DecayModeVector"))
  {
    ATH_MSG_WARNING("passed truth particle is not a truth tau, return 0");
    return 0;
  }

  static const SG::AuxElement::ConstAccessor<std::vector<int> > accDecayModeVector("DecayModeVector");
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
xAOD::TauJetParameters::DecayMode TauTruthMatchingTool::getDecayMode(const xAOD::TruthParticle& xTruthTau) const
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
StatusCode TauTruthMatchingTool::findTruthTau(const xAOD::TauJet& xTau,
                                              TruthTausEvent& truthTausEvent) const
{
  // check if decorations were already added to the first passed tau
  if (!m_bIsTruthMatchedAvailable.isValid()) {
    bool avail = xTau.isAvailable<char>("IsTruthMatched") ;
    m_bIsTruthMatchedAvailable.set (avail);
  }
  // check if decorations were already added to the first passed tau
  if (!m_bIsTruthParticleLinkAvailable.isValid()) {
    bool avail = xTau.isAvailable<ElementLink< xAOD::TruthParticleContainer >>("truthParticleLink");
    m_bIsTruthParticleLinkAvailable.set (avail);
  }

  if (*m_bIsTruthMatchedAvailable.ptr() || *m_bIsTruthParticleLinkAvailable.ptr()) {
    return StatusCode::SUCCESS;
  }

  // only search for truth taus once

  // need to be reviewed

  if (m_bTruthTauAvailable)
    return checkTruthMatch(xTau, *truthTausEvent.m_xTruthTauContainerConst, truthTausEvent);
  else
    return checkTruthMatch(xTau, *truthTausEvent.m_xTruthTauContainer, truthTausEvent);
}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::checkTruthMatch (const xAOD::TauJet& xTau, const xAOD::TruthParticleContainer& xTruthTauContainer, const TruthTausEvent& truthTausEvent) const
{
  const xAOD::TruthParticle* xTruthMatch = nullptr;
  const xAOD::Jet* xTruthJetMatch = nullptr;
  TruthMatchedParticleType eTruthMatchedParticleType = Unknown;
  static const SG::AuxElement::Decorator<char> decIsTruthMatched("IsTruthMatched");
  static const SG::AuxElement::Decorator< ElementLink< xAOD::JetContainer > > decTruthJetLink("truthJetLink");
  
  double dPtMax = 0.;
  for (auto xTruthTauIt : xTruthTauContainer)
  {
    TLorentzVector vTruthVisTLV;
    vTruthVisTLV.SetPtEtaPhiM(m_accPtVis(*xTruthTauIt),
                              m_accEtaVis(*xTruthTauIt),
                              m_accPhiVis(*xTruthTauIt),
                              m_accMVis(*xTruthTauIt));
    if (xTau.p4().DeltaR(vTruthVisTLV) <= m_dMaxDeltaR)
    {
      // FIXME: useless as we break the loop on the first match
      if (vTruthVisTLV.Pt()<dPtMax)
        continue;
      static const SG::AuxElement::ConstAccessor<char> accIsHadronicTau("IsHadronicTau");
      if ((bool)accIsHadronicTau(*xTruthTauIt))
        eTruthMatchedParticleType = TruthHadronicTau;
      else
        eTruthMatchedParticleType = TruthLeptonicTau;

      xTruthMatch = xTruthTauIt;
      dPtMax = vTruthVisTLV.Pt();
      break;
    }
  }

  if (!xTruthMatch and truthTausEvent.m_xTruthMuonContainerConst)
  {
    dPtMax = 0.;
    for (auto xTruthMuonIt : *truthTausEvent.m_xTruthMuonContainerConst)
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

  if (!xTruthMatch and truthTausEvent.m_xTruthElectronContainerConst)
  {
    dPtMax = 0.;
    for (auto xTruthElectronIt : *truthTausEvent.m_xTruthElectronContainerConst)
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

  if (truthTausEvent.m_xTruthJetContainerConst)
  {
    dPtMax = 0.;
    for (auto xTruthJetIt : *truthTausEvent.m_xTruthJetContainerConst)
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
    ElementLink < xAOD::JetContainer > lTruthParticleLink(xTruthJetMatch, *truthTausEvent.m_xTruthJetContainerConst);
    decTruthJetLink(xTau) = lTruthParticleLink;
  }
  else
  {
    ElementLink < xAOD::JetContainer > lTruthParticleLink;
    decTruthJetLink(xTau) = lTruthParticleLink;
  }

  if (!m_bWriteTruthTaus and !m_bTruthTauAvailable)
  {
    static const SG::AuxElement::Decorator< const xAOD::TruthParticle* > decTruthTau("TruthTau");
    decTruthTau(xTau) = xTruthMatch;
    return StatusCode::SUCCESS;
  }

  // create link to the original TruthParticle
  static const SG::AuxElement::Decorator< ElementLink< xAOD::TruthParticleContainer > > decTruthParticleLink("truthParticleLink");

  if (xTruthMatch)
  {
    if (eTruthMatchedParticleType == TruthHadronicTau or eTruthMatchedParticleType == TruthLeptonicTau)
    {
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthMatch, xTruthTauContainer);
      decTruthParticleLink(xTau) = lTruthParticleLink;
    }
    else if (eTruthMatchedParticleType == TruthMuon)
    {
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthMatch, *truthTausEvent.m_xTruthMuonContainerConst);
      decTruthParticleLink(xTau) = lTruthParticleLink;
    }
    else if (eTruthMatchedParticleType == TruthElectron)
    {
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthMatch, *truthTausEvent.m_xTruthElectronContainerConst);
      decTruthParticleLink(xTau) = lTruthParticleLink;
    }
  }
  else
  {
    ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink;
    decTruthParticleLink(xTau) = lTruthParticleLink;
  }

  return StatusCode::SUCCESS;
}
