/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigEFMissingET/PufitUtils.h"
#include <algorithm>
#include <numeric>

namespace HLT
{
  namespace MET
  {
    namespace PufitUtils
    {

      CovarianceSum::CovarianceSum() : sum(Eigen::Vector2d::Zero()),
                                       covariance(Eigen::Matrix2d::Zero())
      {
      }

      CovarianceSum::CovarianceSum(
          const Eigen::Vector2d &sum,
          const Eigen::Matrix2d &covariance) : sum(sum), covariance(covariance) {}

      CovarianceSum &CovarianceSum::add(
          const SignedKinematics &kin,
          double sigma)
      {
        Eigen::Vector2d cosSin(kin.cosPhi(), kin.sinPhi());
        sum += Eigen::Vector2d(kin.px(), kin.py());
        covariance += sigma * cosSin * cosSin.transpose();
        return *this;
      }

      void trimmedMeanAndVariance(
          const std::vector<double> &sorted,
          double trimFraction,
          double &mean,
          double &variance)
      {
        // The number to trim from each side
        std::size_t nTrim = sorted.size() * (1 - trimFraction) / 2;

        mean = std::accumulate(sorted.begin() + nTrim, sorted.end() - nTrim, 0.);
        mean /= (sorted.size() - 2 * nTrim);

        // Now get the variance
        variance = 0;
        for (std::size_t ii = 0; ii < sorted.size() - nTrim; ++ii)
        {
          double val = sorted.at(ii) - mean;
          variance += val * val * (ii < nTrim ? 2 : 1);
        }
        variance /= sorted.size();
      }

      void trimmedMeanAndVariance(
          const PufitGrid &grid,
          double trimFraction,
          double &mean,
          double &variance)
      {
        std::vector<double> sorted;
        sorted.reserve(grid.nTowers());
        for (const PufitGrid::Tower &tower : grid)
          sorted.insert(
              std::lower_bound(sorted.begin(), sorted.end(), tower.sumEt()),
              tower.sumEt());
        trimmedMeanAndVariance(sorted, trimFraction, mean, variance);
      }

      void unmaskedMeanAndVariance(
          const PufitGrid &grid,
          double &mean,
          double &variance)
      {
        double sum = 0;
        double squaredSum = 0;
        std::size_t n = 0;
        for (const PufitGrid::Tower &tower : grid)
        {
          if (tower.masked())
            continue;
          ++n;
          sum += tower.sumEt();
          squaredSum += tower.sumEt() * tower.sumEt();
        }
        mean = sum / n;
        // Note that this could result in catastrophic cancellation in some cases.
        // However the amount of precision present in a double is around 10^15 so we
        // can afford to lose some significant figures
        variance = squaredSum / n - mean * mean;
      }

      GridDisplacement selectGrid(const PufitGridSet &grids)
      {
        GridDisplacement maximum = NoDisplacement;
        double maxSum = 0;
        for (std::size_t d = 0; d < 4; ++d)
        {
          double sum = 0;
          for (const PufitGrid::Tower &tower : grids[GridDisplacement(d)])
            if (tower.masked())
              sum += tower.sumEt();
          if (sum > maxSum)
          {
            maximum = GridDisplacement(d);
            maxSum = sum;
          }
        }
        return maximum;
      }

      Eigen::VectorXd pufit(
          const Eigen::Vector2d &pileupSum,
          const Eigen::Matrix2d &pileupCovariance,
          const Eigen::VectorXd &towerExpectations,
          const Eigen::VectorXd &towerVariances,
          const Eigen::VectorXd &correctionDirections,
          double constraintImportance)
      {
        // Keep track of the number of corrections to derive
        std::size_t nCorr = correctionDirections.size();
        if (nCorr == 0)
          // No need to do anything to derive corrections to nothing
          return {};
        // Turn the directions into cos/sin pairs
        Eigen::Matrix<double, 2, Eigen::Dynamic> cosSin(2, nCorr);
        cosSin.row(0) = correctionDirections.array().cos();
        cosSin.row(1) = correctionDirections.array().sin();
        return pufit(
            pileupSum,
            pileupCovariance,
            towerExpectations,
            towerVariances,
            cosSin,
            constraintImportance);
      }

      Eigen::VectorXd pufit(
          const Eigen::Vector2d &pileupSum,
          const Eigen::Matrix2d &pileupCovariance,
          double towerMean,
          double towerVariance,
          const Eigen::VectorXd &correctionDirections,
          double constraintImportance)
      {
        std::size_t nCorr = correctionDirections.size();
        return pufit(
            pileupSum,
            pileupCovariance,
            Eigen::VectorXd::Constant(nCorr, towerMean),
            Eigen::VectorXd::Constant(nCorr, towerVariance),
            correctionDirections,
            constraintImportance);
      }

