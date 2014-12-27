/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EMAmbiguityTool.cxx
PACKAGE:  offline/Reconstruction/egamma/egammaTools

********************************************************************/

#include "EMAmbiguityTool.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "egammaRecEvent/egammaRec.h"

EMAmbiguityTool::EMAmbiguityTool(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent)
{
  declareInterface<IEMAmbiguityTool>(this);
  //Minimum silicon hits for electron

  declareProperty("minNoSiHits",  m_MinNoSiHits = 4, "Minimum number of silicon hits to be an electron==>not photon for sure");
  declareProperty("minNoPixHits", m_MinNoPixHits = 2, "Minimum number of Pixel hits to be an electron==>not photon for sure");
  declareProperty("maxEoverPCut", m_maxEoverPCut = 10,"Maximum EoverP , more that this is ambiguous");
  declareProperty("minPCut",      m_minPtCut = 2000 ,  "Minimum Pt, less than that is ambiguous");

}

EMAmbiguityTool::~EMAmbiguityTool() {}

StatusCode EMAmbiguityTool::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");
  
  return StatusCode::SUCCESS;
}

StatusCode EMAmbiguityTool::finalize() {
  return StatusCode::SUCCESS;
}

// ====================================================================
// return value: AuthorElectron, AuthorPhoton, AuthorAmbiguous, AuthorUnknown

unsigned int EMAmbiguityTool::ambiguityResolve(const egammaRec *egRec) const
{

  const xAOD::CaloCluster* cluster = egRec->caloCluster();
  const xAOD::Vertex* vx =  egRec->vertex();
  const xAOD::TrackParticle* tp = egRec->trackParticle();


  uint8_t trkExpectBlayerHit(0), trkBlayerHits(0), trkPixelHits(0), trkSiHits(0);

  double ep(0);
  if(tp){
    ep= cluster->e() * fabs(tp->qOverP());
  }

  if (tp && !tp->summaryValue(trkExpectBlayerHit,xAOD::expectInnermostPixelLayerHit))
    ATH_MSG_WARNING("Could not retrieve expected InnermostPixelLayer hit from track");
  
  if (tp && !tp->summaryValue(trkBlayerHits,xAOD::numberOfInnermostPixelLayerHits))
    ATH_MSG_WARNING("Could not retrieve number of InnermostPixelLayer hits from track");
  
  if (tp && !tp->summaryValue(trkPixelHits,xAOD::numberOfPixelHits))
    ATH_MSG_WARNING("Could not retrieve number of pixel hits from track");
  
  if (tp && !tp->summaryValue(trkSiHits, xAOD::numberOfSCTHits))
    ATH_MSG_WARNING("Could not retrieve number of SCT hits from track");
  trkSiHits += trkPixelHits;
  
  xAOD::EgammaParameters::ConversionType convType(xAOD::EgammaHelpers::conversionType(vx));
  bool vxDoubleSi = (convType == xAOD::EgammaParameters::doubleSi);
  

  //See if the Si+Si conversion shares one track with the electron
  //if not we might have a trident.
  //if yes and the track is not good we definetely matched the conversion as electron.
  bool shareTrack=false;
  if(tp && trkSiHits>=m_MinNoSiHits && vxDoubleSi){
    const xAOD::TrackParticle *trk1 = ( vx->nTrackParticles() ? vx->trackParticle(0) : 0 );
    const xAOD::TrackParticle *trk2 = ( vx->nTrackParticles() > 1 ? vx->trackParticle(1) : 0 );
  
    if( (tp==trk1) || (tp==trk2)) {
      shareTrack=true;

    }
  }
  
  ATH_MSG_DEBUG("Vertex, SiSi: " << (vx != 0) << " , " << vxDoubleSi);
  ATH_MSG_DEBUG("Track, Si hits, pixel hits, bLayerHits, expectBlayerHit: " 
    << (tp != 0) << " , " << (int) trkSiHits << " , " << (int) trkPixelHits 
    << " , " <<  (int) trkBlayerHits << " , " << (int) trkExpectBlayerHit);
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
    return xAOD::EgammaParameters::AuthorPhoton;
  }
  
  //Ambigous due to E/P ,Min Pt, pixel hits
  //We are not sure it is a Photon , but is not good enough either to be surely an Electron
  // - E/P >10  or track Pt < 2.0 GeV or no-pixel then Ambiguous
  if( ep > m_maxEoverPCut ||  tp->pt()<m_minPtCut || !trkPixelHits) {
    ATH_MSG_DEBUG("Returning Ambiguous due to E over P ||  min Pt  || no pixels");
    return xAOD::EgammaParameters::AuthorAmbiguous;
  }

  //Electron ==> Surely not Photon
  // - Track with at least the minimum Si and Pixel hits (previous selection for photons/ambiguous)
  // - And has E/P < 10 and Pt > 2.0 GeV (previous for ambiguous)
  // - No vertex Matched
  // - Or if a vertex exists and is not  Si+Si
  //    - Either Blayer hit
  //    - Or (2) or more Pixel hits when not expecting a b-layer hit
  // In this case we do not want this to be in Photons
  
  if ( !vx|| 
      (trkBlayerHits && !vxDoubleSi) || 
      (!trkExpectBlayerHit && trkPixelHits>=m_MinNoPixHits  && !vxDoubleSi) ){
      ATH_MSG_DEBUG("Returning Electron");
      return xAOD::EgammaParameters::AuthorElectron;
  }

  // Ambiguous all else, these will go to both electrons and photons
  // A matched Si+Si vertex and  a track with 2 pixel / blayer hits
  // A non Si+Si vertex matched and a track with no 2 pixel / blayer hits


  ATH_MSG_DEBUG("Returning Ambiguous");
  return xAOD::EgammaParameters::AuthorAmbiguous;
}

