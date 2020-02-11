/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 *
 * @brief Helper functions for pufit calculations
 * @author Bob Kowalewski, Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_PUFITUTILS_H
#define TRIGEFMISSINGET_PUFITUTILS_H

#include <Eigen/Dense>
#include "TrigEFMissingET/SignedKinematics.h"
#include "TrigEFMissingET/PufitGrid.h"
#include <vector>

namespace HLT { namespace MET { namespace PufitUtils {
  /// Helper struct to hold the sum over pileup objects and its covariance
  struct CovarianceSum {
    /// Default constructor - zero initialize everything.
    CovarianceSum();
    /// Construct with an existing sum and matrix
    CovarianceSum(const Eigen::Vector2d& sum, const Eigen::Matrix2d& covariance);
    /**
     * @brief Add a new contribution to the sum
     * @brief kin The kinematics of the contribution
     * @brief sigma The resolution of the contribution
     */
    CovarianceSum& add(const SignedKinematics& kin, double sigma);
    /// The sum
    Eigen::Vector2d sum;
    /// The covariance matrix
    Eigen::Matrix2d covariance;
  };

  /**
   * @brief Calculate the trimmed mean and variance for a grid of towers
   * @param grid The grid to calculate
   * @param trimFraction The fraction of towers to use in the calculation.
   * @return A pair containing the mean in 'first' and variance in 'second'
   *
   * Any masks applied to the grid will be ignored.
   */
  std::pair<double, double> trimmedMeanAndVariance(
      const PufitGrid& grid,
      double trimFraction);

  /**
   * @brief Calculate the mean and variance of unmasked towers
   * @param grid The grid to calculate
   * @return A pair containing the mean in 'first' and variance in 'second'
   */
  std::pair<double, double> unmaskedMeanAndVariance(const PufitGrid& grid);

  /// Select the grid with the highest masked sumEt
  PufitGridSet::GridDisplacement selectGrid(const PufitGridSet& grids);

  /**
   * @brief Perform the pile-up fit
   * @param pileupSum 2-vector sum over the pileup contributions
   * @param pileupCovariance Covariance matrix of the pileup sum
   * @param towerExpectations Expected values for each tower based on the pileup
   * energy density
   * @param towerVariances Expected values for the variances of each tower based
   * on variances of background towers
   * @param correctionDirections The directions of the objects to correct
   * @param The relative importance of the constraints.
   * @return A vector containing the magnitudes of the corrections
   *
   * This variant provides individual estimates of the expected energy in each
   * tower and the corresponding variances - these are for cases where the areas
   * of each object are different.
   */
  Eigen::VectorXd pufit(
      const Eigen::Vector2d& pileupSum,
      const Eigen::Matrix2d& pileupCovariance,
      const Eigen::VectorXd& towerExpectations,
      const Eigen::VectorXd& towerVariances,
      const Eigen::VectorXd& correctionDirections,
      double constraintImportance = 1);

  /**
   * @brief Perform the pile-up fit
   * @param pileupSum 2-vector sum over the pileup contributions
   * @param pileupCovariance Covariance matrix of the pileup sum
   * @param towerExpectations Expected values for each tower based on the pileup
   * energy density
   * @param towerVariances Expected values for the variances of each tower based
   * on variances of background towers
   * @param cosSin The cosines and sines of the objects to correct
   * @param The relative importance of the constraints.
   * @return A vector containing the magnitudes of the corrections
   *
   * This variant directly provides the cos-sin matrix for the correction,
   * potentially allowing many cos/sin calls to be avoided.
   * This variant provides individual estimates of the expected energy in each
   * tower and the corresponding variances - these are for cases where the areas
   * of each object are different.
   */
  Eigen::VectorXd pufit(
      const Eigen::Vector2d& pileupSum,
      const Eigen::Matrix2d& pileupCovariance,
      const Eigen::VectorXd& towerExpectations,
      const Eigen::VectorXd& towerVariances,
      const Eigen::Matrix<double, 2, Eigen::Dynamic>& cosSin,
      double constraintImportance = 1);

