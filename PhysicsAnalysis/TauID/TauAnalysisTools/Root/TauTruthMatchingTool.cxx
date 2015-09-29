/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TauAnalysisTools/TauTruthMatchingTool.h>

// Core include(s):
#include "AthLinks/ElementLink.h"

// EDM include(s):
#include "xAODTruth/TruthVertex.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauTruthMatchingTool::TauTruthMatchingTool( const std::string& name )
  : AsgTool(name)
  , m_xTruthParticleContainer(0)
  , m_xTruthTauContainerConst(0)
  , m_xTruthMuonContainerConst(0)
  , m_xTruthElectronContainerConst(0)
  , m_xTruthJetContainerConst(0)
  , m_xTruthTauContainer(0)
  , m_xTruthTauAuxContainer(0)
  , m_dMaxDeltaR(0.2)
  , m_bWriteTruthTaus(false)
  , m_sNewTruthTauContainerName("TruthTaus")
  , m_sNewTruthTauContainerNameAux("TruthTausAux.")
  , m_sTruthTauContainerName("TruthTaus")
  , m_sTruthMuonContainerName("TruthMuons")
  , m_sTruthElectronContainerName("TruthElectrons")
  , m_sTruthParticlesContainerName("TruthParticles")
  , m_bTruthTauAvailable(true)
  , m_bTruthMuonAvailable(true)
  , m_bTruthElectronAvailable(true)
  , m_bTruthJetAvailable(true)
  , m_bWriteVisibleChargedFourMomentum(false)
  , m_bWriteVisibleNeutralFourMomentum(false)
  , m_bWriteDecayModeVector(true)
{
  declareProperty( "MaxDeltaR", m_dMaxDeltaR = .2);
  declareProperty( "WriteTruthTaus", m_bWriteTruthTaus = false);

  // container names
  declareProperty( "NewTruthTauContainerName", m_sNewTruthTauContainerName = "TruthTaus");
  declareProperty( "TruthTauContainerName", m_sTruthTauContainerName = "TruthTaus");
  declareProperty( "TruthMuonContainerName", m_sTruthMuonContainerName = "TruthMuons");
  declareProperty( "TruthElectronContainerName", m_sTruthElectronContainerName = "TruthElectrons");
  declareProperty( "TruthJetContainerName", m_sTruthJetContainerName = "AntiKt4TruthJets");
  declareProperty( "TruthParticlesContainerName", m_sTruthParticlesContainerName = "TruthParticles");
  declareProperty( "WriteVisibleChargedFourMomentum", m_bWriteVisibleChargedFourMomentum = false);
  declareProperty( "WriteVisibleNeutralFourMomentum", m_bWriteVisibleNeutralFourMomentum = false);
  declareProperty( "WriteDecayModeVector", m_bWriteDecayModeVector = true);

  // deprecated properties
  declareProperty( "OptimizeForReco", m_bOptimizeForReco = false);
  declareProperty( "SampleType", m_iSampleType = -1);
}

