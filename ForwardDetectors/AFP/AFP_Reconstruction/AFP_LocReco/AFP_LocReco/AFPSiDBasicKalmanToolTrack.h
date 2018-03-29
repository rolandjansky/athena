/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file   AFPSiDBasicKalmanToolTrack.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-04-29
/// 
/// @brief  Header file for AFPSiDBasicKalmanToolTrack used in tracks reconstruction with Kalman filter.

#ifndef AFP_LOCRECO_AFPSIDBASICKALMANTOOLTRACK_H
#define AFP_LOCRECO_AFPSIDBASICKALMANTOOLTRACK_H 1

#include <list>

#include "xAODForward/AFPSiHitsCluster.h"


#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>

/// Class representing a reconstructed track with Kalman filter.
class AFPSiDBasicKalmanToolTrack
{
public:

  /// @brief Main constructor that creates the track seed from two clusters.
  ///
  /// It adds the two clusters to the list of clusters used to
  /// reconstruct the track. It sets the position of the last point of
  /// the track to be equal to the position of the second cluster and
  /// calculates track slopes using first and second clusters. It adds
  /// first entry to #m_chi2History and sets it to 0.
  ///
  /// @param firstCluster pointer to the first cluster of the track
  /// @param secondCluster pointer to the second cluster of the track
  /// @param observationModel matrix representing observation model (see #m_observationModel)
  /// @param observationNoise see #m_observationNoise
  /// @param covarianceMatrix initial error covariance matrix (see #m_positionAndSlopeCovHistory)
  AFPSiDBasicKalmanToolTrack (const xAOD::AFPSiHitsCluster* firstCluster, const xAOD::AFPSiHitsCluster* secondCluster, const CLHEP::HepMatrix& observationModel, const CLHEP::HepMatrix& observationNoise, const CLHEP::HepMatrix& covarianceMatrix);

  /// @brief Create state transition matrix to calculate x and y position and slopes between to Z positions
  ///
  /// The matrix leaves slopes unchanged and adds to the previous
  /// positions slopes multiplied by the z distance i.e. secondPos - firstPos.
  /// @f[
  /// \left(
  /// \begin{array}{cccc}
  /// 1 & \Delta z & 0 & 0 \\       
  /// 0 & 1 & 0 & 0 \\      
  /// 0 & 0 & 1 & \Delta z \\     
  /// 0 & 0 & 0 & 1
  /// \end{array}
  /// \right)
  /// @f]
  inline CLHEP::HepMatrix transitionModel (const double firstPos, const double secondPos) const;

  /// @brief Create state transition matrix to calculate x and y position and slopes at given Z
  ///
  /// The matrix for transition between the last point and given Z position is calculated.
  ///
  /// @copydetails AFPSiDBasicKalmanToolTrack::transitionModel(const double, const double)
  CLHEP::HepMatrix transitionModel (const double zPosition) const
  {return transitionModel (m_zPositionHistory.back(),  zPosition);}
    
  /// Predict x and y positions and slopes of the track at given Z
  CLHEP::HepVector predictNextPoint (const double zPosition) const
  {return transitionModel(zPosition)*m_positionAndSlopeHistory.back();}

  /// Predict covariance matrix of the track at given Z
  inline CLHEP::HepMatrix predictNextPointCov (const double zPosition) const
  {CLHEP::HepMatrix transition = transitionModel(zPosition);
    return transition*m_positionAndSlopeCovHistory.back()*transition.T();}


  /// @brief Finds the cluster which is nearest to the track, returns nullptr if no is found.
  ///
  /// The method predicts what are track X and Y coordinates for the Z
  /// of a cluster. Next, loops over the clusters in the vector and
  /// calculates distance @f$r = \sqrt{\Delta x^2 + \Delta
  /// y^2}@f$. Returns nullptr if there is no cluster which is closer
  /// than #maxAllowedDistance or if there are clusters with r <
  /// #maxAllowedDistance a pointer to the closest one.
  ///
  /// @param clusters vector of clusters from which the closest one is to be found
  /// @param maxAllowedDistance if no cluster is closer than this argument nullptr is returned
  const xAOD::AFPSiHitsCluster* findNearestCluster (const std::vector<const xAOD::AFPSiHitsCluster*>& clusters, const float maxAllowedDistance) const;


  /// Adds a new cluster to the track and updates history and position
  void addCluster (const xAOD::AFPSiHitsCluster* cluster, const float htiMaxChi2);

  /// @copydoc AFPSiDBasicKalmanToolTrack::m_holes;
  int holes () const {return m_holes;}

  /// Increase number of holes by 1. (see #m_holes)
  void addHole() {++m_holes;}

