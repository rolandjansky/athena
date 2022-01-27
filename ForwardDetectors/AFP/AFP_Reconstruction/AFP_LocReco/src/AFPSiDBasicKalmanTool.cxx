/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @file   AFPSiDBasicKalmanTool.cxx
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-03-01
/// 
/// @brief  Implementation file for AFPSiDBasicKalmanTool used in tracks reconstruction.

// STL includes
#include<list>
#include<sstream>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

// xAOD includes
#include "xAODForward/AFPSiHitsCluster.h"
#include "xAODForward/AFPSiHitsClusterContainer.h"
#include "xAODForward/AFPTrack.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrackAuxContainer.h"

// AFP_LocReco includes
#include "AFP_LocReco/AFPSiDBasicKalmanTool.h"

AFPSiDBasicKalmanTool::AFPSiDBasicKalmanTool( const std::string& type, 
                                              const std::string& name, 
                                              const IInterface* parent ) : 
  base_class( type, name, parent )
{
}

StatusCode AFPSiDBasicKalmanTool::initialize()
{
  CHECK( m_hitsClusterContainerKey.initialize() );
  
  // monitoring
  if (!(m_monTool.name().empty())) {
    CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG("m_monTool name: " << m_monTool);
  }
  
  // === Kalman matrices initialization ===

  // --- initialise observation model matrix ---
  m_observationModel = CLHEP::HepMatrix(2, 4, 0);
  // if proper m_observationModelInit is available use it
  if (checkMatrixAndVectorSize(m_observationModel, m_observationModelInit)) {
    initMatrixFromVector (m_observationModel, m_observationModelInit);
  }
  else {
    // otherwise use default unit transformation
    m_observationModel[0][0] = 1.;
    m_observationModel[1][2] = 1.;
  }
  ATH_MSG_DEBUG("Observation model matrix (Hk) is initialised to: "<<m_observationModel);

  // --- set observation noise matrix ---
  m_observationNoise = CLHEP::HepMatrix(2, 2, 0);
  if (checkMatrixAndVectorSize(m_observationNoise, m_observationNoiseInit)) { 
    // use matrix from job options if set
    initMatrixFromVector(m_observationNoise, m_observationNoiseInit);
  }
  else {
    // if matrix is not initialised from the job options use pixel sizes
    const double pixelSizeX = 0.05;
    const double pixelSizeY = 0.25;
    m_observationNoise[0][0] = pixelSizeX*pixelSizeX;
    m_observationNoise[1][1] = pixelSizeY*pixelSizeY;
  }
  ATH_MSG_DEBUG("Observation noise  matrix (Vk) is initialised to: "<<m_observationNoise);

  // --- set covariance matrix of the process noise
  m_processNoiseCov = CLHEP::HepMatrix(4, 4, 0); // initialise with default values
  if (checkMatrixAndVectorSize(m_processNoiseCov, m_processNoiseCovInit)) {
    // use matrix from job options if set
    initMatrixFromVector(m_processNoiseCov, m_processNoiseCovInit);
  }
  ATH_MSG_DEBUG("Process noise covariance matrix (Qk) is initialised to: "<<m_processNoiseCov);


  // --- set initial value of the a posteriori error covariance matrix ---
  m_aposterioriCov = CLHEP::HepMatrix(4, 4, 0);
  if (checkMatrixAndVectorSize(m_aposterioriCov, m_aposterioriCovInit)) {
    // use matrix from job options if set
    initMatrixFromVector(m_aposterioriCov, m_aposterioriCovInit);
  }
  else {
    // set default matrix if not defined in job options
    const float planesZDist = 9.;

    const float pixelSizeX = 0.05;
    m_aposterioriCov[0][0] = pixelSizeX * pixelSizeX / 3.;
    m_aposterioriCov[1][1] = pixelSizeX * pixelSizeX / (planesZDist*planesZDist*3.);

    const float pixelSizeY = 0.25;
    m_aposterioriCov[2][2] = pixelSizeY * pixelSizeY / 3.;
    m_aposterioriCov[3][3] = pixelSizeY * pixelSizeY / (planesZDist*planesZDist*3.);
  }
  ATH_MSG_DEBUG("A posteriori error covariance matrix (Pkk) is initialised to: "<<m_aposterioriCov);


  // print information about initialised layers and stations
  if (m_numberOfLayersInStation >= 2)
  {
    ATH_MSG_DEBUG("Station with ID="<<m_stationID <<" will have "<<m_numberOfLayersInStation<<" layers.");
  }
  else
  {
    ATH_MSG_ERROR("Impossible to reconstruct tracks with less than 2 layers in station, but configured to run with "<<m_numberOfLayersInStation<<".");
    return StatusCode::FAILURE;
  }
  
  // print information about remaining parameters
  ATH_MSG_DEBUG("Maximal distance at which cluster can be joined to the track m_maxAllowedDistance = "<<m_maxAllowedDistance);
  ATH_MSG_DEBUG("Minimal number of clusters in track. If there are less clusters track is rejected m_minClustersNumber = "<<m_minClustersNumber);
  ATH_MSG_DEBUG("Maximal chi2 of cluster to be added to track = "<<m_clusterMaxChi2);
  ATH_MSG_DEBUG("Maximal chi2 of the track = "<<m_trackMaxChi2);

  return StatusCode::SUCCESS;
}

