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
#include "TrigEFMissingET/PufitMultiGrid.h"
#include <vector>

namespace HLT
{
    namespace MET
    {
        namespace PufitUtils
        {
            /// Helper struct to hold the sum over pileup objects and its covariance
            struct CovarianceSum
            {
                /// Default constructor - zero initialize everything.
                CovarianceSum();
                /// Construct with an existing sum and matrix
                CovarianceSum(const Eigen::Vector2d &sum, const Eigen::Matrix2d &covariance);
                /**
                 * @brief Add a new contribution to the sum
                 * @brief kin The kinematics of the contribution
                 * @brief sigma The resolution of the contribution
                 */
                CovarianceSum &add(const SignedKinematics &kin, double sigma);
                /// The sum
                Eigen::Vector2d sum;
                /// The covariance matrix
                Eigen::Matrix2d covariance;
            };

            /**
             * @brief Calculate the trimmed mean and variance for a vector of tower sumEts
             * @param sorted The sumEts for all towers, sorted in ascending order
             * @param trimFraction The fraction of towers to use in the calculation
             * @param[out] mean The calculated mean
             * @param[out] variance The calculated variance
             */
            void trimmedMeanAndVariance(
                const std::vector<double> &sorted,
                double trimFraction,
                double &mean,
                double &variance);

            /**
             * @brief Calculate the trimmed mean and variance for a vector of tower sumEts
             * @param grid The grid to calculate
             * @param trimFraction The fraction of towers to use in the calculation
             * @param[out] mean The calculated mean
             * @param[out] variance The calculated variance
             */
            void trimmedMeanAndVariance(
                const PufitGrid &grid,
                double trimFraction,
                double &mean,
                double &variance);

            /**
             * @brief Calculate the trimmed mean and variance for a vector of tower sumEts
             * @param grid The grid to calculate
             * @param type The int mask of subgrid types
             * @param trimFraction The fraction of twoers to use in the calculation
             * @param[out] mean The calculated mean
             * @param[out] variance The calculated variance
             */
            template <std::size_t N>
            void trimmedMeanAndVariance(
                const PufitMultiGrid<N> &grid,
                std::size_t type,
                double trimFraction,
                double &mean,
                double &variance);

            /**
             * @brief Calculate the mean and variance of unmasked towers
             * @param grid The grid to calculate
             * @param[out] mean The calculated mean
             * @param[out] variance The calculated variance
             */
            void unmaskedMeanAndVariance(
                const PufitGrid &grid, double &mean, double &variance);

            /**
             * @brief Calculate the mean and variance of unmasked towers
             * @param grid The grid to calculate
             * @param type The int mask of subgrid types
             * @param[out] mean The calculated mean
             * @param[out] variance The calculated variance
             */
            template <std::size_t N>
            void unmaskedMeanAndVariance(
                const PufitMultiGrid<N> &grid, int type, double &mean, double &variance);

            /// Select the grid with the highest masked sumEt
            GridDisplacement selectGrid(const PufitGridSet &grids);

            /// Select the grid with the highest masked sumEt
            template <typename Grid>
            GridDisplacement selectGrid(
                const PufitMultiGridSet<Grid> &grids,
                std::size_t type);

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
                const Eigen::Vector2d &pileupSum,
                const Eigen::Matrix2d &pileupCovariance,
                const Eigen::VectorXd &towerExpectations,
                const Eigen::VectorXd &towerVariances,
                const Eigen::VectorXd &correctionDirections,
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
                const Eigen::Vector2d &pileupSum,
                const Eigen::Matrix2d &pileupCovariance,
                const Eigen::VectorXd &towerExpectations,
                const Eigen::VectorXd &towerVariances,
                const Eigen::Matrix<double, 2, Eigen::Dynamic> &cosSin,
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
                const Eigen::Vector2d &pileupSum,
                const Eigen::Matrix2d &pileupCovariance,
                const std::vector<double> &towerExpectations,
                const std::vector<double> &towerVariances,
                const std::vector<SignedKinematics> &toCorrect,
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
                const Eigen::Vector2d &pileupSum,
                const Eigen::Matrix2d &pileupCovariance,
                double towerMean,
                double towerVariance,
                const Eigen::VectorXd &correctionDirections,
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
                const Eigen::Vector2d &pileupSum,
                const Eigen::Matrix2d &pileupCovariance,
                double towerMean,
                double towerVariance,
                const Eigen::Matrix<double, 2, Eigen::Dynamic> &cosSin,
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
                const Eigen::Vector2d &pileupSum,
                const Eigen::Matrix2d &pileupCovariance,
                double towerMean,
                double towerVariance,
                const std::vector<SignedKinematics> &toCorrect,
                double constraintImportance = 1);
        } // namespace PufitUtils
    }     // namespace MET
} // namespace HLT

#include "TrigEFMissingET/PufitUtils.icc"

#endif //> !TRIGEFMISSINGET_PUFITUTILS_H
