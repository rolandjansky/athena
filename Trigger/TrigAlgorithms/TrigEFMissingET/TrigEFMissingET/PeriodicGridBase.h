/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGEFMISSINGET_PERIODICGRIDBASE_H
#define TRIGEFMISSINGET_PERIODICGRIDBASE_H

#include <cstddef>
#include <utility>

/**
 * @file TrigEFMissingET/PeriodicGridBase.h
 * @brief Provide a base class for the grids used in some pufit algorithms
 * @author Jon bur (jon.burr@cern.ch)
 */

namespace HLT
{
  namespace MET
  {
    /// Enum to describe the positioning of the grid
    enum GridDisplacement
    {
      /// The grid is not shifted
      NoDisplacement = 0,
      /// The grid is shifted by half a tower width in eta
      EtaDisplaced = 1,
      /// The grid is shifted by half a tower width in phi
      PhiDisplaced = 2,
      /// The grid is shifted by half a tower width in both eta and phi
      EtaPhiDisplaced = 3
    }; //> end enum GridDisplacement

    /**
   * @brief Parameters describing a grid
   * 
   * How the grid divides up the detector is described mainly by the extent in
   * eta (phi is by definition 2*pi) and the number of divisions in eta and phi.
   * However, the grids must also support being displaced slightly in eta and
   * phi, so these members are also required.
   */
    struct GridParameters
    {
      /// The maximum |eta| value
      double maxEta{0.};
      /// The number of divisions along the eta axis
      std::size_t nEtaTowers{0};
      /// The number of divisions along the phi axis
      std::size_t nPhiTowers{0};
      /// Whether the grid is displaced in eta
      bool displaceEta{false};
      /// Whether the grid is displaced in phi
      bool displacePhi{false};
      /// Check inequality with other parameter sets
      bool operator!=(const GridParameters &other) const;
      bool operator==(const GridParameters &other) const;
      /// The @see GridDisplacement enum
      GridDisplacement displacement() const;
    }; //> end struct GridParameters

    /**
   * @brief Base class for grids used in some of the pufit algorithms
   * 
   * Many versions of the pufit technique require breaking the calorimeter into
   * a grid in order to measure the variance of energy deposits across the
   * detector and to identify areas which have a significantly high energy.
   * 
   * These grids must also support being displaced slightly in eta and phi to
   * ensure that high energy deposits are not artificially divided between two
   * towers.
   * 
   * The grids are also periodic in phi - i.e. the tower located at phi is the
   * same one located at phi + 2pi.
   */
    class PeriodicGridBase
    {
    public:
      /**
     * @brief Base class for towers belonging to the grids
     */
      class Tower
      {
      public:
        Tower(std::size_t index);
        /// The grid which owns this tower
        virtual const PeriodicGridBase *grid() const = 0;

        /// The central phi coordinate of this tower
        double towerPhi() const;

        /// The central eta coordinate of this tower
        double towerEta() const;

        /// The global index of this tower
        std::size_t index() const;

        /// The eta index of this tower
        std::size_t etaIndex() const;

        /// The phi index of this tower
        std::size_t phiIndex() const;

        /// The eta/phi indices of the tower together
        std::pair<std::size_t, std::size_t> etaPhiIndex() const;

      private:
        std::size_t m_index;
      }; //> end class Tower

      /// Construct the grid from its parameters
      PeriodicGridBase(const GridParameters &parameters);

      /// Construct the grid from its parameters provided directly
      PeriodicGridBase(
          double maxEta,
          std::size_t nEtaTowers,
          std::size_t nPhiTowers,
          bool displaceEta = false,
          bool displacePhi = false);

      /**
     * @brief Get the index for the given eta, phi values.
     * @param eta The eta value
     * @param phi The phi value
     * @param[out] outOfRange Set to true if outside of the eta range
     *
     * If it's out of range the returned index will be nTowers
     */
      std::size_t getIndex(
          double eta,
          double phi,
          bool &outOfRange) const;

      /**
     * @brief Get the eta index for the given value
     * @param eta The eta value
     * @param[out] outOfRange Set to true if outside of the eta range
     *
     * If it's out of range the returned index will be nEta
     */
      std::size_t getEtaIndex(double eta, bool &outOfRange) const;

      /// Get the phi index for the given value
      std::size_t getPhiIndex(double phi) const;

      /// Convert eta and phi to a global index
      std::size_t globalIndex(std::size_t iEta, std::size_t iPhi) const;

      /// Convert a global index to an eta/phi index pair
      std::pair<std::size_t, std::size_t> etaPhiIndex(std::size_t index) const;

      /// Central eta coordinate of the given eta index
      double centralEta(std::size_t iEta) const;
      /// Central phi coordinate of the given phi
      double centralPhi(std::size_t iPhi) const;

      /// The grid parameters
      const GridParameters &parameters() const;

      /// The maximum eta range for the grid
      double maxEta() const;

      /// The number of eta bins
      std::size_t nEtaTowers() const;

      /// The number of phi bins
      std::size_t nPhiTowers() const;

      /// The number of bins
      std::size_t nTowers() const;

      /// Whether or not this is displaced in eta
      bool displaceEta() const;

      /// Whether or not this is displaced in phi
      bool displacePhi() const;

      /// The grid displacement
      GridDisplacement displacement() const;

      /// The bin width in eta
      double etaWidth() const;

      /// The bin width in phi
      double phiWidth() const;

    private:
      /// The grid's parameters
      const GridParameters m_params;
    }; //> end class PeriodicGridBase
  }    // namespace MET
} // namespace HLT

#endif //> !PUFITUTILS_PERIODICGRIDBASE_H