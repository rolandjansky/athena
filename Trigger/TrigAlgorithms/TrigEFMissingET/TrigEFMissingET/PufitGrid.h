/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   PufitGrid
 *
 * @brief Grid to use in the pufit calculation
 * @author Bob Kowalewski, Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_PUFITGRID_H
#define TRIGEFMISSINGET_PUFITGRID_H

#include <vector>
#include <utility>
#include <array>
#include "TrigEFMissingET/SignedKinematics.h"

namespace HLT { namespace MET {
  // Forward declare
  class METComponent;

  /**
   * @brief Bins energy deposits into a grid.
   *
   * The grid can be displaced in eta and/or phi. In this case the grid will
   * be moved by half a bin in the relevant directions. For these purposes
   * the grid will be treated as periodic in eta, even though the
   * calorimeter isn't really.
   */
  class PufitGrid {
    public:
      /**
       * @brief Describes a single element of the grid.
       *
       * There are two sets of directions here - the 'kinematic' directions
       * which result from the sums of objects going into the towers and the
       * tower coordinates themselves.
       */
      class Tower {
        public:
          /**
           * @brief Create a tower with its parent
           * @param parent The parent grid of this tower
           * @param index The index of this tower in its parent's grid
           */
          Tower(const PufitGrid* parent, std::size_t index);

          Tower(const Tower&) = default;

          /**
           * @brief Copy assignment operator
           *
           * This will not copy the other tower's parent. Each tower's parent is
           * fixed from construction! This only copies the energy and masking
           * information over.
           */
          Tower& operator=(const Tower& other);
          
          /// The x-component of this tower's energy
          double ex() const;

          /// The y-component of this tower's energy
          double ey() const;

          /// The z-component of this tower's energy
          double ez() const;

          /// The total sumEt in this tower
          double sumEt() const;

          /// The total sumE in this tower
          double sumE() const;

          /// This tower's kinematic phi
          double phi() const;

          /// This tower's kinematic eta
          double eta() const;

          /// Whether or not this tower was masked
          bool masked() const;

          /// Set the mask on this tower
          void mask(bool value=true);

          /// The central phi of this tower
          double towerPhi() const;

          /// The central eta of this tower
          double towerEta() const;

          /// The global index of this tower
          std::size_t index() const;

          /// The parent grid of this tower
          const PufitGrid* grid() const;

          /// The eta index of this tower
          std::size_t etaIndex() const;

          /// The phi index of this tower
          std::size_t phiIndex() const;

          /// The eta-phi indices of this tower
          std::pair<std::size_t, std::size_t> etaPhiIndex() const;

          /// Build a kinematics object from this tower
          SignedKinematics kinematics() const;
          /// Conversion operator
          operator SignedKinematics() const;

          /**
           * @brief Add a signed object to this tower.
           *
           * No check is done to make sure that this is the right bin.
           * The *momentum* of the kinematics will be used. If you do not want
           * this (i.e. you want mass to be ignored) you should construct the
           * kinematics without mass.
           */
          Tower& operator+=(const SignedKinematics& kin);
          /**
           * @brief Remove the energy of a signed object from this tower.
           *
           * No check is done to make sure that this is the right bin.
           * The *momentum* of the kinematics will be used. If you do not want
           * this (i.e. you want mass to be ignored) you should construct the
           * kinematics without mass.
           */
          Tower& operator-=(const SignedKinematics& kin);

          /**
           * @brief Add another tower's energies into this one.
           */
          Tower& operator+=(const Tower& other);
          /**
           * @brief Subtract another tower's energies from this one.
           */
          Tower& operator-=(const Tower& other);
        private:
          /// The parent grid
          const PufitGrid* const m_parent;

          /// The index in the parent
          const std::size_t m_index;

          SignedKinematics m_kin;
          /// The summed et
          double m_sumEt{0.};
          /// The summed energy
          double m_sumE{0.};
          /// The mask value
          bool m_mask{false};
      }; //> end class Tower

      /// The grid parameters
      struct GridParameters {
        double maxEta;
        std::size_t nEtaTowers;
        std::size_t nPhiTowers;
        bool displaceEta{false};
        bool displacePhi{false};
        bool operator!=(const GridParameters& other) const;
        bool operator==(const GridParameters& other) const;
      };

      /**
       * @brief Create a new tower grid
       * @param maxEta The maximum eta range for the grid
       * @param nEtaTowers The number of eta towers
       * @param nPhiTowers The number of phi towers
       * @param displaceEta Whether to displace eta
       * @param displacePhi Whether to displace phi
       */
      PufitGrid(
          double maxEta,
          std::size_t nEtaTowers,
          std::size_t nPhiTowers,
          bool displaceEta = false,
          bool displacePhi = false);

      /// Construct a grid from the provided parameters
      PufitGrid(const GridParameters& parameters);

      /// Copy constructor
      PufitGrid(const PufitGrid& other);

      /**
       * @brief Assignment operator
       * @param other The grid whose towers to take
       * @except std::invalid_argument If other's parameters don't match ours.
       *
       * Take the tower energies/masking from the other grid.
       * This is only allowed between two grids with matching parameters.
       */
      PufitGrid& operator=(const PufitGrid& other);

