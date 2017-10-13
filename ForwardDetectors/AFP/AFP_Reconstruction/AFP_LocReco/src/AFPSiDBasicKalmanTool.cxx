/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  ::AthAlgTool  ( type, name, parent ),
  m_hitsClusterContainer (nullptr)
{
  declareInterface< IAFPSiDLocRecoTrackAlgTool >(this);

  declareProperty( "hitsClusterContainerName", m_hitsClusterContainerName = "AFPSiHitsClusterContainer", "Name of the container with clusters of hits from which tracks are to be reconstructed" );
  declareProperty( "tracksContainerName", m_tracksContainerName = "AFPTrackContainer", "Name of the container in which tracks are saved");

  declareProperty( "stationID", m_stationID = 0, "ID number of station for which tracks should be reconstructed");

  declareProperty ("numberOfLayersInStations", m_numberOfLayersInStation = 4, "The size of the vector sets number of stations. Each element defines number of pixel layers in the station.");
  declareProperty ("maxAllowedDistance", m_maxAllowedDistance = 10, "Maximal distance at which cluster can be joined to the track");

  declareProperty ("minClustersNumber", m_minClustersNumber = 3, "Minimal number of clusters in track. If there are less clusters track is rejected");

  declareProperty ("maxSharedClusters", m_maxSharedClusters = 2, "Maximal number of hits that two tracks can share. If they share more one is deleted.");

  declareProperty ("clusterMaxChi2", m_clusterMaxChi2 = 3, "Maximal value of chi2 for which a cluster is added.");
  declareProperty ("trackMaxChi2", m_trackMaxChi2 = 3, "Maximal value of chi2 for the track.");

  // Kalman matrices initialisation
  declareProperty ("observationModel", m_observationModelInit, "vector used to initialise observation model matrix (2x4), the first four numbers correspond to the first row of the matrix");
  declareProperty ("observationNoise", m_observationNoiseInit, "vector used to initialise observation noise matrix (2x2), the first two numbers correspond to the first row of the matrix");
  declareProperty ("processNoiseCov", m_processNoiseCovInit, "A vector used to initialise process noise covariance matrix (4x4).The first 4 numbers correspond to the first row of the matrix.");
  declareProperty ("aposterioriCov", m_aposterioriCovInit, "A vector used to initialise a posteriori covariance matrix (4x4). The first 4 numbers correspond to the first row of the matrix.");
}

StatusCode AFPSiDBasicKalmanTool::initialize()
{
  // === Kalman matrices initialization ===

  // --- initialise observation model matrix ---
  m_observationModel = CLHEP::HepMatrix(2, 4, 0);
  // if proper m_observationModelInit is available use it
  if (checkMatrixAndVectorSize(m_observationModel, m_observationModelInit)) {
    initMatrixFromVector (m_observationModel, m_observationModelInit);
  }
  else {			// otherwise use default unit transformation
    m_observationModel[0][0] = 1.;
    m_observationModel[1][2] = 1.;
  }
  ATH_MSG_INFO("Observation model matrix (Hk) is initialised to: "<<m_observationModel);

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
  ATH_MSG_INFO("Observation noise  matrix (Vk) is initialised to: "<<m_observationNoise);

  // --- set covariance matrix of the process noise
  m_processNoiseCov = CLHEP::HepMatrix(4, 4, 0); // initialise with default values
  if (checkMatrixAndVectorSize(m_processNoiseCov, m_processNoiseCovInit)) {
    // use matrix from job options if set
    initMatrixFromVector(m_processNoiseCov, m_processNoiseCovInit);
  }
  ATH_MSG_INFO("Process noise covariance matrix (Qk) is initialised to: "<<m_processNoiseCov);


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

  ATH_MSG_INFO("A posteriori error covariance matrix (Pkk) is initialised to: "<<m_aposterioriCov);

  if (m_numberOfLayersInStation >= 2)		// the track reconstruction will seg fault if there are less than 2 layers (seed cannot be created)
    m_stationClusters.setNumberOfLayers(m_numberOfLayersInStation);
  else {
    ATH_MSG_ERROR ("Impossible to reconstruct tracks with less than 2 layers in station, but configured to run with "<<m_numberOfLayersInStation<<".");
    return StatusCode::FAILURE;
  }

  // print information about initialised layers and stations
  ATH_MSG_INFO("Station with ID="<<m_stationID <<" will have "<<m_numberOfLayersInStation<<" layers.");


  // print information about remaining parameters
  ATH_MSG_INFO("Maximal distance at which cluster can be joined to the track m_maxAllowedDistance = "<<m_maxAllowedDistance);
  ATH_MSG_INFO("Minimal number of clusters in track. If there are less clusters track is rejected m_minClustersNumber = "<<m_minClustersNumber);
  ATH_MSG_INFO("Maximal chi2 of cluster to be added to track = "<<m_clusterMaxChi2);
  ATH_MSG_INFO("Maximal chi2 of the track = "<<m_trackMaxChi2);

  return StatusCode::SUCCESS;
}

