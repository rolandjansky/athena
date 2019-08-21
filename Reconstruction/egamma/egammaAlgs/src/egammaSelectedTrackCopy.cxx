/*
   Copyright (C) 2002-2019  CERN for the benefit of the ATLAS collaboration
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
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCore/ShallowCopy.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/EventContext.h"
//std includes
#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <vector>

egammaSelectedTrackCopy::egammaSelectedTrackCopy(const std::string& name, 
                                                 ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_AllClusters{},
  m_SelectedClusters{},
  m_AllTracks{},
  m_SelectedTracks{},
  m_AllSiTracks{},
  m_SelectedSiTracks{},
  m_AllTRTTracks{},
  m_SelectedTRTTracks{}
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

  ATH_CHECK( m_egammaCaloClusterSelector.retrieve() );

  return StatusCode::SUCCESS;
}  

StatusCode egammaSelectedTrackCopy::egammaSelectedTrackCopy::finalize(){ 

  ATH_MSG_INFO ("--- egamma Selected Track Copy Statistics ---");
  ATH_MSG_INFO ("--- All Clusters: " << m_AllClusters);
  ATH_MSG_INFO ("--- Selected Clusters: " << m_SelectedClusters);
  ATH_MSG_INFO ("--- All Tracks: " << m_AllTracks);
  ATH_MSG_INFO ("--- Selected Tracks: " << m_SelectedTracks);
  ATH_MSG_INFO ("--- All Si Tracks: " << m_AllSiTracks);
  ATH_MSG_INFO ("--- Selected Si Tracks: " << m_SelectedSiTracks);
  ATH_MSG_INFO ("--- All TRT Tracks: " << m_AllTRTTracks);
  ATH_MSG_INFO ("--- Selected TRT Tracks: " << m_SelectedTRTTracks);
  ATH_MSG_INFO ("----------------------------------------- ---");
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
  auto allClusters = m_AllClusters.buffer();
  auto selectedClusters = m_SelectedClusters.buffer();
  auto allTracks= m_AllTracks.buffer();
  auto selectedTracks = m_SelectedTracks.buffer()  ;
  auto allSiTracks = m_AllSiTracks.buffer();  
  auto selectedSiTracks = m_SelectedSiTracks.buffer();  
  auto allTRTTracks =  m_AllTRTTracks.buffer();
  auto selectedTRTTracks = m_SelectedTRTTracks.buffer(); 

  // // lets first check which clusters to seed on;
  std::vector<const xAOD::CaloCluster *> passingClusters;
  for(const xAOD::CaloCluster* cluster : *clusterTES ){
    ++allClusters;   
    if (m_egammaCaloClusterSelector->passSelection(cluster)) {
      passingClusters.push_back(cluster);
      ++selectedClusters;
    }
  }
 //Extrapolation Cache
  IEMExtrapolationTools::Cache cache{};
  for(const xAOD::TrackParticle* track : *trackTES){
    
    ATH_MSG_DEBUG ("Check Track with Eta "<< track->eta()<< " Phi " << track->phi()<<" Pt " <<track->pt());
    ++allTracks;
    bool isTRT=false;
    int nhits(0);
    uint8_t dummy(0); 
    if( track->summaryValue(dummy,xAOD::numberOfPixelHits) ){
      nhits += dummy;
    }
    if( track->summaryValue(dummy,xAOD::numberOfSCTHits) ){
      nhits+= dummy;
    }
    if(nhits<4){
      isTRT = true;
      ++allTRTTracks;
    } else{
      isTRT = false;
      ++allSiTracks;
    }
    for(const xAOD::CaloCluster* cluster : passingClusters ){
      /*
       check if it the track is selected due to this cluster.
       If not continue to next cluster
       */
      if(!Select(Gaudi::Hive::currentContext(), cluster,track,cache,isTRT)){
        ATH_MSG_DEBUG ("Track did not match cluster");
        continue;
      }
      ATH_MSG_DEBUG ("Track Matched");
      viewCopy->push_back(track); 
      ++selectedTracks;
      if(isTRT) {
        ++selectedTRTTracks;
      }
      else {
        ++selectedSiTracks;
      }
      /*
       * The particular track got selected 
       * due to a cluster (any one of them will do)
       * break here and move to the next track
       */
      break;
    }//Loop on clusters

  }//Loop on tracks

  ATH_MSG_DEBUG ("Selected Track container size: "  << viewCopy->size() );
  ATH_CHECK( outputTrkPartContainer.record(std::move(viewCopy)) );

  return StatusCode::SUCCESS;
}