      Eigen::VectorXd pufit(
          const Eigen::Vector2d &pileupSum,
          const Eigen::Matrix2d &pileupCovariance,
          const Eigen::VectorXd &towerExpectations,
          const Eigen::VectorXd &towerVariances,
          const Eigen::Matrix<double, 2, Eigen::Dynamic> &cosSin,
          double constraintImportance)
      {
        // Keep track of the number of corrections to derive
        std::size_t nCorr = cosSin.cols();
        if (nCorr == 0)
          // No need to do anything to derive corrections to nothing
          return {};
        // Eigen internally uses somewhat complicated 'expression templates' to hold
        // the mathematics of a matrix calculation without actually doing the loop.
        // This allows it to avoid doing more loops than it actually needs to. It
        // will do this by default unless you force it to evaluate an expression by
        // binding it to a vector matrix type. Therefore we use 'auto' to store the
        // individual expression pieces, rather than forcing an early evaluation.
        auto constants =
            // Constant part derived from the uniformity constraint
            towerExpectations.cwiseQuotient(towerVariances) -
            // Constant part derived from the pileup balance constraint
            constraintImportance * cosSin.transpose() * pileupCovariance.inverse() * pileupSum;
        Eigen::MatrixXd diagonal = towerVariances.cwiseInverse().asDiagonal();
        auto coeffMatrix =
            // Matrix part derived from the uniformity constraint
            diagonal +
            // Matrix part derived from the pileup balance constraint
            constraintImportance * (cosSin.transpose() * pileupCovariance.inverse() * cosSin);
        // Now return the actual corrections
        return coeffMatrix.inverse() * constants;
      }

      Eigen::VectorXd pufit(
          const Eigen::Vector2d &pileupSum,
          const Eigen::Matrix2d &pileupCovariance,
          double towerMean,
          double towerVariance,
          const Eigen::Matrix<double, 2, Eigen::Dynamic> &cosSin,
          double constraintImportance)
      {
        std::size_t nCorr = cosSin.cols();
        return pufit(
            pileupSum,
            pileupCovariance,
            Eigen::VectorXd::Constant(nCorr, towerMean),
            Eigen::VectorXd::Constant(nCorr, towerVariance),
            cosSin,
            constraintImportance);
      }

      std::vector<SignedKinematics> pufit(
          const Eigen::Vector2d &pileupSum,
          const Eigen::Matrix2d &pileupCovariance,
          const std::vector<double> &towerExpectations,
          const std::vector<double> &towerVariances,
          const std::vector<SignedKinematics> &toCorrect,
          double constraintImportance)
      {
        // Keep track of the number of corrections to derive
        std::size_t nCorr = toCorrect.size();
        if (nCorr == 0)
          // No need to do anything to derive corrections to nothing
          return {};

        // Get the cos-sin matrix
        Eigen::Matrix<double, 2, Eigen::Dynamic> cosSin(2, nCorr);
        Eigen::VectorXd vecTowerExpectations(nCorr);
        Eigen::VectorXd vecTowerVariances(nCorr);
        for (std::size_t ii = 0; ii < nCorr; ++ii)
        {
          cosSin(0, ii) = toCorrect.at(ii).cosPhi();
          cosSin(1, ii) = toCorrect.at(ii).sinPhi();
          vecTowerExpectations(ii) = towerExpectations.at(ii);
          vecTowerVariances(ii) = towerVariances.at(ii);
        }
        Eigen::VectorXd corrections = pufit(
            pileupSum,
            pileupCovariance,
            vecTowerExpectations,
            vecTowerVariances,
            cosSin,
            constraintImportance);
        // Now convert these into kinematics
        std::vector<SignedKinematics> kinCorrections;
        kinCorrections.reserve(nCorr);
        for (std::size_t ii = 0; ii < nCorr; ++ii)
          kinCorrections.push_back(SignedKinematics::fromEtEtaPhi(
              -corrections.coeff(ii),
              toCorrect.at(ii).eta(),
              toCorrect.at(ii).phi()));
        return kinCorrections;
      }
      std::vector<SignedKinematics> pufit(
          const Eigen::Vector2d &pileupSum,
          const Eigen::Matrix2d &pileupCovariance,
          double towerMean,
          double towerVariance,
          const std::vector<SignedKinematics> &toCorrect,
          double constraintImportance)
      {
        // Keep track of the number of corrections to derive
        std::size_t nCorr = toCorrect.size();
        if (nCorr == 0)
          // No need to do anything to derive corrections to nothing
          return {};

        return pufit(
            pileupSum,
            pileupCovariance,
            std::vector<double>(nCorr, towerMean),
            std::vector<double>(nCorr, towerVariance),
            toCorrect,
            constraintImportance);
      }
    } // namespace PufitUtils
  }   // namespace MET
} // namespace HLT
