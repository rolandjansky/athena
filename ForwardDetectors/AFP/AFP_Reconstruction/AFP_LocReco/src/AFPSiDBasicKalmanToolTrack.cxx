/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file   AFPSiDBasicKalmanToolTrack.cxx
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-04-30
/// 
/// @brief  Implementation file for AFPSiDBasicKalmanToolTrack used in tracks reconstruction with Kalman filter.

#include <math.h>

// debug remove after debugging
#include<iostream>
// end remove after debugging

#include "AFP_LocReco/AFPSiDBasicKalmanToolTrack.h"

AFPSiDBasicKalmanToolTrack::AFPSiDBasicKalmanToolTrack (const xAOD::AFPSiHitsCluster* firstCluster, const xAOD::AFPSiHitsCluster* secondCluster, const CLHEP::HepMatrix& observationModel, const CLHEP::HepMatrix& observationNoise, const CLHEP::HepMatrix& covarianceMatrix) :
  m_observationModel(observationModel),
  m_observationNoise(observationNoise),
  m_holes(0)
{
  m_positionAndSlopeHistory.emplace_back(4);

  //  const double zDistance = fabs(secondCluster->zLocal() - firstCluster->zLocal()); // absolute value taken in case global z position is used, in that case the distance would be negative for stations on the A side
  const double zDistance = secondCluster->zLocal() - firstCluster->zLocal(); // absolute value taken in case global z position is used, in that case the distance would be negative for stations on the A side
  
  CLHEP::HepVector& positionAndSlope = m_positionAndSlopeHistory.back();
  positionAndSlope[0] = secondCluster->xLocal(); // set track X position 
  positionAndSlope[1] = (secondCluster->xLocal() - firstCluster->xLocal())/zDistance; // set track X slope
  positionAndSlope[2] = secondCluster->yLocal(); // set track Y position
  positionAndSlope[3] = (secondCluster->yLocal() - firstCluster->yLocal())/zDistance; // set track Y slope

  // set z position of the track
  m_zPositionHistory.push_back(secondCluster->zLocal());

  // add clusters to the list 
  m_clustersInTrack.push_back(firstCluster);
  m_clustersInTrack.push_back(secondCluster);

  // set the first covariance matrix
  m_positionAndSlopeCovHistory.emplace_back(covarianceMatrix);

  // set chi2 to 0 for two points
  m_chi2History.emplace_back(0);
}

void AFPSiDBasicKalmanToolTrack::addCluster (const xAOD::AFPSiHitsCluster* cluster, const float clusterMaxChi2)
{
  // prepare a vector with new observation point coordinates
  CLHEP::HepVector newPoint (2);
  newPoint[0] = cluster->xLocal();
  newPoint[1] = cluster->yLocal();

  // get the values for the predicted new point
  const double newZ = cluster->zLocal();
 
  const CLHEP::HepVector predictedTruePosition = predictNextPoint(newZ);
  const CLHEP::HepMatrix predictedCovariance = predictNextPointCov(newZ);

  // difference between measured and predicted (called innovation y_k and S_k)
  const CLHEP::HepVector residualPredicted = newPoint - m_observationModel*predictedTruePosition;
  const CLHEP::HepMatrix residualPredictedCov = m_observationNoise + m_observationModel*predictedCovariance*m_observationModel.T();

  // optimal Kalman gain (Kk)
  const CLHEP::HepMatrix kalmanGain = predictedCovariance*m_observationModel.T()*qr_inverse(residualPredictedCov);

  // === calculated new values ===
  // Updated (a posteriori) state estimate
  const CLHEP::HepVector newPosition = predictedTruePosition + kalmanGain*residualPredicted;

  // Updated (a posteriori) residuals
  const CLHEP::HepVector newResidual = newPoint - m_observationModel*newPosition;
  const CLHEP::HepMatrix newResidualCov = m_observationNoise - m_observationModel*kalmanGain*m_observationNoise;

  // chi2 statistics
  const CLHEP::HepVector chi2Vector = newResidual.T()*qr_inverse(newResidualCov)*newResidual;

  // check if hit is good
  if (chi2Vector[0] < clusterMaxChi2) {
    // add cluster and all information
    m_clustersInTrack.push_back(cluster);
    m_zPositionHistory.push_back(newZ);
    m_transitionHistory.emplace_back(transitionModel(newZ));
    m_positionAndSlopePredictedHistory.push_back(predictedTruePosition);
    m_positionAndSlopeCovPredictedHistory.push_back(predictedCovariance);
    m_positionAndSlopeHistory.push_back(newPosition);
    m_chi2History.emplace_back(chi2Vector[0]);
    // Updated (a posteriori) estimate covariance
    m_positionAndSlopeCovHistory.emplace_back(predictedCovariance - kalmanGain*m_observationModel*predictedCovariance);
  }
  else {
    addHole();
  }
}