StatusCode AFPSiDBasicKalmanTool::finalize()
{
  return StatusCode::SUCCESS;
}

void AFPSiDBasicKalmanTool::fillLayersWithClusters(AFPLocRecoStationBasicObj& my_stationClusters, SG::ReadHandle<xAOD::AFPSiHitsClusterContainer>& hitsClusterContainer) const
{
  // retrieve clusters
  try {
    // fill layers with clusters
    for (const xAOD::AFPSiHitsCluster* theCluster : *hitsClusterContainer)
      if (theCluster->stationID() == m_stationID) // check if cluster is from the correct station
        my_stationClusters.clustersInLayer(theCluster->pixelLayerID()).push_back(theCluster);
  }
  catch (const std::out_of_range& outOfRange) {
    ATH_MSG_WARNING("Cluster with station or pixel ID outside expected range. Aborting track reconstruction.");
    clearAllLayers(my_stationClusters);
  }

  return;
}

StatusCode AFPSiDBasicKalmanTool::reconstructTracks(std::unique_ptr<xAOD::AFPTrackContainer>& outputContainer, const EventContext& ctx) const
{
  SG::ReadHandle<xAOD::AFPSiHitsClusterContainer> hitsClusterContainer( m_hitsClusterContainerKey, ctx );
  if(!hitsClusterContainer.isValid())
  {
      // this is allowed, there might be no AFP data in the input
      return StatusCode::SUCCESS;
  }
  
  
  typedef std::vector< std::vector<const xAOD::AFPSiHitsCluster*> >::const_iterator LayersIter_t;

  // prepare list for storing temporary reconstructed tracks
  std::list<AFPSiDBasicKalmanToolTrack> reconstructedTracks;

  AFPLocRecoStationBasicObj my_stationClusters;
  
  // the track reconstruction will seg fault if there are less than 2 layers (seed cannot be created)
  my_stationClusters.setNumberOfLayers(m_numberOfLayersInStation);


  clearAllLayers(my_stationClusters);
  fillLayersWithClusters(my_stationClusters, hitsClusterContainer);

  // ===== do tracks reconstruction =====
  // start with making seeds by combining all hits from the first and second layers
  const LayersIter_t layersEnd = my_stationClusters.layers().end();
  LayersIter_t layersIterator = my_stationClusters.layers().begin();

  // make all combinations between first and second layer
  const std::vector<const xAOD::AFPSiHitsCluster*>& firstLayer = *layersIterator;
  ++layersIterator;
  const std::vector<const xAOD::AFPSiHitsCluster*>& secondLayer = *layersIterator;
  ++layersIterator;

  for (const xAOD::AFPSiHitsCluster* firstCluster : firstLayer)
    for (const xAOD::AFPSiHitsCluster* secondCluster : secondLayer) {
      // make the seed
      reconstructedTracks.emplace_back(firstCluster, secondCluster, m_observationModel, m_observationNoise, m_aposterioriCov);
      AFPSiDBasicKalmanToolTrack& theTrack = reconstructedTracks.back();

      // loop over remaining layers
      for (LayersIter_t remainingLayersIT = layersIterator; remainingLayersIT != layersEnd; ++remainingLayersIT) {
        const xAOD::AFPSiHitsCluster* closestCluster = theTrack.findNearestCluster(*remainingLayersIT, m_maxAllowedDistance);
        if (closestCluster != nullptr) // if there is a cluster near the track add it to the track
          theTrack.addCluster(closestCluster, m_clusterMaxChi2);
        else                        // if there is no cluster add a hole (missing cluster in the layer)
          theTrack.addHole();
      }

      // process the track only if there are enough clusters, remove the ones with less tracks 
      if (theTrack.clustersInTrack().size() >= m_minClustersNumber)
        theTrack.smooth();
      else 
        reconstructedTracks.pop_back();
        
    } // end of loop over seeds (all combinations between the first and second layer
  

  filterTrkCollection(reconstructedTracks);


  // === Save result to xAOD container ===
  for (const AFPSiDBasicKalmanToolTrack& track : reconstructedTracks)
  {
    saveToXAOD(track, outputContainer, hitsClusterContainer);
  }
  
  // monitoring
  auto trkStationID = Monitored::Collection("TrkStationID",*outputContainer, &xAOD::AFPTrack::stationID);
  auto trkXLocal    = Monitored::Collection("TrkXLocal",   *outputContainer, &xAOD::AFPTrack::xLocal);
  auto trkYLocal    = Monitored::Collection("TrkYLocal",   *outputContainer, &xAOD::AFPTrack::yLocal);
  auto trkZLocal    = Monitored::Collection("TrkZLocal",   *outputContainer, &xAOD::AFPTrack::zLocal);
  auto trkXSlope    = Monitored::Collection("TrkXSlope",   *outputContainer, &xAOD::AFPTrack::xSlope);
  auto trkYSlope    = Monitored::Collection("TrkYSlope",   *outputContainer, &xAOD::AFPTrack::ySlope);
  auto trkNClusters = Monitored::Collection("TrkNClusters",*outputContainer, &xAOD::AFPTrack::nClusters);
  auto trkNHoles    = Monitored::Collection("TrkNHoles",   *outputContainer, &xAOD::AFPTrack::nHoles);
  auto trkChi2      = Monitored::Collection("TrkChi2",     *outputContainer, &xAOD::AFPTrack::chi2);
  Monitored::Group(m_monTool, trkStationID, trkXLocal, trkYLocal, trkZLocal, trkXSlope, trkYSlope, trkNClusters, trkNHoles, trkChi2);
  
  return StatusCode::SUCCESS;
}

