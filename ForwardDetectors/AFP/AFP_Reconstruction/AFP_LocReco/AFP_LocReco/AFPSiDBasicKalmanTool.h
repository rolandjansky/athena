/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file   AFPSiDBasicKalmanTool.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-03-01
/// 
/// @brief  Header file for AFPSiDBasicKalmanTool used in tracks reconstruction.
#ifndef AFP_LOCRECO_AFPSIDBASICKALMANTOOL_H
#define AFP_LOCRECO_AFPSIDBASICKALMANTOOL_H 1

// STL includes
#include <string>
#include <list>
#include <vector>

#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// xAOD includes
#include "xAODForward/AFPSiHitsCluster.h"
#include "xAODForward/AFPSiHitsClusterContainer.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrackAuxContainer.h"

// AFP_LocReco includes
#include "AFP_LocRecoInterfaces/IAFPSiDLocRecoTrackAlgTool.h"
#include "AFP_LocReco/AFPLocRecoStationBasicObj.h"
#include "AFP_LocReco/AFPSiDBasicKalmanToolTrack.h"


/// Class reconstructing tracks using basic Kalman filter.
///
/// The idea of the reconstruction is presented in the talk
/// https://indico.cern.ch/event/275484/contributions/1618277/attachments/499062/689421/mdyndal_2013-10-15_AFP_Gen.pdf
class AFPSiDBasicKalmanTool
  : virtual public ::IAFPSiDLocRecoTrackAlgTool,
    public ::AthAlgTool
{
public:
  AFPSiDBasicKalmanTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  virtual ~AFPSiDBasicKalmanTool() override {}

  /// Read parameters from job options and print tool configuration
  virtual StatusCode initialize() override;

  /// Does nothing
  virtual StatusCode finalize() override;

  /// @brief Does actual tracks reconstruction.
  ///
  /// -# Reads clusters from the xAOD container.
  /// -# Creates seeds by making all combinations of clusters in the first and second planes.
  /// -# For each seed all remaining layers are checked for clusters that fit the track. If the cluster is found it is added to the track if not a number of holes (xAOD::AFPTrack::m_holes) is incremented.
  /// -# If the track has more clusters than #m_minClustersNumber a smoothing algorithm is run on it and added to the output list.
  /// -# The output list of tracks if filtered for duplicates using AFPSiDBasicKalmanTool::filterTrkCollection().
  /// -# The output list is saved to xAOD containers and objects.
  StatusCode reconstructTracks(xAOD::AFPTrackContainer* outputContainer) override;

  /// @copydoc IAFPSiDLocRecoTrackAlgTool::outputContainerName(xAOD::AFPTrackContainer*)
  const std::string& outputContainerName () const override {return m_tracksContainerName;}

  /// @brief Filters duplicate tracks from the list
  ///
  /// The tracks are considered duplicates if more than
  /// #m_maxSharedClusters clusters are the same in both tracks. In
  /// such situation track quality is calculated according to the
  /// formula @f$q = N_{clusters} + ((\chi^2_{trk, max} -
  /// \chi^2{trk})/(\chi^2_{trk, max} + 1))@f$. The track with
  /// smaller value of @f$q@f$ is deleted.
  void filterTrkCollection(std::list<AFPSiDBasicKalmanToolTrack>& reconstructedTracks) const;

  /// Returns number of the same clusters used to reconstruct two tracks.
  int countSharedClusters(const AFPSiDBasicKalmanToolTrack& firstTrack, const AFPSiDBasicKalmanToolTrack& secondTrack) const;
		
  /// clear all layers from clusters saved in #m_stationsClusters;
  void clearAllLayers()
  {m_stationClusters.clear();}


private:
  /// Name of the xAOD container to which tracks will be saved
  std::string m_tracksContainerName;

  /// AFP station ID for which tracks will be reconstructed
  int m_stationID;

  /// @brief The observation model which maps the true state space into the observed space (@f$H_{k}@f$)
  ///
  /// This is 2x4 matrix, where dimension 2 comes from two position
  /// variables (x, y) and dimension 4 comes from position and slope
  /// in each of the two directions.
  /// @f$z_{k} = H_{k} x_{k} + v_{k}@f$
  ///
  /// If no proper observationModel array is defined in python
  /// configuration the default unit transformation is used - X and Y
  /// position are simply copied.
  CLHEP::HepMatrix m_observationModel;

  /// @brief A vector used to initialise #m_observationModel matrix.
  ///
  /// The first four numbers correspond to the first row of the  matrix.
  std::vector<float> m_observationModelInit;


  /// @brief The observation noise matrix. (@f$v_{k}@f$)
  ///
  /// This is 2x2 matrix 
  /// In standard Kalman it is assumed to be zero mean Gaussian white
  /// noise with covariance specified by the matrix.
  /// @f$z_{k} = H_{k} x_{k} + v_{k}@f$
  ///
  /// If the matrix is not defined in job options it set to be
  /// diagonal matrix with squared sizes of pixel.
  CLHEP::HepMatrix m_observationNoise;

  /// @brief A vector used to initialise #m_observationNoise matrix.
  ///
  /// The first two numbers correspond to the first row of the matrix.
  std::vector<float> m_observationNoiseInit;


  /// @brief The covariance matrix of process noise. (@f$Q_{k}@f$)
  ///
  /// This is 4x4 matrix.
  ///
  /// If the matrix is not defined in job options it set to 0.
  CLHEP::HepMatrix m_processNoiseCov;

  /// @brief A vector used to initialise #m_processNoiseCov matrix.
  ///
  /// The first 4 numbers correspond to the first row of the matrix.
  std::vector<float> m_processNoiseCovInit;

  /// @brief A posteriori error covariance matrix (a measure of the estimated
  /// accuracy of the state estimate) (@f$P_{k|k}@f$)
  ///
  /// This is 4x4 matrix.
  ///
  /// If the matrix is not defined in job options it set to 0.
  CLHEP::HepMatrix m_aposterioriCov;

  /// @brief A vector used to initialise #m_aposterioriCov matrix.
  ///
  /// The first 4 numbers correspond to the first row of the matrix.
  std::vector<float> m_aposterioriCovInit;

  /// @brief Number of layers used for reconstruction in station

  /// If not set in job options 4 stations, each with 4 layers are
  /// created.
  int m_numberOfLayersInStation;

  /// Hit clusters divided into layers in an object representing a station
  AFPLocRecoStationBasicObj m_stationClusters;

  /// Name of the xAOD container with clusters to be used in track reconstruction
  std::string m_hitsClusterContainerName;

  /// Container with clusters from which tracks are reconstructed
  const xAOD::AFPSiHitsClusterContainer* m_hitsClusterContainer;

  /// Maximal distance at which cluster can be joined to the track (Default = 100 - all clusters)
  double m_maxAllowedDistance;

  /// Minimal number of clusters in track. If there are less clusters track is rejected (Default = 3)
  unsigned int m_minClustersNumber;

  /// Maximal number of hits that two tracks can share. If they share more one is deleted.
  int m_maxSharedClusters;

  /// Maximal value of chi2 for which a cluster is added.
  float m_clusterMaxChi2;

  /// Maximal value of chi2 for the track.
  float m_trackMaxChi2;
  
  /// Fills layers with clusters of hits, dividing them into stations and layers
  void fillLayersWithClusters();

  /// @brief Method that initialises 2D matrix using values from the vector.
  ///
  /// The method checks if the matrix size agrees with the vector size
  /// (N_columns*N_rows = N_vector). If the sizes are incompatible the
  /// matrix is unchanged.
  ///
  /// @param[out] the matrix to be initialised
  /// @param[in] the vector with values to be copied to the matrix
  void initMatrixFromVector (CLHEP::HepMatrix& matrix, const std::vector<float>& vec1D) const;

  /// Returns true if vector size equals matrix rows times columns
  bool checkMatrixAndVectorSize(const CLHEP::HepMatrix& matrix, const std::vector<float>& vec1D) const
  {return ((int)vec1D.size()) == matrix.num_row()*matrix.num_col();}

  /// Save reconstructed track to the xAOD container
  void saveToXAOD (const AFPSiDBasicKalmanToolTrack& recoTrack, xAOD::AFPTrackContainer* containerToFill) const;
};


#endif // > !AFP_LOCRECO_AFPSIDBASICKALMANTOOL_H
