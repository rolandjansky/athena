/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGEFMISSINGET_PUFITMULTIGRID_H
#define TRIGEFMISSINGET_PUFITMULTIGRID_H

#include "TrigEFMissingET/PeriodicGridBase.h"
#include "TrigEFMissingET/PufitGrid.h"
#include <limits>
#include <type_traits>

namespace HLT
{
  namespace MET
  {
    /// Compile time check if a number is a power of 2
    constexpr bool isPow2(std::size_t i)
    {
      // An int is a power of 2 if it only contains one 'on' bit
      // We check for this by shifting an int until its first bit is 1. Then it
      // is a power of two if that is the only bit set.
      // We also need a check if this is 0 otherwise the recursion will never
      // end
      if (i == 0)
        // 0 is *not* a power of 2
        return false;
      else if (i == 1)
        // 1 is a power of 2
        return true;
      else if (i & 1)
        // If we're not 1, but the '1' bit is set then this can't be a power of 2
        return false;
      else
        // shifting to the right esse
        return isPow2(i >> 1);
    }

    /**
     * @brief Compile time calculation of the log base 2 of an integer
     * 
     * Returns the floor of the log, so intLog2(9) = 3. Log base 2 of 0 is
     * undefined and here returns the maximum value for std::size_t.
     */
    constexpr uint16_t intLog2(std::size_t i, uint16_t tmp = 0)
    {
      if (i == 0)
        return UINT16_MAX;
      else if (i == 1)
        return tmp;
      else
        return intLog2(i >> 1, tmp + 1);
    }

    /**
     * @brief Multiple grids combined into one
     * 
     * This class allows representing N different categories in a single grid.
     */
    template <std::size_t N>
    class PufitMultiGrid : public PeriodicGridBase
    {
      static_assert(N > 0, "N must be greater than 0");
      // The C++ standard only guarantees that std::size_t has a bit width of
      // at least 16
      static_assert(N < 17, "N must be no greater than 16");

    public:
      /// Maximum value representable by N bits
      constexpr static std::size_t All = (1 << N) - 1;
      /// The number of separate categories in the grid
      constexpr static std::size_t NCategories = N;

      /**
       * @brief Tower in the multi-grid
       * 
       * Each tower is built from N 'normal' PufitGrid::Towers. Kinematic
       * quantities can be read from the tower either from any of the
       * 'subtowers' or from a sum over them, passing in a std::size_t as a bitmask.
       */
      class Tower : public PeriodicGridBase::Tower
      {
        friend class PufitMulitGrid;

      public:
        /**
         * @brief Create a tower with its parent grid
         * @param parent The parent grid of this tower
         * @param index The index of this tower in its parent's grid
         */
        Tower(PufitMultiGrid *parent, std::size_t index);

        /**
         * @brief Copy assignment operator
         *
         * This will not copy the other tower's parent. Each tower's parent is
         * fixed from construction! This only copies the energy and masking
         * information over.
         */
        Tower &operator=(const Tower &other);

        /// The x-component of this tower's energy
        double ex(std::size_t type = All) const;

        /// The y-component of this tower's energy
        double ey(std::size_t type = All) const;

        /// The z-component of this tower's energy
        double ez(std::size_t type = All) const;

        /// The total sumEt in this tower
        double sumEt(std::size_t type = All) const;

        /// The total sumE in this tower
        double sumE(std::size_t type = All) const;

        /// This tower's kinematic phi
        double phi(std::size_t type = All) const;

        /// This tower's kinematic eta
        double eta(std::size_t type = All) const;

        /// Whether or not this tower was masked
        bool masked() const;

        /// Set the mask on this tower
        void mask(bool value = true);

        /// The parent grid of this tower
        const PufitMultiGrid *grid() const override;

        /// Build a kinematics object from this tower
        SignedKinematics kinematics(std::size_t type = All) const;

        /// Conversion operator, returns the value for the sum of all the
        /// grids
        operator SignedKinematics() const;

        /**
         * @brief Add another tower's energies into this one.
         */
        Tower &operator+=(const Tower &other);
        /**
         * @brief Subtract another tower's energies from this one.
         */
        Tower &operator-=(const Tower &other);

        /**
         * @brief Get one of the underlying towers
         *
         * This is only defined where I is a power of 2
         */
        template <std::size_t I,
                  typename = typename std::enable_if<isPow2(I)>::type,
                  typename = typename std::enable_if<I <= All>::type>
        PufitGrid::Tower &get()
        {
          return subTower(intLog2(I));
        }

        /**
         * @brief Get one of the underlying towers
         *
         * This is only defined where I is a power of 2
         */
        template <std::size_t I,
                  typename = typename std::enable_if<isPow2(I)>::type,
                  typename = typename std::enable_if<I <= All>::type>
        const PufitGrid::Tower &get() const
        {
          return subTower(intLog2(I));
        }

      private:
        /// The parent grid
        PufitMultiGrid *const m_parent;

        /// The mask value
        bool m_mask{false};

        /// Get a subtower by index
        PufitGrid::Tower &subTower(std::size_t ii)
        {
          return m_parent->m_grids[ii][index()];
        }
        /// Get a subtower by index
        const PufitGrid::Tower &subTower(std::size_t ii) const
        {
          return m_parent->m_grids[ii][index()];
        }

