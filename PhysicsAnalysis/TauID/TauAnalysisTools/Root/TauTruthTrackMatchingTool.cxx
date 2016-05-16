/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TauAnalysisTools/TauTruthTrackMatchingTool.h>

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
  ATH_MSG_INFO( "Initializing TauTruthTrackMatchingTool" );
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

  checkTrackIsTauInheritant(xTrackParticle);
  checkTrackSpuriousType(xTrackParticle);
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::classifyTracks(std::vector<const TAUTRACKPARTICLE*>& vTrackParticles)
{
  for (auto xTrackParticle : vTrackParticles)
  {
    checkTrackIsTauInheritant(*xTrackParticle);
    checkTrackSpuriousType(*xTrackParticle);
  }
  return StatusCode::SUCCESS;
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
void TauTruthTrackMatchingTool::checkTrackSpuriousType(const TAUTRACKPARTICLE& xTrackParticle)
{
  const xAOD::TruthParticle* xTruthParticle = getTruthParticle(xTrackParticle);

  static SG::AuxElement::Decorator<int> decSpuriousType("SpuriousType");
  // if not truth track is available -> unclassified
  if (!xTruthParticle)
  {
    decSpuriousType(xTrackParticle) = TauAnalysisTools::UnclassifiedTrack;
    return;
  }

  int iBarcode = xTruthParticle->barcode();
  static SG::AuxElement::ConstAccessor<float> accTruthMatchProbability("truthMatchProbability");
  if (accTruthMatchProbability(xTrackParticle) >= 0.5)
  {
    if (iBarcode >= 10000 && iBarcode < 200000) decSpuriousType(xTrackParticle) = TauAnalysisTools::PileupTrack;
    else if (iBarcode >= 200000)                decSpuriousType(xTrackParticle) = TauAnalysisTools::ConversionTrack;
    else if (iBarcode > 0 && iBarcode < 10000)  decSpuriousType(xTrackParticle) = TauAnalysisTools::UnderlyingEventTrack;
    else if (iBarcode == 0)                     decSpuriousType(xTrackParticle) = TauAnalysisTools::FakeTrack;
  }
  else
  {
    if (iBarcode < 200000)                      decSpuriousType(xTrackParticle) = TauAnalysisTools::FakeTrack;
    else                                        decSpuriousType(xTrackParticle) = TauAnalysisTools::ConversionTrack;
  }
}


//______________________________________________________________________________
const xAOD::TruthParticle* TauTruthTrackMatchingTool::getTruthParticle(const TAUTRACKPARTICLE& xTrackParticle)
{
  static SG::AuxElement::ConstAccessor< ElementLink<xAOD::TruthParticleContainer> > accTruthParticleLink("truthParticleLink");
  auto xTruthParticleContainer = accTruthParticleLink(xTrackParticle);
  //check validity of truth particle element link
  if (xTruthParticleContainer.isValid())
    return (*xTruthParticleContainer);
  return NULL;
}

//______________________________________________________________________________
void TauTruthTrackMatchingTool::checkTrackIsTauInheritant(const TAUTRACKPARTICLE& xTrackParticle)
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
