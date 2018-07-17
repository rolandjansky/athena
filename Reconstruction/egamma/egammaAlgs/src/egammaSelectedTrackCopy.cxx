/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

/*
NAME:     egammaSelectedTrackCopy
PACKAGE:  offline/Reconstruction/egamma/egammaAlgs/egammaSelectedTrackCopy
AUTHORS:  Anastopoulos
CREATED:  25/06/2018

PURPOSE: Select track to be refitted later on with GSF
UPDATE : 25/06/2018
*/

#include "egammaSelectedTrackCopy.h"
//
#include "egammaUtils/CandidateMatchHelpers.h"
#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCore/ShallowCopy.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

//std includes
#include <algorithm>
#include <cmath>
#include <memory>

egammaSelectedTrackCopy::egammaSelectedTrackCopy(const std::string& name, 
                                         ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
}

StatusCode egammaSelectedTrackCopy::initialize() {

  ATH_CHECK(m_clusterContainerKey.initialize());
  ATH_CHECK(m_trackParticleContainerKey.initialize());
  ATH_CHECK(m_OutputTrkPartContainerKey.initialize());

  /* the extrapolation tool*/
  if(m_extrapolationTool.retrieve().isFailure()){
    ATH_MSG_ERROR("initialize: Cannot retrieve extrapolationTool " << m_extrapolationTool);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}  

StatusCode egammaSelectedTrackCopy::egammaSelectedTrackCopy::finalize(){ 

  ATH_MSG_INFO ("AllTracks " << m_AllTracks);
  ATH_MSG_INFO ("AllSiTracks " << m_AllSiTracks);
  ATH_MSG_INFO ("AllTRTTracks " << m_AllTRTTracks);
  ATH_MSG_INFO ("SelectedTracks " << m_SelectedTracks);
  ATH_MSG_INFO ("SelectedSiTracks " << m_SelectedSiTracks);
  ATH_MSG_INFO ("SelectedTRTTracks " << m_SelectedTRTTracks);
  return StatusCode::SUCCESS;
}

StatusCode egammaSelectedTrackCopy::execute()
{
  SG::ReadHandle<xAOD::CaloClusterContainer> clusterTES(m_clusterContainerKey);
  if(!clusterTES.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve cluster container: "<< m_clusterContainerKey.key());
    return StatusCode::FAILURE;
  }
  SG::ReadHandle<xAOD::TrackParticleContainer> trackTES(m_trackParticleContainerKey);
  if(!trackTES.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve TrackParticle container: "<< m_trackParticleContainerKey.key());
    return StatusCode::FAILURE;
  }

  SG::WriteHandle<ConstDataVector<xAOD::TrackParticleContainer> > outputTrkPartContainer(m_OutputTrkPartContainerKey); 
  /*
   * Here it just needs to be a view copy , 
   * i.e the collection of selected trackParticles 
   * we create does not really own its elements
   */
  auto viewCopy =  std::make_unique<ConstDataVector <xAOD::TrackParticleContainer> >(SG::VIEW_ELEMENTS );

  ATH_MSG_DEBUG ("Cluster container size: "  << clusterTES->size() );
  ATH_MSG_DEBUG ("Track Particle  container  size: "  <<trackTES->size() );

  //Local counters
  unsigned int allTracks(0);
  unsigned int allTRTTracks(0);
  unsigned int allSiTracks(0);  
  unsigned int selectedTracks(0);
  unsigned int selectedTRTTracks(0); 
  unsigned int selectedSiTracks(0);  
 
  for(const xAOD::TrackParticle* track : *trackTES){
    ATH_MSG_DEBUG ("Check Track with Eta "<< track->eta()<< " Phi " << track->phi()<<" Pt " <<track->pt());
    bool isTRT=false;
    int nhits(0);
    uint8_t dummy(0); 
    if( track->summaryValue(dummy,xAOD::numberOfPixelHits) ){
      nhits += dummy;
    }
    if( track->summaryValue(dummy,xAOD::numberOfSCTHits) ){
      nhits+= dummy;
    }
    ++allTracks;    
    if(nhits<4){
      isTRT = true;
      ++allTRTTracks;
    } else{
      isTRT = false;
      ++allSiTracks;
    }
    for(const xAOD::CaloCluster* cluster : *clusterTES ){
      /*
        check if it the track is selected due to this cluster.
        If not continue to next cluster
      */
      if(!Select(cluster, isTRT,track )){
        ATH_MSG_DEBUG ("Track did not match cluster");
        continue;
      }
      //Track is selected/matched
      ATH_MSG_DEBUG ("Track Matched");
     //Push back the selected to the view container
      viewCopy->push_back(track); 
      ++selectedTracks;
      if(isTRT) {
        ++selectedTRTTracks;
      }
      else {
        ++selectedSiTracks;
      }
      /*
       * The particular track got  selected 
       * due to a cluster (any one of them will do)
       * break here and move to the next track
       */
      break;
    }//Loop on clusters
  }//Loop on tracks

  ATH_MSG_DEBUG ("Selected Track container size: "  << viewCopy->size() );
  ATH_CHECK( outputTrkPartContainer.record(std::move(viewCopy)) );

  /*
   * Typical use for relaxed memory ordering is incrementing counters, 
   * (such as the reference counters of std::shared_ptr) 
   * since this only requires atomicity, but not ordering or synchronization.
   */
  m_AllTracks.fetch_add(allTracks, std::memory_order_relaxed);
  m_AllTRTTracks.fetch_add(allTRTTracks, std::memory_order_relaxed);
  m_AllSiTracks.fetch_add(allSiTracks,std::memory_order_relaxed);
  m_SelectedTracks.fetch_add(selectedTracks,std::memory_order_relaxed);
  m_SelectedTRTTracks.fetch_add(selectedTRTTracks,std::memory_order_relaxed);
  m_SelectedSiTracks.fetch_add(selectedSiTracks,std::memory_order_relaxed);

  return StatusCode::SUCCESS;
}



bool egammaSelectedTrackCopy::Select(const xAOD::CaloCluster*   cluster,
                                 bool                       trkTRT,
                                 const xAOD::TrackParticle* track) const
{

  ATH_MSG_DEBUG("egammaSelectedTrackCopy::Select()" );
  if (cluster == 0 || track == 0) return false;
  const Trk::Perigee& candidatePerigee = track->perigeeParameters();

  //Get Perigee Parameters
  double  trkPhi = candidatePerigee.parameters()[Trk::phi];
  double  trkEta = candidatePerigee.eta();
  double  r_first=candidatePerigee.position().perp();
  double  z_first=candidatePerigee.position().z();

  //Get Cluster parameters
  double clusterEta=cluster->etaBE(2);
  bool isEndCap= cluster->inEndcap();
  double Et= cluster->e()/cosh(trkEta);
  if(trkTRT){
    Et = cluster->et();
  }

  // a few sanity checks
  if (fabs(clusterEta) > 10.0 || fabs(trkEta) > 10.0 || Et <= 0.0) {
    ATH_MSG_DEBUG("FAILS sanity checks :  Track Eta : " << trkEta 
                  << ", Cluster Eta " << clusterEta );
    return false;
  }

  //Calculate corrrected eta and Phi
  double etaclus_corrected = CandidateMatchHelpers::CorrectedEta(clusterEta,z_first,isEndCap);
  double phiRot = CandidateMatchHelpers::PhiROT(Et,trkEta, track->charge(),r_first ,isEndCap)  ;
  double phiRotTrack = CandidateMatchHelpers::PhiROT(track->pt(),trkEta, track->charge(),r_first ,isEndCap)  ;

  //Calcualate deltaPhis 
  double deltaPhiStd = P4Helpers::deltaPhi(cluster->phiBE(2), trkPhi);
  double trkPhiCorr = P4Helpers::deltaPhi(trkPhi, phiRot);
  double deltaPhi2 = P4Helpers::deltaPhi(cluster->phiBE(2), trkPhiCorr);
  double trkPhiCorrTrack = P4Helpers::deltaPhi(trkPhi, phiRotTrack);
  double deltaPhi2Track = P4Helpers::deltaPhi(cluster->phiBE(2), trkPhiCorrTrack);

  if ((!trkTRT)&& fabs(cluster->etaBE(2) - trkEta) > 2*m_broadDeltaEta && 
      fabs( etaclus_corrected- trkEta) > 2.*m_broadDeltaEta){
    ATH_MSG_DEBUG("FAILS broad window eta match (track eta, cluster eta, cluster eta corrected): ( " 
                  << trkEta << ", " << cluster->etaBE(2) <<", "<<etaclus_corrected<<")" );
    return false;
  }
  //if it does not fail the eta cut, does it fail the phi?
  if ( (fabs(deltaPhi2) > 2*m_broadDeltaPhi) && (fabs(deltaPhi2Track) > 2.*m_broadDeltaPhi) 
       && (fabs(deltaPhiStd) > 2*m_broadDeltaPhi)){
    ATH_MSG_DEBUG("FAILS broad window phi match (track phi, phirotCluster , phiRotTrack ,cluster phi): ( " 
                  << trkPhi << ", " << phiRot<< ", "<<phiRotTrack<< ", " << cluster->phiBE(2) << ")" );
    return false;
  }

  //Extrapolate from last measurement, since this is before brem fit last measurement is better.
  IEMExtrapolationTools::TrkExtrapDef extrapFrom = IEMExtrapolationTools::fromLastMeasurement;
  std::vector<double>  eta(4, -999.0);
  std::vector<double>  phi(4, -999.0);
  std::vector<double>  deltaEta(4, -999.0);
  std::vector<double>  deltaPhi(4, -999.0);
  if (m_extrapolationTool->getMatchAtCalo (cluster, 
                                           track, 
                                           trkTRT,
                                           Trk::alongMomentum, 
                                           eta,
                                           phi,
                                           deltaEta, 
                                           deltaPhi, 
                                           extrapFrom).isFailure()) {return false;}  

  // Selection in narrow eta/phi window
  if(( trkTRT || fabs(deltaEta[2]) < m_narrowDeltaEta ) && 
     deltaPhi[2] < m_narrowDeltaPhi && 
     deltaPhi[2] > -m_narrowDeltaPhiBrem) {
    ATH_MSG_DEBUG("Standard Match success " << deltaPhi[2] );
    return true;
  }
  else if(!trkTRT && fabs(deltaEta[2]) < m_narrowDeltaEta ){ 
    ATH_MSG_DEBUG("Normal matched Failed deltaPhi/deltaEta " 
                  << deltaPhi[2] <<" / "<< deltaEta[2]<<", Trying Rescale" );
    //Extrapolate from Perigee Rescaled 
    IEMExtrapolationTools::TrkExtrapDef extrapFrom1 = IEMExtrapolationTools::fromPerigeeRescaled;
    std::vector<double>  eta1(4, -999.0);
    std::vector<double>  phi1(4, -999.0);
    std::vector<double>  deltaEta1(4, -999.0);
    std::vector<double>  deltaPhi1(5, -999.0); // Set size to 5 to store deltaPhiRot
    if (m_extrapolationTool->getMatchAtCalo (cluster, 
                                             track, 
                                             trkTRT,
                                             Trk::alongMomentum, 
                                             eta1,
                                             phi1,
                                             deltaEta1, 
                                             deltaPhi1, 
                                             extrapFrom1).isFailure()) return false;
    if( fabs(deltaEta1[2]) < m_narrowDeltaEta 
        && deltaPhi1[2] < m_narrowRescale
        && deltaPhi1[2] > -m_narrowRescaleBrem) {
      ATH_MSG_DEBUG("Rescale Match success " << deltaPhi1[2] );
      return true;
    }
    else {
      ATH_MSG_DEBUG("Rescaled matched Failed deltaPhi/deltaEta " 
                    << deltaPhi1[2] <<" / "<< deltaEta1[2] );
      return false;
    }
  } 
  ATH_MSG_DEBUG("Matched Failed deltaPhi/deltaEta " << deltaPhi[2] <<" / "<< deltaEta[2]<<",isTRT, "<< trkTRT);
  return false;
}