//______________________________________________________________________________
TauTruthMatchingTool::~TauTruthMatchingTool( )
{

}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::initialize()
{
  ATH_MSG_INFO( "Initialising TauTruthMatchingTool" );
  m_sNewTruthTauContainerNameAux = m_sNewTruthTauContainerName + "Aux.";

  if (m_bOptimizeForReco)
    ATH_MSG_WARNING("The property OptimizeForReco is not used at the moment");
  if (m_iSampleType != -1)
    ATH_MSG_WARNING("The property SampleType is deprecated and will be removed");

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::createTruthTauContainer()
{
  ATH_MSG_FATAL("This function is deprecated. The TruthTau Container will be created automatically passing truth particle container via setTruthParticleContainer or passing Event via initializeEvent. \nFor further information please refer to the README:\nhttps://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauTruthMatchingTool.rst\nReturn null pointer");
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
xAOD::TruthParticleContainer* TauTruthMatchingTool::getTruthTauContainer()
{
  if (!m_bTruthTauAvailable)
    return m_xTruthTauContainer;
  else
  {
    ATH_MSG_WARNING("TruthTau container was available from the event store and not rebuilt. Please get it from the event store");
    return 0;
  }
}

//______________________________________________________________________________
xAOD::TruthParticleAuxContainer* TauTruthMatchingTool::getTruthTauAuxContainer()
{
  if (!m_bTruthTauAvailable)
    return m_xTruthTauAuxContainer;
  else
  {
    ATH_MSG_WARNING("TruthTau auxiliary container was available from the event store and not rebuilt. Please get it from the event store");
    return 0;
  }
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

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::setTruthParticleContainer(const xAOD::TruthParticleContainer* xTruthParticleContainer)
{
  m_xTruthParticleContainer = xTruthParticleContainer;
  // should be false by default, but better to be sure
  m_bTruthTauAvailable = false;
  buildTruthTausFromTruthParticles();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::initializeEvent()
{
  return retrieveTruthTaus();
}

//______________________________________________________________________________
const xAOD::TruthParticle* TauTruthMatchingTool::getTruth(const xAOD::TauJet& xTau) const
{
  if (findTruthTau(xTau).isFailure())
    ATH_MSG_WARNING("There was a failure in finding the matched truth tau");

  // if matched to a truth tau return its pointer, else return a null pointer
  if ((bool)xTau.auxdata<char>("IsTruthMatched"))
  {
    if (m_bWriteTruthTaus or m_bTruthTauAvailable)
      return *(xTau.auxdata < ElementLink< xAOD::TruthParticleContainer > > ("truthParticleLink"));
    else
      return xTau.auxdata < const xAOD::TruthParticle* > ("TruthTau");
  }
  return nullptr;
}

//______________________________________________________________________________
TLorentzVector TauTruthMatchingTool::getTruthTauP4Vis(const xAOD::TauJet& xTau) const
{
  const xAOD::TruthParticle* xTruthTau = getTruth(xTau);
  TLorentzVector vTLV;
  if (xTruthTau == nullptr)
  {
    ATH_MSG_INFO("no truth particle was found, returning TLorentzVector with all values equal to 0");
    return vTLV;
  }

  vTLV.SetPtEtaPhiM(
    xTruthTau->auxdata<double>("pt_vis"),
    xTruthTau->auxdata<double>("eta_vis"),
    xTruthTau->auxdata<double>("phi_vis"),
    xTruthTau->auxdata<double>("m_vis"));
  return vTLV;
}

//______________________________________________________________________________
TLorentzVector TauTruthMatchingTool::getTruthTauP4Vis(const xAOD::TruthParticle& xTruthTau) const
{
  TLorentzVector vTLV;
  if (!xTruthTau.isAvailable<std::vector<int>>("pt_vis"))
    return vTLV;
  vTLV.SetPtEtaPhiM(
    xTruthTau.auxdata<double>("pt_vis"),
    xTruthTau.auxdata<double>("eta_vis"),
    xTruthTau.auxdata<double>("phi_vis"),
    xTruthTau.auxdata<double>("m_vis"));
  return vTLV;
}

//______________________________________________________________________________
TLorentzVector TauTruthMatchingTool::getTruthTauP4Prompt(const xAOD::TauJet& xTau) const
{
  const xAOD::TruthParticle* xTruthTau = getTruth(xTau);
  TLorentzVector vTLV;
  if (xTruthTau == nullptr)
  {
    ATH_MSG_INFO("no truth particle was found, returning TLorentzVector with all values equal to 0");
    return vTLV;
  }

  vTLV.SetPtEtaPhiM(
    xTruthTau->auxdata<double>("pt_prompt"),
    xTruthTau->auxdata<double>("eta_prompt"),
    xTruthTau->auxdata<double>("phi_prompt"),
    xTruthTau->auxdata<double>("m_prompt"));
  return vTLV;
}

//______________________________________________________________________________
TLorentzVector TauTruthMatchingTool::getTruthTauP4Prompt(const xAOD::TruthParticle& xTruthTau) const
{
  TLorentzVector vTLV;
  if (!xTruthTau.isAvailable<std::vector<int>>("pt_vis"))
    return vTLV;
  vTLV.SetPtEtaPhiM(
    xTruthTau.auxdata<double>("pt_prompt"),
    xTruthTau.auxdata<double>("eta_prompt"),
    xTruthTau.auxdata<double>("phi_prompt"),
    xTruthTau.auxdata<double>("m_prompt"));
  return vTLV;
}

//______________________________________________________________________________
int TauTruthMatchingTool::getNTauDecayParticles(const xAOD::TauJet& xTau, int iPdgId, bool bCompareAbsoluteValues) const
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

  for(auto _iPdgId : xTruthTau.auxdata<std::vector<int>>("DecayModeVector"))
    if (!bCompareAbsoluteValues)
    {
      if (_iPdgId == iPdgId) iNum++;
    }
    else
    {
      if (fabs(_iPdgId) == fabs(iPdgId)) iNum++;
    }
  return iNum;
}

#ifdef XAODTAU_VERSIONS_TAUJET_V2_H
//______________________________________________________________________________
xAOD::TauJetParameters::DecayMode TauTruthMatchingTool::getDecayMode(const xAOD::TauJet& xTau) const
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

  int iCharged = getNTauDecayParticles(xTruthTau,MC::PID::PIPLUS, true) + getNTauDecayParticles(xTruthTau,MC::PID::KPLUS, true);;
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
#endif // XAODTAU_VERSIONS_TAUJET_V2_H

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
StatusCode TauTruthMatchingTool::findTruthTau(const xAOD::TauJet& xTau) const
{
  // only search for truth taus once
  // if (!xTau.isAvailable<char>("IsTruthMatched"))
  // temporary fix for SUSY derivations
  if (!xTau.isAvailable<char>("IsTruthMatched")
      || !xTau.isAvailable< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink")
      || (xTau.auxdata<char>("IsTruthMatched") && xTau.auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink") == NULL ))
  {
    if (m_bTruthTauAvailable)
      return checkTruthMatch(xTau, *m_xTruthTauContainerConst);
    else
      return checkTruthMatch(xTau, *m_xTruthTauContainer);
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::retrieveTruthTaus()
{
  if ( m_bTruthTauAvailable )
  {
    if ( evtStore()->retrieve(m_xTruthTauContainerConst,m_sTruthTauContainerName).isSuccess() )
      ATH_MSG_VERBOSE("TruthTaus container found");
    else
    {
      ATH_MSG_INFO("TruthTaus container is not available, will generate the container for each event from TruthParticles container");
      m_bTruthTauAvailable = false;
    }
  }

  if ( m_bTruthMuonAvailable )
  {
    if ( evtStore()->retrieve(m_xTruthMuonContainerConst,m_sTruthMuonContainerName).isSuccess() )
      ATH_MSG_VERBOSE("TruthMuons container found");
    else
    {
      ATH_MSG_INFO("TruthMuons container is not available, won't perform matching to truth muons");
      m_bTruthMuonAvailable = false;
    }
  }

  if ( m_bTruthElectronAvailable )
  {
    if ( evtStore()->retrieve(m_xTruthElectronContainerConst,m_sTruthElectronContainerName).isSuccess() )
      ATH_MSG_VERBOSE("TruthElectrons container found");
    else
    {
      ATH_MSG_INFO("TruthElectrons container is not available, won't perform matching to truth electrons");
      m_bTruthElectronAvailable = false;
    }
  }

  if ( m_bTruthJetAvailable )
  {
    if ( evtStore()->retrieve(m_xTruthJetContainerConst,m_sTruthJetContainerName).isSuccess() )
      ATH_MSG_VERBOSE("TruthJets container found");
    else
    {
      ATH_MSG_INFO("TruthJets container is not available, won't perform matching to truth jets");
      m_bTruthJetAvailable = false;
    }
  }

  // go here if TruthTaus was not found in m_bTruthTauAvailable if-block
  if ( !m_bTruthTauAvailable )
  {
    if ( evtStore()->retrieve(m_xTruthParticleContainer,m_sTruthParticlesContainerName).isSuccess() )
      return buildTruthTausFromTruthParticles();
    else
    {
      ATH_MSG_FATAL("TruthParticles container is not available but needed for building truth taus");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::buildTruthTausFromTruthParticles()
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
      ATH_MSG_FATAL("Couldn't create truth tau container with key " << m_sNewTruthTauContainerName);
    if ( evtStore()->record( m_xTruthTauAuxContainer, m_sNewTruthTauContainerNameAux ).isFailure() )
      ATH_MSG_FATAL("Couldn't create truth tau container with key " << m_sNewTruthTauContainerNameAux);
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

      // create link to the original TruthParticle
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthParticle, *m_xTruthParticleContainer);
      xTruthTau->auxdata< ElementLink< xAOD::TruthParticleContainer > >("originalTruthParticle" ) = lTruthParticleLink;

      m_xTruthTauContainer->push_back(xTruthTau);
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::checkTruthMatch (const xAOD::TauJet& xTau, const xAOD::TruthParticleContainer& xTruthTauContainer) const
{
  const xAOD::TruthParticle* xTruthMatch = 0;
  const xAOD::Jet* xTruthJetMatch = 0;
  TruthMatchedParticleType eTruthMatchedParticleType = Unknown;


  double dPtMax = 0;
  for (auto xTruthTauIt : xTruthTauContainer)
  {
    TLorentzVector vTruthVisTLV;
    vTruthVisTLV.SetPtEtaPhiM(xTruthTauIt->auxdata<double>("pt_vis"),
                              xTruthTauIt->auxdata<double>("eta_vis"),
                              xTruthTauIt->auxdata<double>("phi_vis"),
                              xTruthTauIt->auxdata<double>("m_vis"));
    if (xTau.p4().DeltaR(vTruthVisTLV) <= m_dMaxDeltaR)
    {
      if (vTruthVisTLV.Pt()<dPtMax)
        continue;
      if ((bool)xTruthTauIt->auxdata<char>("IsHadronicTau"))
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

  if (!xTruthMatch and m_xTruthElectronContainerConst)
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

  if (m_xTruthJetContainerConst)
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
    xTau.auxdecor<char>("IsTruthMatched") = (char)true;
  else
    xTau.auxdecor<char>("IsTruthMatched") = (char)false;

  if (xTruthJetMatch)
  {
    ElementLink < xAOD::JetContainer > lTruthParticleLink(xTruthJetMatch, *m_xTruthJetContainerConst);
    xTau.auxdecor< ElementLink< xAOD::JetContainer > >("truthJetLink" ) = lTruthParticleLink;
  }
  else
  {
    ElementLink < xAOD::JetContainer > lTruthParticleLink;
    xTau.auxdecor< ElementLink< xAOD::JetContainer > >("truthJetLink" ) = lTruthParticleLink;
  }


  if (!m_bWriteTruthTaus and !m_bTruthTauAvailable)
  {
    xTau.auxdecor< const xAOD::TruthParticle* > ("TruthTau" ) = xTruthMatch;
    return StatusCode::SUCCESS;
  }

  // create link to the original TruthParticle
  if (xTruthMatch)
  {
    xTau.auxdecor<char>("IsTruthMatched") = (char)true;
    if (eTruthMatchedParticleType == TruthHadronicTau or eTruthMatchedParticleType == TruthLeptonicTau)
    {
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthMatch, xTruthTauContainer);
      xTau.auxdecor< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink" ) = lTruthParticleLink;
      if (!m_bTruthTauAvailable)
        xTau.auxdecor< ElementLink< xAOD::TruthParticleContainer > >("TruthTau" ) = lTruthParticleLink;
    }
    else if (eTruthMatchedParticleType == TruthMuon)
    {
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthMatch, *m_xTruthMuonContainerConst);
      xTau.auxdecor< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink" ) = lTruthParticleLink;
    }
    else if (eTruthMatchedParticleType == TruthElectron)
    {
      ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink(xTruthMatch, *m_xTruthElectronContainerConst);
      xTau.auxdecor< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink" ) = lTruthParticleLink;
    }
  }
  else
  {
    xTau.auxdecor<char>("IsTruthMatched") = (char)false;
    ElementLink < xAOD::TruthParticleContainer > lTruthParticleLink;
    xTau.auxdecor< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink" ) = lTruthParticleLink;
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::examineTruthTau(const xAOD::TruthParticle& xTruthParticle)
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

  if (m_bIsHadronicTau)
    ATH_MSG_VERBOSE(m_iNChargedDaughters << " prong hadronic truth tau was found with barcode "<<xTruthParticle.barcode());
  else
    ATH_MSG_VERBOSE(m_iNChargedDaughters << " prong leptonic truth tau was found with barcode "<<xTruthParticle.barcode());
  if ( m_iNChargedDaughters%2 == 0 )
  {
    ATH_MSG_WARNING("found tau with even multiplicity: " << m_iNChargedDaughters);
    printDecay(xTruthParticle);
  }

  xTruthParticle.auxdecor<double>("pt_vis")  = m_vTruthVisTLV.Pt();
  xTruthParticle.auxdecor<double>("eta_vis") = m_vTruthVisTLV.Eta();
  xTruthParticle.auxdecor<double>("phi_vis") = m_vTruthVisTLV.Phi();
  xTruthParticle.auxdecor<double>("m_vis")   = m_vTruthVisTLV.M();

  xTruthParticle.auxdecor<size_t>("numCharged") = m_iNChargedDaughters;
  xTruthParticle.auxdecor<size_t>("numChargedPion") = m_iNChargedPions;
  xTruthParticle.auxdecor<size_t>("numNeutral") = m_iNNeutralPions+m_iNNeutralOthers;
  xTruthParticle.auxdecor<size_t>("numNeutralPion") = m_iNNeutralPions;

  xTruthParticle.auxdecor<char>("IsHadronicTau") = (char)m_bIsHadronicTau;

  if ( m_bWriteVisibleChargedFourMomentum )
  {
    xTruthParticle.auxdecor<double>("pt_vis_charged")  = m_vTruthVisTLVCharged.Pt();
    xTruthParticle.auxdecor<double>("eta_vis_charged") = m_vTruthVisTLVCharged.Eta();
    xTruthParticle.auxdecor<double>("phi_vis_charged") = m_vTruthVisTLVCharged.Phi();
    xTruthParticle.auxdecor<double>("m_vis_charged")   = m_vTruthVisTLVCharged.M();
  }

  if ( m_bWriteVisibleNeutralFourMomentum )
  {
    xTruthParticle.auxdecor<double>("pt_vis_neutral")  = m_vTruthVisTLVNeutral.Pt();
    xTruthParticle.auxdecor<double>("eta_vis_neutral") = m_vTruthVisTLVNeutral.Eta();
    xTruthParticle.auxdecor<double>("phi_vis_neutral") = m_vTruthVisTLVNeutral.Phi();
    xTruthParticle.auxdecor<double>("m_vis_neutral")   = m_vTruthVisTLVNeutral.M();
  }

  if ( m_bWriteDecayModeVector )
    xTruthParticle.auxdecor<std::vector<int>>("DecayModeVector") = m_vDecayMode;

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::examineTruthTauDecay(const xAOD::TruthParticle& xTruthParticle)
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

void TauTruthMatchingTool::printDecay(const xAOD::TruthParticle& xTruthParticle, int depth) const
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