StatusCode AFPSiDBasicKalmanTool::finalize()
{
  return StatusCode::SUCCESS;
}

void AFPSiDBasicKalmanTool::fillLayersWithClusters()
{
  // retrieve clusters
  if (evtStore()->retrieve(m_hitsClusterContainer, m_hitsClusterContainerName).isSuccess()) {
    try {
      // fill layers with clusters
      for (const xAOD::AFPSiHitsCluster* theCluster : *m_hitsClusterContainer)
	if (theCluster->stationID() == m_stationID) // check if cluster is from the correct station
	  m_stationClusters.clustersInLayer(theCluster->pixelLayerID()).push_back(theCluster);
    }
    catch (const std::out_of_range& outOfRange) {
      ATH_MSG_WARNING("Cluster with station or pixel ID outside expected range. Aborting track reconstruction.");
      clearAllLayers();
    }
  }
  else
    ATH_MSG_WARNING("Failed to retrieve clusters container.");
}

StatusCode AFPSiDBasicKalmanTool::reconstructTracks(xAOD::AFPTrackContainer* outputContainer)
{
  typedef std::vector< std::vector<const xAOD::AFPSiHitsCluster*> >::const_iterator LayersIter_t;

  // prepare list for storing temporary reconstructed tracks
  std::list<AFPSiDBasicKalmanToolTrack> reconstructedTracks;

  clearAllLayers();
  fillLayersWithClusters();

  // ===== do tracks reconstruction =====
  // start with making seeds by combining all hits from the first and second layers
  const LayersIter_t layersEnd = m_stationClusters.layers().end();
  LayersIter_t layersIterator = m_stationClusters.layers().begin();

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
	else			// if there is no cluster add a hole (missing cluster in the layer)
	  theTrack.addHole();
      }

      // process the track only if there are enough clusters, remove the ones with less tracks 
      if (theTrack.clustersInTrack().size() >= m_minClustersNumber)
	theTrack.smooth();
      else 
	reconstructedTracks.pop_back();
	
    }	// end of loop over seeds (all combinations between the first and second layer
  

  filterTrkCollection(reconstructedTracks);


  // === Save result to xAOD ===
  // save to xAOD
  for (const AFPSiDBasicKalmanToolTrack& track : reconstructedTracks)
    saveToXAOD(track, outputContainer);
  
  return StatusCode::SUCCESS;
}

void AFPSiDBasicKalmanTool::saveToXAOD (const AFPSiDBasicKalmanToolTrack& recoTrack, xAOD::AFPTrackContainer* containerToFill) const
{
  xAOD::AFPTrack* track = new xAOD::AFPTrack;
  containerToFill->push_back(track);
  
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

  // add links to clusters
  ATH_MSG_DEBUG("Track position: (x="<<track->xLocal()<<", y="<<track->yLocal()<<", z="<<track->zLocal()<<")   slope: (dx="<<track->xSlope()<<", dy="<<track->ySlope()<<")   chi2="<<track->chi2());
  for (const xAOD::AFPSiHitsCluster* theCluster : recoTrack.clustersInTrack()) {
    ElementLink< xAOD::AFPSiHitsClusterContainer >* clusterLink = new ElementLink< xAOD::AFPSiHitsClusterContainer >; // will be taken over by the xAODCluster and deleted
    clusterLink->toContainedElement(*m_hitsClusterContainer, theCluster);
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