bool egammaSelectedTrackCopy::Select(const EventContext& ctx,
                                     const xAOD::CaloCluster* cluster,
                                     const xAOD::TrackParticle* track,
                                     IEMExtrapolationTools::Cache& cache,
                                     bool trkTRT) const
{

  ATH_MSG_DEBUG("egammaSelectedTrackCopy::Select()" );
  if (cluster == 0 || track == 0) return false;
  const Trk::Perigee& candidatePerigee = track->perigeeParameters();

  //Get Perigee Parameters
  const double  trkPhi = candidatePerigee.parameters()[Trk::phi];
  const double  trkEta = candidatePerigee.eta();
  const double  r_first=candidatePerigee.position().perp();
  const double  z_first=candidatePerigee.position().z();

  //Get Cluster parameters
  const double clusterEta=cluster->etaBE(2);
  const bool isEndCap= !xAOD::EgammaHelpers::isBarrel(cluster);
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
  const double etaclus_corrected = CandidateMatchHelpers::CorrectedEta(clusterEta,z_first,isEndCap);
  const double phiRot = CandidateMatchHelpers::PhiROT(Et,trkEta, track->charge(),r_first ,isEndCap)  ;
  const double phiRotTrack = CandidateMatchHelpers::PhiROT(track->pt(),trkEta, track->charge(),r_first ,isEndCap)  ;
  //Calcualate deltaPhis 
  const double deltaPhiStd = P4Helpers::deltaPhi(cluster->phiBE(2), trkPhi);
  const double trkPhiCorr = P4Helpers::deltaPhi(trkPhi, phiRot);
  const double deltaPhi2 = P4Helpers::deltaPhi(cluster->phiBE(2), trkPhiCorr);
  const double trkPhiCorrTrack = P4Helpers::deltaPhi(trkPhi, phiRotTrack);
  const double deltaPhi2Track = P4Helpers::deltaPhi(cluster->phiBE(2), trkPhiCorrTrack);

  /* 
   * First we will see if it fails the quick match 
   * Then if it passeis check if the extrapolation 
   * from last measurement keeps it (no GSF at this stage)
   */

  //Broad phi check
  if ( (fabs(deltaPhi2) > 2*m_broadDeltaPhi) 
       && (fabs(deltaPhi2Track) > 2.*m_broadDeltaPhi) 
       && (fabs(deltaPhiStd) > 2*m_broadDeltaPhi)){
    ATH_MSG_DEBUG("FAILS broad window phi match (track phi, phirotCluster , phiRotTrack ,cluster phi): ( " 
                  << trkPhi << ", " << phiRot<< ", "<<phiRotTrack<< ", " << cluster->phiBE(2) << ")" );
    return false;
  }
  //if TRT we can stop here , we can not check much in eta really.
  if(trkTRT){
    return true;
  }

  //eta check
  if (fabs(cluster->etaBE(2) - trkEta) > 2*m_broadDeltaEta && 
      fabs( etaclus_corrected- trkEta) > 2.*m_broadDeltaEta){
    ATH_MSG_DEBUG("FAILS broad window eta match (track eta, cluster eta, cluster eta corrected): ( " 
                  << trkEta << ", " << cluster->etaBE(2) <<", "<<etaclus_corrected<<")" );
    return false;
  }

  //Extrapolate from last measurement, since this is before brem fit last measurement is better.
  std::vector<double>  eta(4, -999.0);
  std::vector<double>  phi(4, -999.0);
  std::vector<double>  deltaEta(4, -999.0);
  std::vector<double>  deltaPhi(4, -999.0);
  if (m_extrapolationTool->getMatchAtCalo (ctx,
                                           cluster, 
                                           track, 
                                           Trk::alongMomentum, 
                                           eta,
                                           phi,
                                           deltaEta, 
                                           deltaPhi, 
                                           IEMExtrapolationTools::fromLastMeasurement,
                                           &cache).isFailure()) {
    return false;
  }  

  // Selection in narrow eta/phi window from last measurement
  if(fabs(deltaEta[2]) < m_narrowDeltaEta && 
     deltaPhi[2] < m_narrowDeltaPhi && 
     deltaPhi[2] > -m_narrowDeltaPhiBrem) {
    ATH_MSG_DEBUG("Match from Last measurement is successful :  " << deltaPhi[2] );
    return true;
  }
  ATH_MSG_DEBUG("Matching Failed deltaPhi/deltaEta " << deltaPhi[2] <<" / "<< deltaEta[2]<<",isTRT, "<< trkTRT);
  return false;
}