        /// Apply a function to all sub towers
        void applyToAll(PufitGrid::Tower &(PufitGrid::Tower::*f)(const PufitGrid::Tower &), const PufitMultiGrid::Tower &other)
        {
          for (std::size_t ii = 0; ii < N; ++ii)
            (subTower(ii).*f)(other.subTower(ii));
        }

        /// Sum over the results of all sub towers whose indices match the
        /// 'type' mask
        template <typename T>
        typename std::decay<T>::type sumOver(int type, T (PufitGrid::Tower::*f)() const) const
        {
          typename std::decay<T>::type val{};
          for (std::size_t ii = 0; ii < N; ++ii)
            if (1 << ii & type)
              val += (subTower(ii).*f)();
          return val;
        }

      }; //> end class Tower

      /**
       * @brief Create a new tower grid
       * @param maxEta The maximum eta range for the grid
       * @param nEtaTowers The number of eta towers
       * @param nPhiTowers The number of phi towers
       * @param displaceEta Whether to displace eta
       * @param displacePhi Whether to displace phi
       */
      PufitMultiGrid(
          double maxEta,
          std::size_t nEtaTowers,
          std::size_t nPhiTowers,
          bool displaceEta = false,
          bool displacePhi = false);

      /// Construct a grid from the provided parameters
      PufitMultiGrid(const GridParameters &parameters);

      /// Copy constructor
      PufitMultiGrid(const PufitMultiGrid &other);

      /**
       * @brief Assignment operator
       * @param other The grid whose towers to take
       *
       * Take the tower energies/masking from the other grid.
       */
      PufitMultiGrid &operator=(const PufitMultiGrid &other);

      /// Reset the internal storage
      void reset();

      /// Access stored value by eta/phi index (access is bounds checked)
      Tower &operator[](const std::pair<std::size_t, std::size_t> &indices);
      /// Access stored value by eta/phi index (access is bounds checked)
      const Tower &operator[](const std::pair<std::size_t, std::size_t> &indices) const;

      /// Access stored value by global index number (access is bounds checked)
      Tower &operator[](std::size_t index);
      /// Access stored value by global index number (access is bounds checked)
      const Tower &operator[](std::size_t index) const;

      /// Access by iterator
      typename std::vector<Tower>::iterator begin();
      /// Access by iterator
      typename std::vector<Tower>::const_iterator begin() const;
      /// Iterator end point
      typename std::vector<Tower>::iterator end();
      /// Iterator end point
      typename std::vector<Tower>::const_iterator end() const;

      // All of these fail if the grid parameters do not match!
      /// Add a whole grid into this.
      PufitMultiGrid &operator+=(const PufitMultiGrid &other);
      /// Subtract a whole grid from this.
      PufitMultiGrid &operator-=(const PufitMultiGrid &other);

      /**
       * @brief Get one of the underlying grids
       *
       * This is only defined where I is a power of 2
       */
      template <std::size_t I,
                typename = typename std::enable_if<isPow2(I)>::type,
                typename = typename std::enable_if<I <= All>::type>
      PufitGrid &get()
      {
        return m_grids[intLog2(I)];
      }

      /**
       * @brief Get one of the underlying grids
       *
       * This is only defined where I is a power of 2
       */
      template <std::size_t I,
                typename = typename std::enable_if<isPow2(I)>::type,
                typename = typename std::enable_if<I <= All>::type>
      const PufitGrid &get() const
      {
        return m_grids[intLog2(I)];
      }

      /**
       * @brief Get one of the underlying grids
       *
       * This version is allowed to return a grid where idx is not a power of
       * 2, in which case the grid will be constructed on the fly from the sum
       * of its bitmask
       */
      PufitGrid get(std::size_t type) const;

    private:
      std::array<PufitGrid, N> m_grids;
      std::vector<Tower> m_towers;
    }; //> end class PufitMultiGrid

    /// Helper struct to forward the SignedKinematics operators nicely
    template <typename Grid>
    struct PufitMultiGridSet
    {
      PufitMultiGridSet(double maxEta, std::size_t nEta, std::size_t nPhi);
      std::array<Grid, 4> grids;

      /// Select a grid
      Grid &operator[](GridDisplacement displacement)
      {
        return grids[displacement];
      }

      /// Select a grid (const)
      const Grid &operator[](GridDisplacement displacement) const
      {
        return grids[displacement];
      }

      /// Helper struct to forward the SignedKinematics operators nicely
      template <std::size_t I>
      struct Element
      {
        Element(PufitMultiGridSet &parent);
        PufitMultiGridSet &parent;
        Element &operator+=(const SignedKinematics &kin);
        Element &operator-=(const SignedKinematics &kin);
      }; //> end struct Element<I>

      template <std::size_t I,
                typename = typename std::enable_if<isPow2(I)>::type,
                typename = typename std::enable_if<I <= Grid::All>::type>
      Element<I> get();

      PufitGridSet get(std::size_t type) const;
    }; //> end struct PufitMultiGridSet<Grid>
  }    // namespace MET
} // namespace HLT

#include "TrigEFMissingET/PufitMultiGrid.icc"

#endif //> !TRIGEFMISSINGET_PUFITMULTIGRID_H
