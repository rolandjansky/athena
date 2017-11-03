/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/**
   @class EGammaAmbiguityTool
   @brief Electron / photon ambiguity resolution. Dual-use version

   @author Bruno Lenzi
   @date   May 2015
*/

#include "ElectronPhotonSelectorTools/EGammaAmbiguityTool.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "FourMomUtils/xAODP4Helpers.h"

#define CHECK_HITS( EXP )				\
  if (!EXP)						\
    {							\
      ATH_MSG_WARNING("Failed \"" << #EXP << "\"" );	\
      return false;					\
    }							\


//=============================================================================
// Standard constructor
//=============================================================================
EGammaAmbiguityTool::EGammaAmbiguityTool(std::string myname) :
  asg::AsgTool(myname)
{
  declareProperty("minNoSiHits",  m_MinNoSiHits = 4, "Minimum number of silicon hits to be an electron==>not photon for sure");
  declareProperty("minNoPixHits", m_MinNoPixHits = 2, "Minimum number of Pixel hits to be an electron==>not photon for sure");
  declareProperty("maxEoverPCut", m_maxEoverPCut = 10,"Maximum EoverP , more that this is ambiguous");
  declareProperty("minPCut",      m_minPtCut = 2000 ,  "Minimum Pt, less than that is ambiguous");
  declareProperty("maxDeltaR_innermost",      m_maxDeltaR_innermost = 40 ,  "Maximum value for Rconv - RfirstHit for Si+Si conversions where both tracks have innermost hits");

}


//=============================================================================
// Standard destructor
//=============================================================================
EGammaAmbiguityTool::~EGammaAmbiguityTool(){
}

StatusCode EGammaAmbiguityTool::initialize(){
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  return StatusCode::SUCCESS;
}


//=============================================================================
// finalize method (now called by destructor)
//=============================================================================
StatusCode EGammaAmbiguityTool::finalize(){
  return StatusCode::SUCCESS;
}

// ====================================================================
// return value: AuthorElectron, AuthorPhoton, AuthorAmbiguous, AuthorUnknown

unsigned int EGammaAmbiguityTool::ambiguityResolve(const xAOD::CaloCluster* cluster,
                                                   const xAOD::Vertex* vx,
                                                   const xAOD::TrackParticle* tp,
						   xAOD::AmbiguityTool::AmbiguityType& type) const{

  type=xAOD::AmbiguityTool::unknown;
  // Energy from 3x5 cluster, stored in altE after EMClusterTool
  // Result should stay the same before after calibration corrections
  double cluster_e = (cluster->clusterSize() != xAOD::CaloCluster::SW_35ele && 
		      cluster->clusterSize() != xAOD::CaloCluster::SW_35gam &&
		      cluster->clusterSize() != xAOD::CaloCluster::SuperCluster) ?  cluster->altE() : cluster->e();

  
  //Number of hits from the track
  uint8_t trkPixelHits(0), trkSiHits(0);    
  if (tp && !tp->summaryValue(trkPixelHits,xAOD::numberOfPixelHits)){
    ATH_MSG_WARNING("Could not retrieve number of pixel hits from track");
  }
  if (tp && !tp->summaryValue(trkSiHits, xAOD::numberOfSCTHits)){
    ATH_MSG_WARNING("Could not retrieve number of SCT hits from track");
  }
  trkSiHits += trkPixelHits;
  
  //Check if the vertex is double silicon
  xAOD::EgammaParameters::ConversionType convType(xAOD::EgammaHelpers::conversionType(vx));
  bool vxDoubleSi = (convType == xAOD::EgammaParameters::doubleSi);
  
  // Check if the track particle and the ones associated to the conversion vertex
  // have innermost pixel hits
  bool trkHasInnermostHit = tp && hasInnermostPixelHit(*tp);
  size_t nTrkVxWithInnermostHit = vxDoubleSi ? nTrkWithInnermostPixelHits(*vx) : 0;

  //See if the Si+Si conversion shares one track with the electron
  //if not we might have a trident.
  //if yes and the track is not good we definetely matched the conversion as electron.
  bool shareTrack = tp && trkSiHits>=m_MinNoSiHits && vxDoubleSi && isInVertex(*tp, *vx);
  
  //Debug messages
  ATH_MSG_DEBUG("Vertex, SiSi, tracks with innermost pixel hits: " 
		<< (vx != 0) << ", " << vxDoubleSi << ",  " << nTrkVxWithInnermostHit);
  ATH_MSG_DEBUG("Track, Si hits, pixel hits, has innermost pixel hit: " 
		<< (tp != 0) << ", " << (int) trkSiHits << ", " << (int) trkPixelHits 
		<< " , " <<  (int) trkHasInnermostHit);
  ATH_MSG_DEBUG("Share track : " << shareTrack);

  //The Electron collection will NOT contain Photons
  //The Photon collection will NOT contain Electrons

  //So for conversions vs electrons better to go to ambiguous 
  //except  if we are sure.

  //Practically a photon here means definetely not Electron
  //And vice-versa

  //Photon ==> Surely not electron
  // - no track
  // - or no track with the minimum requirent hits to be an electron
  // - or Si+Si vertex and
  //    - No pixel
  //    - The electron track is part of the conversion
  // In this case we do not want this to be in Electrons

  if (!tp || 
      trkSiHits<m_MinNoSiHits || 
      (vxDoubleSi && !trkPixelHits && shareTrack)){
    ATH_MSG_DEBUG("Returning Photon");
    type= xAOD::AmbiguityTool::photon;
    return xAOD::EgammaParameters::AuthorPhoton;
  }
  
  //Ambigous due to E/P, Min Pt, pixel hits
  //We are not sure it is a Photon, but is not good enough either to be surely an Electron
  // - E/P >10  or track Pt < 2.0 GeV or no-pixel then Ambiguous
  double track_ep= cluster_e * fabs(tp->qOverP());

  if(tp->pt()<m_minPtCut) {
    ATH_MSG_DEBUG("Returning Ambiguous due  min Pt");
    type=xAOD::AmbiguityTool::ambiguousTrackLowPt;
    return xAOD::EgammaParameters::AuthorAmbiguous;
  }

  if( track_ep > m_maxEoverPCut) {
    ATH_MSG_DEBUG("Returning Ambiguous due to E over P");
    type=xAOD::AmbiguityTool::ambiguousTrackLowEoverP;
    return xAOD::EgammaParameters::AuthorAmbiguous;
  }


  if(!trkPixelHits) {
    ATH_MSG_DEBUG("Returning Ambiguous due to  no pixels");
    type=xAOD::AmbiguityTool::ambiguousTrackNoPixel;
    return xAOD::EgammaParameters::AuthorAmbiguous;
  }

  //Electron ==> Surely not Photon
  // - Track with at least the minimum Si and Pixel hits (previous selection for photons/ambiguous)
  // - And has E/P < 10 and Pt > 2.0 GeV (previous for ambiguous)
  // - No vertex Matched
  // - Or if a vertex exists and:
  //   * is not Si+Si 
  //   * is Si+Si but only 1 trk has "innermost pixel hits"
  //   * is Si+Si and both tracks have "innermost pixel hits" but  
  //     Rconv - RfirstHit > maxDeltaR_innermost 
  // In this case we do not want this to be in Photons
  
  if ( !vx|| 
       (trkHasInnermostHit && (!vxDoubleSi || nTrkVxWithInnermostHit == 1 || !passDeltaR_innermost(*vx))) ){
    ATH_MSG_DEBUG("Returning Electron");
    type=xAOD::AmbiguityTool::electron;
    return xAOD::EgammaParameters::AuthorElectron;
  }

  // Ambiguous all else, these will go to both electrons and photons
  // A matched Si+Si vertex (where either both or none of the tracks have 
  // "innermost pixel hits") 
  // and a track with "innermost pixel hits"
  // A non Si+Si vertex matched and a track with no "innermost pixel hits"

  //Here the types can become interesting
  double track_p = fabs(1.0/tp->qOverP());
  double vertex_p=xAOD::EgammaHelpers::momentumAtVertex(*vx).mag() ;

  type = (fabs(track_p - cluster_e) < fabs(vertex_p- cluster_e)) ? 
    xAOD::AmbiguityTool::ambiguousTrackEoverPBetterThanVertexEoverP : 
    xAOD::AmbiguityTool::ambiguousVertexEoverPBetterThanTrackEoverP ;
  //end of types
  
  ATH_MSG_DEBUG("Returning Ambiguous");
  return xAOD::EgammaParameters::AuthorAmbiguous;
}

unsigned int EGammaAmbiguityTool::ambiguityResolve(const xAOD::CaloCluster* cluster,
                                                   const xAOD::Vertex* vx,
                                                   const xAOD::TrackParticle* tp) const{

  xAOD::AmbiguityTool::AmbiguityType type;
  return ambiguityResolve(cluster,
			  vx,
			  tp,
			  type);

}



/** Redo the ambiguity resolution of central electrons and photons and return
 * AuthorElectron, AuthorPhoton, AuthorAmbiguous, AuthorUnknown 
 * or the author of the object if no overlapping object is found **/

unsigned int EGammaAmbiguityTool::ambiguityResolve(const xAOD::Egamma& egamma) const{
  
    //Fwd and Topo seeded not handled 
  if (egamma.author() == xAOD::EgammaParameters::AuthorFwdElectron ||
      egamma.author() == xAOD::EgammaParameters::AuthorCaloTopo35){
    ATH_MSG_DEBUG("Author Fwd of Topo seeded.  Do not do overlap or ambiguity");
    return egamma.author();
  }
  
  //No overlap found so either photon or electron
  if (!egamma.ambiguousObject()){
    //!ELink.isAvailable(*egamma)) {
    ATH_MSG_DEBUG("No overlaping object found");
    return egamma.author();
  }
  
  //Overlap found. define the electron and the photon
  const xAOD::Electron *electron = 
    dynamic_cast<const xAOD::Electron*>(egamma.type() == xAOD::Type::Electron ? &egamma : egamma.ambiguousObject());
  const xAOD::Photon *photon = 
    dynamic_cast<const xAOD::Photon*>(egamma.type() == xAOD::Type::Photon ? &egamma : egamma.ambiguousObject());
  
  //Error if cannot define any of them
  if (!electron || !photon){
    ATH_MSG_WARNING("Could not cast to electron and/or photon");
    return egamma.author();
  }
  
  //pass the proper input to the ambiguity resolve method
  xAOD::AmbiguityTool::AmbiguityType type;
  unsigned int result = ambiguityResolve(electron->caloCluster(),
					 photon->vertex(), 
					 electron->trackParticle(),
					 type);
  ATH_MSG_DEBUG("Performed ambiguity resolution, resulting type is: "<< type);
  return result;
}

/** Accept or reject egamma object based on ambiguity resolution */
bool EGammaAmbiguityTool::accept( const xAOD::Egamma& egamma, bool acceptAmbiguous ) const{
  unsigned int author = ambiguityResolve(egamma);

  if (author == xAOD::EgammaParameters::AuthorFwdElectron ||
      author == xAOD::EgammaParameters::AuthorCaloTopo35){
    return true;
  }

  if (acceptAmbiguous && author == xAOD::EgammaParameters::AuthorAmbiguous){
    return true;
  }

 return (author == (egamma.type() == xAOD::Type::Electron ? 
		     xAOD::EgammaParameters::AuthorElectron : xAOD::EgammaParameters::AuthorPhoton) );
}

/** Return true if track has innermost pixel hit 
 * or next-to-innermost in case innermost is not expected
 * or at least m_MinNoPixHits pixel hits in case next-to-innermost is not expected
 **/
bool EGammaAmbiguityTool::hasInnermostPixelHit(const xAOD::TrackParticle& tp) const
{
  uint8_t trkExpectHit(0), trkNhits(0);
  CHECK_HITS( tp.summaryValue(trkNhits,xAOD::numberOfInnermostPixelLayerHits) );
  if (trkNhits) {return true;}
  
  CHECK_HITS( tp.summaryValue(trkExpectHit,xAOD::expectInnermostPixelLayerHit) );
  if (trkExpectHit) {return false;}
  
  CHECK_HITS( tp.summaryValue(trkNhits,xAOD::numberOfNextToInnermostPixelLayerHits) );
  if (trkNhits) {return true;}
  
  CHECK_HITS( tp.summaryValue(trkExpectHit,xAOD::expectNextToInnermostPixelLayerHit) );
  if (trkExpectHit) {return false;}  
  
  CHECK_HITS( tp.summaryValue(trkNhits,xAOD::numberOfPixelHits) );
  return (trkNhits >= m_MinNoPixHits);
}

/** Return the number of tracks with "innermost pixel hits" (see above) 
 * in the given vertex **/
size_t EGammaAmbiguityTool::nTrkWithInnermostPixelHits(const xAOD::Vertex& vx) const{
  size_t n = 0;
  for (size_t i = 0; i < vx.nTrackParticles(); ++i){
    const xAOD::TrackParticle *tp = vx.trackParticle(i);
    if (!tp) {ATH_MSG_WARNING("Null pointer to track particle in vertex");}
    else if (hasInnermostPixelHit(*tp)) {++n;}
  }
  return n;
}

/** Return true if the given TrackParticle is part of the vertex **/
bool EGammaAmbiguityTool::isInVertex(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx) const{
  for (size_t i = 0; i < vx.nTrackParticles(); ++i){
    const xAOD::TrackParticle *tp = vx.trackParticle(i);
    if (!tp) {ATH_MSG_WARNING("Null pointer to track particle in vertex");}
    if (tp == &trk) {return true;}
  }
  return false;
}

/** Return true if the vertex passes the requirement on Rconv - RfirstHit **/
bool EGammaAmbiguityTool::passDeltaR_innermost(const xAOD::Vertex& vx) const
{
  if (not vx.isAvailable<float>("minRfirstHit")){
    ATH_MSG_WARNING("minRfirstHit not available");
    return false;
  }
  return xAOD::EgammaHelpers::conversionRadius(&vx) - vx.auxdata<float>("minRfirstHit") < m_maxDeltaR_innermost;
}