void AFPSiDBasicKalmanTool::saveToXAOD (const AFPSiDBasicKalmanToolTrack& recoTrack, std::unique_ptr<xAOD::AFPTrackContainer>& containerToFill, SG::ReadHandle<xAOD::AFPSiHitsClusterContainer>& hitsClusterContainer) const
{
  auto track = containerToFill->push_back(std::make_unique<xAOD::AFPTrack>());
  
  const xAOD::AFPSiHitsCluster* firstCluster = recoTrack.clustersInTrack().front();
  const CLHEP::HepVector& firstPoint = recoTrack.positionAndSlopeSmooth().back(); // reading from smoothed collection which is done in reversed order
  
  track->setStationID(firstCluster->stationID());
  track->setXLocal(firstPoint[0]);
  track->setYLocal(firstPoint[2]);
  track->setZLocal(firstCluster->zLocal());
  track->setXSlope(firstPoint[1]);
  track->setYSlope(firstPoint[3]);
  track->setNClusters(recoTrack.clustersInTrack().size());
  track->setNHoles(recoTrack.holes());
  track->setChi2(recoTrack.trkChi2NDFSmooth());
  track->setAlgID(xAOD::AFPTrackRecoAlgID::basicKalman);

  // add links to clusters
  ATH_MSG_DEBUG("Track position: (x="<<track->xLocal()<<", y="<<track->yLocal()<<", z="<<track->zLocal()<<")   slope: (dx="<<track->xSlope()<<", dy="<<track->ySlope()<<")   chi2="<<track->chi2()<<", nHoles="<<track->nHoles()<<", nClusters="<<track->nClusters());
  for (const xAOD::AFPSiHitsCluster* theCluster : recoTrack.clustersInTrack()) {
    ElementLink< xAOD::AFPSiHitsClusterContainer >* clusterLink = new ElementLink< xAOD::AFPSiHitsClusterContainer >; // will be taken over by the xAODCluster and deleted
    clusterLink->toContainedElement(*hitsClusterContainer, theCluster);
    track->addCluster(*clusterLink);

    ATH_MSG_DEBUG("cluster position: (x="<<theCluster->xLocal()<<", y="<<theCluster->yLocal()<<", z="<<theCluster->zLocal()<<")");
  }
}

