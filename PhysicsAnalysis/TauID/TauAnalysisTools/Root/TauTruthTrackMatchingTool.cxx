#include <TauAnalysisTools/TauTruthTrackMatchingTool.h>

// EDM include(s)
#include "xAODTruth/TruthParticle.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauTruthTrackMatchingTool::TauTruthTrackMatchingTool( const std::string& name )
  : AsgTool(name)
  , m_bIsHadronicTrackAvailable(false)
  , m_bIsHadronicTrackAvailableChecked(false)
{
}

//______________________________________________________________________________
TauTruthTrackMatchingTool::~TauTruthTrackMatchingTool( )
{
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::initialize()
{
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::classifyTrack(const TAUTRACKPARTICLE& xTrackParticle)
{
  // don't classify tracks if this was already done
  if (!m_bIsHadronicTrackAvailableChecked)
  {
    m_bIsHadronicTrackAvailable = xTrackParticle.isAvailable<char>("IsHadronicTrack");
    m_bIsHadronicTrackAvailableChecked = true;
    if (m_bIsHadronicTrackAvailable)
    {
      ATH_MSG_DEBUG("IsHadronicTrack decoration is available on first track processed, switched of rerun for further taus.");
      ATH_MSG_DEBUG("If a truth track matching needs to be redone, please pass a shallow copy of the original track.");
    }
  }
  if (m_bIsHadronicTrackAvailable)
    return StatusCode::SUCCESS;

  ATH_CHECK(checkTrackIsTauInheritant(xTrackParticle));
  ATH_CHECK(checkTrackType(xTrackParticle));
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::classifyTracks(std::vector<const TAUTRACKPARTICLE*>& vTrackParticles)
{
  for (auto xTrackParticle : vTrackParticles)
  {
    ATH_CHECK(checkTrackIsTauInheritant(*xTrackParticle));
    ATH_CHECK(checkTrackType(*xTrackParticle));
  }
  return StatusCode::SUCCESS;
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::checkTrackType(const TAUTRACKPARTICLE& xTrackParticle)
{
  const xAOD::TruthParticle* xTruthParticle = getTruthParticle(xTrackParticle);

  static SG::AuxElement::Decorator<int> decTruthType("TruthType");
  if (!xTruthParticle)
  {
    decTruthType(xTrackParticle) = TauAnalysisTools::UnclassifiedTrack;
    return StatusCode::SUCCESS;
  }

  static SG::AuxElement::ConstAccessor<float> accTruthMatchProbability("truthMatchProbability");
#ifndef XAODTAU_VERSIONS_TAUJET_V3_H
  if (accTruthMatchProbability(xTrackParticle) < 0.5)
#else
  if (accTruthMatchProbability(*(xTrackParticle.track())) < 0.5)
#endif // not XAODTAU_VERSIONS_TAUJET_V3_H
  {
    decTruthType(xTrackParticle) = TauAnalysisTools::FakeTrack;
    return StatusCode::SUCCESS;
  }

  static SG::AuxElement::ConstAccessor< char > accIsHadronicTrack("IsHadronicTrack");
  static SG::AuxElement::ConstAccessor< int > accIsHadronicTrackDecayDepth("IsHadronicTrackDecayDepth");
  if ((bool)accIsHadronicTrack(xTrackParticle) and accIsHadronicTrackDecayDepth(xTrackParticle) == 0)
  {
    decTruthType(xTrackParticle) = TauAnalysisTools::TauTrack;
    return StatusCode::SUCCESS;
  }

  int iBarcode = xTruthParticle->barcode();
  if (iBarcode > 0 && iBarcode < 10000)            decTruthType(xTrackParticle) = TauAnalysisTools::UnderlyingEventTrack;
  else if (iBarcode >= 10000 && iBarcode < 200000) decTruthType(xTrackParticle) = TauAnalysisTools::PileupTrack;
  else if (iBarcode == 0)                          decTruthType(xTrackParticle) = TauAnalysisTools::FakeTrack;
  else if (iBarcode >= 200000)                     ATH_CHECK(classifyConversion(xTrackParticle, *xTruthParticle));
  else ATH_MSG_WARNING("No truth classification possible for barcode "<<iBarcode);

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::classifyConversion(const TAUTRACKPARTICLE& xTrackParticle, const xAOD::TruthParticle& xTruthParticle)
{
  static SG::AuxElement::Decorator<int> decTruthType("TruthType");
  if (!xTruthParticle.isElectron())
  {
    decTruthType(xTrackParticle) = TauAnalysisTools::SecondaryTrack;
    return StatusCode::SUCCESS;
  }
  const xAOD::TruthVertex* xProdVertex = xTruthParticle.prodVtx();
  if ( !xProdVertex )
  {
    decTruthType(xTrackParticle) = TauAnalysisTools::SecondaryTrack;
    return StatusCode::SUCCESS;
  }
  for ( size_t iIncomingParticle = 0; iIncomingParticle < xProdVertex->nIncomingParticles(); ++iIncomingParticle )
  {
    const xAOD::TruthParticle* xTruthParent = xProdVertex->incomingParticle(iIncomingParticle);
    if (!xTruthParent)
    {
      ATH_MSG_FATAL("Truth parent of tau decay was not found in TruthParticles container. Please ensure that this container has the full tau decay information or produce the TruthTaus container in AtlasDerivation.\nInformation on how to do this can be found here:\nhttps://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015#Accessing_Tau_Truth_Information");
      return StatusCode::SUCCESS;
    }

    if (!xTruthParent->isPhoton())
    {
      decTruthType(xTrackParticle) = TauAnalysisTools::SecondaryTrack;
      return StatusCode::SUCCESS;
    }
  }

  size_t iElectrons = 0;
  for ( size_t iOutgoingParticle = 0; iOutgoingParticle < xProdVertex->nOutgoingParticles(); ++iOutgoingParticle )
  {
    const xAOD::TruthParticle* xTruthDaughter = xProdVertex->outgoingParticle(iOutgoingParticle);
    if (!xTruthDaughter)
    {
      ATH_MSG_FATAL("Truth daughter of tau decay was not found in TruthParticles container. Please ensure that this container has the full tau decay information or produce the TruthTaus container in AtlasDerivation.\nInformation on how to do this can be found here:\nhttps://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015#Accessing_Tau_Truth_Information");
      return StatusCode::SUCCESS;
    }

    if (!xTruthDaughter->isElectron())
    {
      decTruthType(xTrackParticle) = TauAnalysisTools::SecondaryTrack;
      return StatusCode::SUCCESS;
    }
    iElectrons++;
  }
  if (iElectrons != 2)
  {
    decTruthType(xTrackParticle) = TauAnalysisTools::SecondaryTrack;
    return StatusCode::SUCCESS;
  }
  decTruthType(xTrackParticle) = TauAnalysisTools::ConversionTrack;
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
const xAOD::TruthParticle* TauTruthTrackMatchingTool::getTruthParticle(const TAUTRACKPARTICLE& xTrackParticle)
{
  static SG::AuxElement::ConstAccessor< ElementLink<xAOD::TruthParticleContainer> > accTruthParticleLink("truthParticleLink");
#ifdef XAODTAU_VERSIONS_TAUJET_V3_H
  auto xTruthParticleContainer = accTruthParticleLink(*(xTrackParticle.track()));
#else
  auto xTruthParticleContainer = accTruthParticleLink(xTrackParticle);
#endif // XAODTAU_VERSIONS_TAUJET_V3_H
  //check validity of truth particle element link
  if (xTruthParticleContainer.isValid())
    return (*xTruthParticleContainer);
  return NULL;
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::checkTrackIsTauInheritant(const TAUTRACKPARTICLE& xTrackParticle)
{
  static SG::AuxElement::Decorator< char > decIsHadronicTrack("IsHadronicTrack");
  static SG::AuxElement::Decorator< int > decIsHadronicTrackDecayDepth("IsHadronicTrackDecayDepth");
  static SG::AuxElement::Decorator< std::string > decDecayHistory("DecayHistory");
  decIsHadronicTrack(xTrackParticle) = (char)false;
  int iDepth = -1;
  const xAOD::TruthParticle* xTruthParticle = getTruthParticle(xTrackParticle);
  std::string sHistory = "";
  if (xTruthParticle)
  {
    sHistory = std::to_string(xTruthParticle->pdgId());
    if (checkTruthParent(*xTruthParticle, iDepth, sHistory))
    {
      decIsHadronicTrack(xTrackParticle) = (char)true;
    }
  }
  decIsHadronicTrackDecayDepth(xTrackParticle) = iDepth;
  decDecayHistory(xTrackParticle) = sHistory;

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
bool TauTruthTrackMatchingTool::checkTruthParent(const xAOD::TruthParticle& xTruthParticle, int& iDepth, std::string& sHistory)
{
  iDepth++;
  if (xTruthParticle.hasProdVtx())
  {
    const xAOD::TruthVertex* xVertex = xTruthParticle.prodVtx();
    if (xVertex->nIncomingParticles() != 1)
      ATH_MSG_VERBOSE("not exactly one incomming particles for vertex, number of incomming particles: "<<xVertex->nIncomingParticles());
    if (xVertex->nIncomingParticles() > 0)
    {
      const xAOD::TruthParticle* xTruthParticleParent = xVertex->incomingParticle(0);
      // store parent pdgID in history
      sHistory.insert(0, std::to_string(xTruthParticleParent->pdgId())+":");//xTruthParticleParent->pdgId());
      if (xTruthParticleParent->absPdgId() == 15)
      {
        return true;
      }
      else
      {
        return checkTruthParent(*xTruthParticleParent, iDepth, sHistory);
      }
    }
  }
  return false;
}
