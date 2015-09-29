/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TauAnalysisTools/TauTruthTrackMatchingTool.h>

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauTruthTrackMatchingTool::TauTruthTrackMatchingTool( const std::string& name )
  : AsgTool(name)
{
}

//______________________________________________________________________________
TauTruthTrackMatchingTool::~TauTruthTrackMatchingTool( )
{
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::initialize()
{
  ATH_MSG_INFO( "Initialising TauTruthTrackMatchingTool" );
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::classifyTrack(const xAOD::TrackParticle& xTrackParticle)
{
  checkTrackIsTauInheritant(xTrackParticle);
  checkTrackSpuriousType(xTrackParticle);
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::classifyTracks(std::vector<const xAOD::TrackParticle*>& vTrackParticles)
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
void TauTruthTrackMatchingTool::checkTrackSpuriousType(const xAOD::TrackParticle& xTrackParticle)
{
  const xAOD::TruthParticle* xTruthParticle = getTruthParticle(xTrackParticle);

  // if not truth track is available -> unclassified
  if (!xTruthParticle)
  {
    xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::UnclassifiedTrack;
    return;
  }

  int iBarcode = xTruthParticle->barcode();
  if (xTrackParticle.auxdata<float>("truthMatchProbability") >= 0.5)
  {
    if (iBarcode >= 10000 && iBarcode < 200000) xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::PileupTrack;
    else if (iBarcode >= 200000)                xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::ConversionTrack;
    else if (iBarcode > 0 && iBarcode < 10000)  xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::UnderlyingEventTrack;
    else if (iBarcode == 0)                     xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::FakeTrack;
  }
  else
  {
    if (iBarcode < 200000)                      xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::FakeTrack;
    else                                        xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::ConversionTrack;
  }
}


//______________________________________________________________________________
const xAOD::TruthParticle* TauTruthTrackMatchingTool::getTruthParticle(const xAOD::TrackParticle& xTrackParticle)
{
  auto xTruthParticleContainer = xTrackParticle.auxdata<ElementLink<xAOD::TruthParticleContainer>>("truthParticleLink");
  //check validity of truth particle element link
  if (xTruthParticleContainer.isValid())
    return (*xTruthParticleContainer);
  return NULL;
}

//______________________________________________________________________________
void TauTruthTrackMatchingTool::checkTrackIsTauInheritant(const xAOD::TrackParticle& xTrackParticle)
{
  xTrackParticle.auxdecor<char>("IsHadronicTrack") = (char)false;
  int iDepth = -1;
  const xAOD::TruthParticle* xTruthParticle = getTruthParticle(xTrackParticle);
  std::string sHistory = "";
  if (xTruthParticle)
  {
    sHistory = std::to_string(xTruthParticle->pdgId());
    if (checkTruthParent(*xTruthParticle, iDepth, sHistory))
    {
      xTrackParticle.auxdecor<char>("IsHadronicTrack") = (char)true;
    }
  }
  xTrackParticle.auxdecor<std::string>("DecayHistory") = sHistory;
  xTrackParticle.auxdecor<int>("IsHadronicTrackDecayDepth") = iDepth;
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