void AFPSiDBasicKalmanTool::filterTrkCollection(std::list<AFPSiDBasicKalmanToolTrack>& tracksList) const
{
  //filtering tracking collection using shared hits + quality requirement
  std::list<AFPSiDBasicKalmanToolTrack>::iterator mainIterator = tracksList.begin();
  while (mainIterator != tracksList.end()) {

    bool deletedMain = false;
    // start comparing from the next object to the one currently testing
    std::list<AFPSiDBasicKalmanToolTrack>::iterator compareIterator = mainIterator;
    ++compareIterator;
    while (compareIterator != tracksList.end()) {
      if ( countSharedClusters(*mainIterator, *compareIterator) > m_maxSharedClusters) {
        // calculate quality of the tracks preferring tracks with more clusters
        const AFPSiDBasicKalmanToolTrack& mainTrk = (*mainIterator);
        const double mainTrkQuality = mainTrk.chi2Smooth().size() + ((m_trackMaxChi2 - mainTrk.trkChi2NDFSmooth()) / (m_trackMaxChi2 + 1.));
        AFPSiDBasicKalmanToolTrack& compareTrk = (*compareIterator);
        const double compareTrkQuality = compareTrk.chi2Smooth().size() + ((m_trackMaxChi2 - compareTrk.trkChi2NDFSmooth()) / (m_trackMaxChi2 + 1.));

        if ( mainTrkQuality >= compareTrkQuality) {
          tracksList.erase(compareIterator++);
          continue;
        }
        else {
          tracksList.erase(mainIterator++);
          deletedMain = true;
          break;
        }
      }

      ++compareIterator;
    } // end while (compareIterator)

    // go to the next object only if the main was not deleted, because iterator was already moved when deleting
    if (!deletedMain) ++mainIterator;
    
  } // end mainIterator
}

int AFPSiDBasicKalmanTool::countSharedClusters(const AFPSiDBasicKalmanToolTrack &firstTrack, const AFPSiDBasicKalmanToolTrack  &secondTrack) const
{
  int sharedClustersN = 0;

  for (const xAOD::AFPSiHitsCluster* firstCluster : firstTrack.clustersInTrack())
    for (const xAOD::AFPSiHitsCluster* secondCluster : secondTrack.clustersInTrack())
      if (firstCluster == secondCluster) {
        ++sharedClustersN;
        break;
      }
        
  return sharedClustersN;
}

void AFPSiDBasicKalmanTool::initMatrixFromVector (CLHEP::HepMatrix& matrix, const std::vector<float>& vec1D) const
{
  const int rowsN = matrix.num_row();
  const int columnsN = matrix.num_col();

  // check if size of matrix and vector are compatible
  if ( ((int)vec1D.size()) == rowsN*columnsN) { 
    // if sizes are compatible initialise matrix
    for (int rowID = 0; rowID < rowsN; rowID++)
      for (int columnID = 0; columnID < columnsN; columnID++)
        matrix[rowID][columnID] = vec1D[rowID*columnsN + columnID];
  }
  else {
    // if sizes are incompatible print warning message and do nothing
    std::stringstream warningMessage;
    warningMessage<<"Matrix size ("<<rowsN<<"x"<<columnsN
                  <<" = "<<rowsN*columnsN<<") is not compatible with vector size: "
                  <<vec1D.size()<<".";
    warningMessage<<"Matrix will not be initialised. The vector contains following numbers: ";
    for (const float number : vec1D)
      warningMessage<<number<<" ";

    ATH_MSG_WARNING (warningMessage.str());
  }
}
