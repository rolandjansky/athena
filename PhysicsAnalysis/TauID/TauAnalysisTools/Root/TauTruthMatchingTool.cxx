/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TauAnalysisTools/TauTruthMatchingTool.h>

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauTruthMatchingTool::TauTruthMatchingTool( const std::string& name )
  : AsgTool(name)
  , m_xTruthParticleContainer(0)
  , m_xTruthParticleAuxContainer(0)
  , m_xTruthTauContainer(0)
  , m_xTruthTauAuxContainer(0)
{
  declareProperty( "SampleType", m_iSampleType = PYTHIA);
  declareProperty( "MaxDeltaR", m_dMaxDeltaR = .2);
}

//______________________________________________________________________________
TauTruthMatchingTool::~TauTruthMatchingTool( )
{

}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::initialize()
{
  ATH_MSG_INFO( "Initialising TauTruthMatchingTool" );
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::createTruthTauContainer()
{
  // fill m_vTruthTaus with all truth taus in TruthParticleContainer
  findTruthTaus();

  // create a new TruthTauContainer
  m_xTruthTauContainer = new xAOD::TruthParticleContainer();
  m_xTruthTauAuxContainer = new xAOD::TruthParticleAuxContainer();
  m_xTruthTauContainer->setStore(m_xTruthTauAuxContainer);

  for (auto xTruthTau : m_vTruthTaus)
  {
    // make a non-const copy
    xAOD::TruthParticle* xTruthTauCopy = new xAOD::TruthParticle();
    xTruthTauCopy->makePrivateStore( *xTruthTau );

    // create link to the original TruthParticle
    ElementLink<xAOD::TruthParticleContainer> link(xTruthTau, *m_xTruthParticleContainer);
    xTruthTauCopy->auxdata< ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" ) = link;

    // add copy to the new TruthTauContainer
    m_xTruthTauContainer->push_back(xTruthTauCopy);
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
const xAOD::TruthParticle* TauTruthMatchingTool::applyTruthMatch(const xAOD::TauJet& xTau)
{
  findTruthTaus();
  return checkTruthMatch(xTau);
}

//______________________________________________________________________________
std::vector<const xAOD::TruthParticle*> TauTruthMatchingTool::applyTruthMatch(const std::vector<const xAOD::TauJet*>& vTaus)
{
  findTruthTaus();
  std::vector<const xAOD::TruthParticle*> vTruthTaus;
  for (auto xTau : vTaus)
  {
    vTruthTaus.push_back(checkTruthMatch(*xTau));
  }
  return vTruthTaus;
}

//______________________________________________________________________________
StatusCode TauTruthMatchingTool::setTruthParticleContainer(const xAOD::TruthParticleContainer* xTruthParticleContainer)
{
  m_xTruthParticleContainer = xTruthParticleContainer;
  return StatusCode::SUCCESS;
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
void TauTruthMatchingTool::findTruthTaus()
{
  ATH_MSG_VERBOSE("looking for truth taus");
  m_vTruthTaus.clear();
  if (!m_xTruthParticleContainer)
  {
    ATH_MSG_FATAL("You have to pass the xAOD::TruthParticleContainer before via: setTruthParticleContainer(xTruthParticleContainer)");
    return;
  }
  xAOD::TruthParticleContainer::const_iterator iItrTruthParticle = m_xTruthParticleContainer->begin();
  xAOD::TruthParticleContainer::const_iterator iEndTruthParticle = m_xTruthParticleContainer->end();
  for( ; iItrTruthParticle != iEndTruthParticle ; ++iItrTruthParticle)
  {

    if ((*iItrTruthParticle)->isTau())
    {
      examineTruthTau(**iItrTruthParticle);

      if (!(*iItrTruthParticle)->auxdata<bool>("IsHadronicTau"))
        continue;
      m_vTruthTaus.push_back(*iItrTruthParticle);
    }
  }
}

//______________________________________________________________________________
const xAOD::TruthParticle* TauTruthMatchingTool::checkTruthMatch (const xAOD::TauJet& xTau) const
{
  xTau.auxdecor<int>("TruthCharge") = -1111;
  xTau.auxdecor<size_t>("TruthProng") = 1111;
  xTau.auxdecor<bool>("IsTruthMatched") = false;
  for (auto xTruthTau : m_vTruthTaus)
  {
    TLorentzVector vTruthVisTLV;
    vTruthVisTLV.SetPtEtaPhiM(xTruthTau->auxdata<double>("pt_vis"),
                              xTruthTau->auxdata<double>("eta_vis"),
                              xTruthTau->auxdata<double>("phi_vis"),
                              xTruthTau->auxdata<double>("m_vis"));
    if (xTau.p4().DeltaR(vTruthVisTLV) <= m_dMaxDeltaR)
    {
      xTau.auxdecor<bool>("IsTruthMatched") = true;
      xTau.auxdecor<size_t>("TruthProng") = xTruthTau->auxdata<size_t>("NTracks");
      xTau.auxdecor<int>("TruthCharge") = (xTruthTau->pdgId() > 0) ? -1 : +1;
      return xTruthTau;
    }
  }
  return NULL;
}

//______________________________________________________________________________
void TauTruthMatchingTool::examineTruthTau(const xAOD::TruthParticle& xTruthParticle) const
{
  size_t iNChargedPions = 0;
  size_t iNNeutralPions = 0;
  size_t iNChargedOthers = 0;
  size_t iNNeutralOthers = 0;
  size_t iNChargedDaughters = 0;
  ATH_MSG_VERBOSE("looking for charged daughters of a truth tau");
  xTruthParticle.auxdecor<bool>("IsHadronicTau") = false;

  TLorentzVector vTruthVisTLV;
  xTruthParticle.auxdecor<double>("pt_vis") = 0;
  xTruthParticle.auxdecor<double>("eta_vis") = 0;
  xTruthParticle.auxdecor<double>("phi_vis") = 0;
  xTruthParticle.auxdecor<double>("m_vis") = 0;

  if (!xTruthParticle.hasDecayVtx())
    return;

  const xAOD::TruthVertex* xDecayVertex = xTruthParticle.decayVtx();
  for (size_t iOutgoingParticle = 0; iOutgoingParticle < xDecayVertex->nOutgoingParticles(); ++iOutgoingParticle )
  {
    const xAOD::TruthParticle* xTruthDaughter = xDecayVertex->outgoingParticle(iOutgoingParticle);
    int iAbsPdgId = xTruthDaughter->absPdgId();
    int iPdgId = xTruthDaughter->pdgId();
    // if tau decays leptonically return 0 truth tracks
    if ( (iAbsPdgId == 11) or (iAbsPdgId == 13) or (iAbsPdgId == 15) )
    {
      ATH_MSG_VERBOSE("found leptonic decay particle with PdgId "<<iPdgId);
      return;
    }
    // ignore neutrinos (actually overhead for this function)
    // maybe remove this block
    if ( (iAbsPdgId == 12) or (iAbsPdgId == 14) or (iAbsPdgId == 16) )
    {
      ATH_MSG_VERBOSE("found neutrino decay particle with PdgId "<<iPdgId);
      continue;
    }

    if (m_iSampleType == PYTHIA)
      if (!isGoodDaughter(xTruthDaughter->status(),
                          xTruthDaughter->barcode(),
                          xDecayVertex->barcode()))
        continue;

    vTruthVisTLV += xTruthDaughter->p4();

    // only count charged decay particles
    if ( xTruthDaughter->isCharged() )
    {
      ATH_MSG_VERBOSE("found charged decay particle with PdgId "<<iPdgId);
      iNChargedDaughters++;
      // count charged pions
      if (iAbsPdgId==211) iNChargedPions++;
      else iNChargedOthers++;
    }
    else
    {
      // count neutral pions
      if (iAbsPdgId==111) iNNeutralPions++;
      else iNNeutralOthers++;
    }
  }

  ATH_MSG_VERBOSE(iNChargedDaughters << " prong truth tau was found with barcode "<<xTruthParticle.barcode());
  if (iNChargedDaughters%2 == 0)
    ATH_MSG_WARNING("found tau with even multiplicity: " << iNChargedDaughters);
  else
    xTruthParticle.auxdecor<bool>("IsHadronicTau") = true;

  xTruthParticle.auxdecor<double>("pt_vis") = vTruthVisTLV.Pt();
  xTruthParticle.auxdecor<double>("eta_vis") = vTruthVisTLV.Eta();
  xTruthParticle.auxdecor<double>("phi_vis") = vTruthVisTLV.Phi();
  xTruthParticle.auxdecor<double>("m_vis") = vTruthVisTLV.M();

  std::string sDecayMode = "";
  sDecayMode+= std::to_string(iNChargedPions)+"p";
  if (iNChargedOthers!=0)
    sDecayMode+= std::to_string(iNChargedOthers)+"co";
  sDecayMode+= std::to_string(iNNeutralPions)+"n";
  if (iNNeutralOthers!=0)
    sDecayMode+= std::to_string(iNNeutralOthers)+"no";

  xTruthParticle.auxdecor<size_t>("NTracks") = iNChargedDaughters;
  xTruthParticle.auxdecor<std::string>("DecayMode") = sDecayMode;
}

//______________________________________________________________________________
bool TauTruthMatchingTool::isGoodDaughter(int iStatus,
    int iBarcode,
    int iVtxBarcode) const
{
  if ( iStatus == 2 ) return true;
  if ( iStatus < 200 && iStatus > 190 ) return true;
  if ( (iStatus%1000 == 1 || (iStatus%1000 == 2 && iStatus > 1000) || (iStatus == 2 && iVtxBarcode < -200000)) && iBarcode < 10000 ) return true;
  ATH_MSG_VERBOSE("decay particle seems to be a bad daughter, Status " << iStatus << ", Barcode " << iBarcode <<  ", Vertex Barcode " << iVtxBarcode);
  return false;
}