  /**
   * @brief Perform the pile-up fit
   * @param pileupSum 2-vector sum over the pileup contributions
   * @param pileupCovariance Covariance matrix of the pileup sum
   * @param towerExpectations Expected values for each tower based on the pileup
   * energy density
   * @param towerVariances Expected values for the variances of each tower based
   * on variances of background towers
   * @param toCorrect Kinematics of the objects to correct
   * @param The relative importance of the constraints.
   * @return The corrections to apply
   *
   * Note that this version still returns the corrections that should be
   * applied, not the corrected objects. The corrections are returned using the
   * negatives of the value calculated by the fit - i.e. you should add the
   * returned objects and let them act as negative energy objects (unless the
   * sign of the correction was negative in which case it will act as a positive
   * energy object).
   * This variant provides individual estimates of the expected energy in each
   * tower and the corresponding variances - these are for cases where the areas
   * of each object are different.
   */
  std::vector<SignedKinematics> pufit(
      const Eigen::Vector2d& pileupSum,
      const Eigen::Matrix2d& pileupCovariance,
      const std::vector<double>& towerExpectations,
      const std::vector<double>& towerVariances,
      const std::vector<SignedKinematics>& toCorrect,
      double constraintImportance = 1);

  /**
   * @brief Perform the pile-up fit
   * @param pileupSum 2-vector sum over the pileup contributions
   * @param pileupCovariance Covariance matrix of the pileup sum
   * @param towerMean Estimate of the mean pileup tower transverse energy
   * @param towerVariance Estimate of the variance of pileup tower transverse
   * energies
   * @param correctionDirections The directions of the objects to correct
   * @param The relative importance of the constraints.
   * @return A vector containing the magnitudes of the corrections
   */
  Eigen::VectorXd pufit(
      const Eigen::Vector2d& pileupSum,
      const Eigen::Matrix2d& pileupCovariance,
      double towerMean,
      double towerVariance,
      const Eigen::VectorXd& correctionDirections,
      double constraintImportance = 1);

  /**
   * @brief Perform the pile-up fit
   * @param pileupSum 2-vector sum over the pileup contributions
   * @param pileupCovariance Covariance matrix of the pileup sum
   * @param towerMean Estimate of the mean pileup tower transverse energy
   * @param towerVariance Estimate of the variance of pileup tower transverse
   * energies
   * @param cosSin The cosines and sines of the objects to correct
   * @param The relative importance of the constraints.
   * @return A vector containing the magnitudes of the corrections
   *
   * This variant directly provides the cos-sin matrix for the correction,
   * potentially allowing many cos/sin calls to be avoided.
   */
  Eigen::VectorXd pufit(
      const Eigen::Vector2d& pileupSum,
      const Eigen::Matrix2d& pileupCovariance,
      double towerMean,
      double towerVariance,
      const Eigen::Matrix<double, 2, Eigen::Dynamic>& cosSin,
      double constraintImportance = 1);

  /**
   * @brief Perform the pile-up fit
   * @param pileupSum 2-vector sum over the pileup contributions
   * @param pileupCovariance Covariance matrix of the pileup sum
   * @param towerMean Estimate of the mean pileup tower transverse energy
   * @param towerVariance Estimate of the variance of pileup tower transverse
   * energies
   * @param toCorrect Kinematics of the objects to correct
   * @param The relative importance of the constraints.
   * @return The corrections to apply
   *
   * Note that this version still returns the corrections that should be
   * applied, not the corrected objects. The corrections are returned using the
   * negatives of the value calculated by the fit - i.e. you should add the
   * returned objects and let them act as negative energy objects (unless the
   * sign of the correction was negative in which case it will act as a positive
   * energy object).
   */
  std::vector<SignedKinematics> pufit(
      const Eigen::Vector2d& pileupSum,
      const Eigen::Matrix2d& pileupCovariance,
      double towerMean,
      double towerVariance,
      const std::vector<SignedKinematics>& toCorrect,
      double constraintImportance = 1);
} } } //> end namespace HLT::MET::PufitUtils

#endif //> !TRIGEFMISSINGET_PUFITUTILS_H