const xAOD::AFPSiHitsCluster* AFPSiDBasicKalmanToolTrack::findNearestCluster (const std::vector<const xAOD::AFPSiHitsCluster*>& clusters, const float maxAllowedDistance) const
{
  double minDisntace = maxAllowedDistance;
  const xAOD::AFPSiHitsCluster* nearestCluster = nullptr;
  for (const xAOD::AFPSiHitsCluster* theCluster : clusters) {
    const CLHEP::HepVector predictedPoint = predictNextPoint(theCluster->zLocal());
    const double xDiff = predictedPoint[0] - theCluster->xLocal();
    const double yDiff = predictedPoint[2] - theCluster->yLocal(); // index 2, because this is (x, dx, y, dy) vector
    const double distance = sqrt(xDiff*xDiff + yDiff*yDiff);
    if (distance < minDisntace) {
      minDisntace = distance;
      nearestCluster = theCluster;
    }
  }

  return nearestCluster;
}

void AFPSiDBasicKalmanToolTrack::clearSmoothMatrices ()
{
  m_positionAndSlopeSmooth.clear();
  m_positionAndSlopeSmoothCov.clear();
  m_chi2Smooth.clear();
}

void AFPSiDBasicKalmanToolTrack::smooth ()
{
  clearSmoothMatrices();

  // prepare iterators for going backwards through the lists
  std::list< CLHEP::HepVector >::const_iterator positionIter = m_positionAndSlopeHistory.end();
  std::list< CLHEP::HepVector >::const_iterator positionBegin = m_positionAndSlopeHistory.begin();
  // if no position is reconstructed there is nothing to be done
  if (positionIter != positionBegin) {
    // prepare remaining iterators
    std::list< CLHEP::HepMatrix >::const_iterator covIter = m_positionAndSlopeCovHistory.end();
    std::list< const xAOD::AFPSiHitsCluster* >::const_iterator clustersIter = m_clustersInTrack.end();
    std::list< CLHEP::HepVector >::const_iterator positionPredictIter = m_positionAndSlopePredictedHistory.end();
    std::list< CLHEP::HepMatrix >::const_iterator covPredictIter  = m_positionAndSlopeCovPredictedHistory.end();
    std::list< CLHEP::HepMatrix >::const_iterator transitionIter  = m_transitionHistory.end();
    
    // === prepare first point ===
    // iterator moved back by 1, because end() points after the last element
    const CLHEP::HepVector& lastPosition = *(--positionIter);
    const CLHEP::HepMatrix& lastCovariance = *(--covIter);
    const xAOD::AFPSiHitsCluster* lastCluster = *(--clustersIter);

    m_positionAndSlopeSmooth.push_back(lastPosition);
    m_positionAndSlopeSmoothCov.push_back(lastCovariance);

    // prepare a vector with new observation point coordinates
    CLHEP::HepVector lastPoint (2);
    lastPoint[0] = lastCluster->xLocal();
    lastPoint[1] = lastCluster->yLocal();

    const CLHEP::HepVector residualSmooth = lastPoint - m_observationModel*lastPosition;
    const CLHEP::HepMatrix residualSmoothCov = m_observationNoise - m_observationModel*lastCovariance*m_observationModel.T();
    const CLHEP::HepVector chi2Smooth = residualSmooth.T()*qr_inverse(residualSmoothCov)*residualSmooth;
    m_chi2Smooth.push_back(chi2Smooth[0]);

    // === loop over remaining clusters ===
    while (positionIter != positionBegin) { 
      // --- read needed variables ---
      const CLHEP::HepVector& position = *(--positionIter);
      const CLHEP::HepMatrix& covariance = *(--covIter);
      const xAOD::AFPSiHitsCluster* cluster = *(--clustersIter);

      // shifted by 1 in history with respect to the position and covariance
      const CLHEP::HepVector& prevPositionPred = *(--positionPredictIter);
      const CLHEP::HepMatrix& prevCovPred = *(--covPredictIter);
      const CLHEP::HepMatrix& prevTransition = *(--transitionIter);

      // --- calculate the smoothed position and covariance ---
      const CLHEP::HepMatrix Ai = qr_inverse(prevTransition); // when Qk == 0
      //      const CLHEP::HepMatrix Ai = covariance * prevTransition.T() * qr_inverse(prevCovPred);		
      const CLHEP::HepVector positionSmooth = position + Ai*( m_positionAndSlopeSmooth.back() - prevPositionPred );
      const CLHEP::HepMatrix covarianceSmooth = covariance + Ai*( m_positionAndSlopeSmoothCov.back() - prevCovPred )*Ai.T();
      
      // --- calculate smoothed chi2 ---
      CLHEP::HepVector point (2);
      point[0] = cluster->xLocal();
      point[1] = cluster->yLocal();
      const CLHEP::HepVector residualSmooth = point - m_observationModel*positionSmooth;
      const CLHEP::HepMatrix residualCovSmooth = m_observationNoise - m_observationModel*covarianceSmooth*m_observationModel.T();
      CLHEP::HepVector chi2Smooth = residualSmooth.T()*qr_inverse(residualCovSmooth)*residualSmooth;

      // --- add results to the lists ---
      m_positionAndSlopeSmooth.push_back(positionSmooth);
      m_positionAndSlopeSmoothCov.push_back(covarianceSmooth);
      m_chi2Smooth.push_back(chi2Smooth[0]);
    } // end while over points/clusters


    // === extrapolate to the first plane ===
    clustersIter = m_clustersInTrack.begin();    
    const xAOD::AFPSiHitsCluster* firstCluster = *clustersIter;
    const xAOD::AFPSiHitsCluster* secondCluster = *(++clustersIter);
    const CLHEP::HepMatrix firstTransition = transitionModel(firstCluster->zLocal(), secondCluster->zLocal());

    const CLHEP::HepMatrix Ai = qr_inverse(firstTransition); // when Qk == 0
    const CLHEP::HepVector firstPosition = Ai*(m_positionAndSlopeSmooth.back());
    const CLHEP::HepMatrix firstCovariance = m_positionAndSlopeCovHistory.front() + Ai*( m_positionAndSlopeSmoothCov.back() - m_positionAndSlopeCovHistory.front())*Ai.T();

    // --- calculate smoothed chi2 ---
    const CLHEP::HepVector firstResidual = m_observationModel*Ai*(m_positionAndSlopeHistory.front() - m_positionAndSlopeSmooth.back());
    CLHEP::HepVector firstPoint (2);
    firstPoint[0] = firstCluster->xLocal();
    firstPoint[1] = firstCluster->yLocal();
    //    const CLHEP::HepVector firstResidual = firstPoint - m_observationModel*firstPosition;
    const CLHEP::HepMatrix firstResidualCov = m_observationNoise - m_observationModel*firstCovariance*m_observationModel.T();
    CLHEP::HepVector firstChi2 = firstResidual.T()*qr_inverse(firstResidualCov)*firstResidual;

    // --- add results to the lists ---
    m_positionAndSlopeSmooth.push_back(firstPosition);
    m_positionAndSlopeSmoothCov.push_back(firstCovariance);
    m_chi2Smooth.push_back(firstChi2[0]);
  }
}

double AFPSiDBasicKalmanToolTrack::trkChi2NDFSmooth() const
{
  double chi2 = 0;
  for (double val : m_chi2Smooth) {
    chi2 += val;
  }
  
  return chi2/m_chi2Smooth.size();
}
