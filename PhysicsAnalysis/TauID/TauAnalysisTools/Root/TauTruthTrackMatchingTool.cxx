/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <TauAnalysisTools/TauTruthTrackMatchingTool.h>

// EDM include(s)
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"

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
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::classifyTrack(const xAOD::TauTrack& xTrackParticle) const
{
  // don't classify tracks if this was already done
  if (!m_bIsHadronicTrackAvailable.isValid())
  {
    bool avail = xTrackParticle.isAvailable<char>("IsHadronicTrack");
    m_bIsHadronicTrackAvailable.set (avail);
    if (avail)
    {
      ATH_MSG_DEBUG("IsHadronicTrack decoration is available on first track processed, switched of rerun for further taus.");
      ATH_MSG_DEBUG("If a truth track matching needs to be redone, please pass a shallow copy of the original track.");
    }
  }
  if (*m_bIsHadronicTrackAvailable.ptr())
    return StatusCode::SUCCESS;

  ATH_CHECK(checkTrackIsTauInheritant(xTrackParticle));
  ATH_CHECK(checkTrackType(xTrackParticle));
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::classifyTracks(std::vector<const xAOD::TauTrack*>& vTrackParticles) const
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
StatusCode TauTruthTrackMatchingTool::checkTrackType(const xAOD::TauTrack& xTrackParticle) const
{
  const xAOD::TruthParticle* xTruthParticle = getTruthParticle(xTrackParticle);

  static const SG::AuxElement::Decorator<int> decTruthType("TruthType");
  if (!xTruthParticle)
  {
    decTruthType(xTrackParticle) = TauAnalysisTools::UnclassifiedTrack;
    return StatusCode::SUCCESS;
  }

  static const SG::AuxElement::ConstAccessor<float> accTruthMatchProbability("truthMatchProbability");

  if (accTruthMatchProbability(*(xTrackParticle.track())) < 0.5)
  {
    decTruthType(xTrackParticle) = TauAnalysisTools::FakeTrack;
    return StatusCode::SUCCESS;
  }

  static const SG::AuxElement::ConstAccessor< char > accIsHadronicTrack("IsHadronicTrack");
  static const SG::AuxElement::ConstAccessor< int > accIsHadronicTrackDecayDepth("IsHadronicTrackDecayDepth");
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
StatusCode TauTruthTrackMatchingTool::classifyConversion(const xAOD::TauTrack& xTrackParticle, const xAOD::TruthParticle& xTruthParticle) const
{
  static const SG::AuxElement::Decorator<int> decTruthType("TruthType");
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
      ATH_MSG_WARNING("Truth parent of tau decay was not found in TruthParticles container. Please ensure that this container has the full tau decay information or produce the TruthTaus container in AtlasDerivation.\nInformation on how to do this can be found here:\nhttps://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015#Accessing_Tau_Truth_Information");
      decTruthType(xTrackParticle) = TauAnalysisTools::SecondaryTrack;
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
      ATH_MSG_WARNING("Truth daughter of tau decay was not found in TruthParticles container. Please ensure that this container has the full tau decay information or produce the TruthTaus container in AtlasDerivation.\nInformation on how to do this can be found here:\nhttps://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015#Accessing_Tau_Truth_Information");
      decTruthType(xTrackParticle) = TauAnalysisTools::SecondaryTrack;
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
const xAOD::TruthParticle* TauTruthTrackMatchingTool::getTruthParticle(const xAOD::TauTrack& xTrackParticle) const
{
  static const SG::AuxElement::ConstAccessor< ElementLink<xAOD::TruthParticleContainer> > accTruthParticleLink("truthParticleLink");
  auto xTruthParticleLink = accTruthParticleLink(*(xTrackParticle.track()));
  //check validity of truth particle element link
  if (xTruthParticleLink.isValid())
    return (*xTruthParticleLink);
  return nullptr;
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::checkTrackIsTauInheritant(const xAOD::TauTrack& xTrackParticle) const
{
  static const SG::AuxElement::Decorator< char > decIsHadronicTrack("IsHadronicTrack");
  static const SG::AuxElement::Decorator< int > decIsHadronicTrackDecayDepth("IsHadronicTrackDecayDepth");
  static const SG::AuxElement::Decorator< std::string > decDecayHistory("DecayHistory");
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
bool TauTruthTrackMatchingTool::checkTruthParent(const xAOD::TruthParticle& xTruthParticle, int& iDepth, std::string& sHistory) const
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
      if (xTruthParticleParent) {
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
      } else {
        ATH_MSG_WARNING("vertex has incoming particles but no valid parent particle");
      }
    }
  }
  return false;
}