      /// Reset the internal storage
      void reset();

      /**
       * @brief Add a signed object to this grid.
       *
       * The object will be assigned to the relevant bin. It will not be added
       * if it falls outside of the eta range.
       * The *momentum* of the kinematics will be used. If you do not want
       * this (i.e. you want mass to be ignored) you should construct the
       * kinematics without mass.
       */
      PufitGrid& operator+=(const SignedKinematics& kin);
      /**
       * @brief Remove the energy of a signed object from this grid.
       *
       * The object will be assigned to the relevant bin. It will not be added
       * if it falls outside of the eta range.
       * The *momentum* of the kinematics will be used. If you do not want
       * this (i.e. you want mass to be ignored) you should construct the
       * kinematics without mass.
       */
      PufitGrid& operator-=(const SignedKinematics& kin);

      /**
       * @brief Get the tower index for the given eta, phi values.
       * @param eta The eta value
       * @param phi The phi value
       * @param[out] outOfRange Set to true if outside of the eta range
       *
       * If it's out of range the returned index will be nTowers
       */
      std::size_t getIndex(
          double eta,
          double phi,
          bool& outOfRange) const;

      /**
       * @brief Get the eta tower index for the given value
       * @param eta The eta value
       * @param[out] outOfRange Set to true if outside of the eta range
       *
       * If it's out of range the returned index will be nEta
       */
      std::size_t getEtaIndex(double eta, bool& outOfRange) const;

      /// Get the phi tower index for the given value
      std::size_t getPhiIndex(double phi) const;

      /// Access stored value by eta/phi index
      Tower& operator[](const std::pair<std::size_t, std::size_t>& indices);
      /// Access stored value by eta/phi index
      const Tower& operator[](
          const std::pair<std::size_t, std::size_t>& indices) const;

      /// Access stored value by global index number
      Tower& operator[](std::size_t index);
      /// Access stored value by global index number
      const Tower& operator[](std::size_t index) const;

      /// Access by iterator
      std::vector<Tower>::iterator begin();
      /// Access by iterator
      std::vector<Tower>::const_iterator begin() const;
      /// Iterator end point
      std::vector<Tower>::iterator end();
      /// Iterator end point
      std::vector<Tower>::const_iterator end() const;

      /// Convert eta and phi index to global index
      std::size_t globalIndex(std::size_t iEta, std::size_t iPhi) const;
      /// Convert global index to eta and phi indices
      std::pair<std::size_t, std::size_t> etaPhiIndex(std::size_t index) const;

      /// Central eta coordinate of the given eta index
      double centralEta(std::size_t iEta) const;
      /// Central phi coordinate of the given phi tower
      double centralPhi(std::size_t iPhi) const;

      /// The grid parameters
      const GridParameters& parameters() const;

      /// The maximum eta range for the grid
      double maxEta() const;

      /// The number of eta towers
      std::size_t nEtaTowers() const;

      /// The number of phi towers
      std::size_t nPhiTowers() const;

      /// The number of towers
      std::size_t nTowers() const;

      /// Whether or not this is displaced in eta
      bool displaceEta() const;

      /// Whether or not this is displaced in phi
      bool displacePhi() const;

      /// The bin width in eta
      double etaWidth() const;

      /// The bin width in phi
      double phiWidth() const;

      /// Helper enum to describe how to sum over towers
      enum class SumStrategy {
        All, //> Sum all towers, regardless of masks
        Masked, //> Only sum masked towers
        Unmasked //> Only sum unmasked towers
      };
      METComponent sum(SumStrategy strategy=SumStrategy::All) const;

      // All of these fail if the grid parameters do not match!
      /// Add a whole grid into this.
      PufitGrid& operator+=(const PufitGrid& other);
      /// Subtract a whole grid from this.
      PufitGrid& operator-=(const PufitGrid& other);
    private:
      const GridParameters m_params;
      double m_etaWidth;
      double m_phiWidth;
      std::vector<Tower> m_towers;
  }; //> end class PufitGrid

  /// Helper struct to contain a full set of grids
  struct PufitGridSet {
    enum GridDisplacement {
      NoDisplacement = 0,
      EtaDisplaced = 1,
      PhiDisplaced = 2,
      EtaPhiDisplaced = 3
    };
    PufitGridSet(double maxEta, std::size_t nEta, std::size_t nPhi);
    std::array<PufitGrid, 4> grids;
    /// Add kinematics
    PufitGridSet& operator+=(const SignedKinematics& kin);
    /// Subtract kinematics
    PufitGridSet& operator-=(const SignedKinematics& kin);
    /// Select a grid
    PufitGrid& operator[](GridDisplacement displacement);
    const PufitGrid& operator[](GridDisplacement displacement) const;
  };

  /// Elementwise sum
  PufitGrid operator+(const PufitGrid& lhs, const PufitGrid& rhs);
  /// Elementwise subtraction
  PufitGrid operator-(const PufitGrid& lhs, const PufitGrid& rhs);
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_PUFITGRID_H