  /// @copydoc AFPSiDBasicKalmanToolTrack::m_clustersInTrack
  const std::list<const xAOD::AFPSiHitsCluster*>& clustersInTrack () const {return m_clustersInTrack;}

  /// Clears matrices used for smoothing the track
  void clearSmoothMatrices ();

  /// @copydoc AFPSiDBasicKalmanToolTrack::m_positionAndSlopeSmooth
  const std::list< CLHEP::HepVector >& positionAndSlopeSmooth () const {return m_positionAndSlopeSmooth;}

  /// @copydoc AFPSiDBasicKalmanToolTrack::m_positionAndSlopeHistory
  const std::list< CLHEP::HepVector >& positionAndSlopeHistory () const {return m_positionAndSlopeHistory;}

  /// @copydoc AFPSiDBasicKalmanToolTrack::m_chi2Smooth
  const std::list< double >& chi2Smooth() const {return m_chi2Smooth;}

  /// @brief Chi2 per degrees of freedom (clusters) for the whole smoothed track
  ///
  /// Sum chi2 for each cluster and divide by the number of clusters.
  double trkChi2NDFSmooth() const;

  /// @brief Run smoothing algorithm
  ///
  /// The algorithm should probably just reconstruct track starting
  /// from the last hit, but the covaraince matrices are changed, so
  /// the result maybe slightly different. Algorithm is just copied
  /// from the previous implementation.
  void smooth ();
protected:
  

  /// Vectors of reconstructed true positions for each step.
  std::list< float > m_zPositionHistory;

  /// @brief Vectors of reconstructed true positions for each step @f$x_{k}@f$.
  ///
  /// Each 4D vector containing x, y position of the track in a given
  /// step as well as slopes in these two directions @f$\Delta x,
  /// \Delta y@f$. @f$x_{k} = (x, \Delta x, y, \Delta y)@f$
  std::list< CLHEP::HepVector > m_positionAndSlopeHistory;

  /// @brief Vectors of predicted true position of a track for each step.
  ///
  /// Each 4D vector containing x, y position of the track in a given
  /// step as well as slopes in these two directions @f$\Delta x,
  /// \Delta y@f$. @f$x_{k} = (x, \Delta x, y, \Delta y)@f$
  std::list< CLHEP::HepVector > m_positionAndSlopePredictedHistory;

  /// Matrices of updated (a posteriori) estimate covariance of the position and slope for each step @f$P_{k|k}@f$.
  std::list< CLHEP::HepMatrix > m_positionAndSlopeCovHistory;

  /// Matrices of predicted estimate covariance of the position and slope for each step.
  std::list< CLHEP::HepMatrix > m_positionAndSlopeCovPredictedHistory;

  /// Matrices of transitionModel used at each step of track reconstruction
  std::list< CLHEP::HepMatrix > m_transitionHistory;

  /// History of chi2 for each step
  std::list< double> m_chi2History;

  /// @brief Vectors of reconstructed true positions for each step after smoothing @f$x_{k}@f$.
  ///
  /// Each 4D vector containing x, y position of the track in a given
  /// step as well as slopes in these two directions @f$\Delta x,
  /// \Delta y@f$. @f$x_{k} = (x, \Delta x, y, \Delta y)@f$
  std::list< CLHEP::HepVector > m_positionAndSlopeSmooth;

  /// Matrices of updated (a posteriori) estimate covariance of the position and slope for each step after smoothing @f$P_{k|k}@f$
  std::list< CLHEP::HepMatrix > m_positionAndSlopeSmoothCov;

  /// History of chi2 for each step after smoothing
  std::list< double > m_chi2Smooth;
  
  /// Clusters used to reconstruct the track
  std::list<const xAOD::AFPSiHitsCluster*> m_clustersInTrack;

  /// Observation model, describing transformation from true to measured state.
  const CLHEP::HepMatrix& m_observationModel; 

  /// @brief Matrix representing observation noise disturbing the measurement.
  ///
  /// Used to estimate covariance matrix of the track.
  const CLHEP::HepMatrix& m_observationNoise;


  /// Number of layers in which hit from a track is expected, but is not present
  int m_holes;
};

inline CLHEP::HepMatrix AFPSiDBasicKalmanToolTrack::transitionModel (const double firstPos, const double secondPos) const
{
  CLHEP::HepMatrix transitionModel (4, 4, 1);	// create unit matrix (1 on diagonal, 0 offdiagonal)
  const double zDistance = secondPos - firstPos;
  transitionModel[0][1] = zDistance;// set linear transformation which sets x' = x + dx*dz
  transitionModel[2][3] = zDistance; // set linear transformation which sets y' = y + dy*dz

  return transitionModel;
}

#endif
