/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigEFMissingET/PufitGrid.h"
#include "TrigEFMissingET/METComponent.h"
#include <algorithm>
#include <cmath>

namespace HLT
{
  namespace MET
  {

    PufitGrid::Tower::Tower(const PufitGrid *parent, std::size_t index) : PeriodicGridBase::Tower(index),
                                                                          m_parent(parent)
    {
    }

    PufitGrid::Tower &PufitGrid::Tower::operator=(const Tower &other)
    {
      m_kin = other.kinematics();
      m_sumEt = other.sumEt();
      m_sumE = other.sumE();
      m_mask = other.masked();
      return *this;
    }

    double PufitGrid::Tower::ex() const { return m_kin.px(); }
    double PufitGrid::Tower::ey() const { return m_kin.py(); }
    double PufitGrid::Tower::ez() const { return m_kin.pz(); }
    double PufitGrid::Tower::sumEt() const { return m_sumEt; }
    double PufitGrid::Tower::sumE() const { return m_sumE; }
    double PufitGrid::Tower::phi() const { return kinematics().phi(); }
    double PufitGrid::Tower::eta() const { return kinematics().eta(); }
    bool PufitGrid::Tower::masked() const { return m_mask; }
    void PufitGrid::Tower::mask(bool value) { m_mask = value; }

    const PufitGrid *PufitGrid::Tower::grid() const { return m_parent; }

    SignedKinematics PufitGrid::Tower::kinematics() const
    {
      return m_kin;
    }
    PufitGrid::Tower::operator SignedKinematics() const { return kinematics(); }

    PufitGrid::Tower &PufitGrid::Tower::operator+=(const SignedKinematics &kin)
    {
      m_kin += kin;
      m_sumEt += kin.pt();
      m_sumE += kin.energy();
      return *this;
    }
    PufitGrid::Tower &PufitGrid::Tower::operator-=(const SignedKinematics &kin)
    {
      m_kin -= kin;
      m_sumEt -= kin.pt();
      m_sumE -= kin.energy();
      return *this;
    }
    PufitGrid::Tower &PufitGrid::Tower::operator+=(const Tower &other)
    {
      m_kin += other.kinematics();
      m_sumEt += other.sumEt();
      m_sumE += other.sumE();
      return *this;
    }
    PufitGrid::Tower &PufitGrid::Tower::operator-=(const Tower &other)
    {
      m_kin -= other.kinematics();
      m_sumEt -= other.sumEt();
      m_sumE -= other.sumE();
      return *this;
    }

    PufitGrid::PufitGrid(
        double maxEta,
        std::size_t nEtaTowers,
        std::size_t nPhiTowers,
        bool displaceEta,
        bool displacePhi)
        : PufitGrid(GridParameters{maxEta, nEtaTowers, nPhiTowers, displaceEta, displacePhi})
    {
    }

    PufitGrid::PufitGrid(const GridParameters &parameters)
        : PeriodicGridBase(parameters)
    {
      m_towers.reserve(nTowers());
      for (std::size_t index = 0; index < nTowers(); ++index)
        m_towers.emplace_back(this, index);
    }

    PufitGrid::PufitGrid(const PufitGrid &other) : PufitGrid(other.parameters())
    {
      *this = other;
    }

    PufitGrid &PufitGrid::operator=(const PufitGrid &other)
    {
      if (parameters() != other.parameters())
        throw std::invalid_argument("Grid parameters do not match!");
      std::copy(other.begin(), other.end(), m_towers.begin());
      return *this;
    }

    void PufitGrid::reset()
    {
      // Here we construct a standalone tower to copy in 0 values to the tower
      // kinematics. This kind of object is safe here but in general it's not a
      // good idea to have one.
      std::fill(begin(), end(), Tower(nullptr, -1));
    }

    PufitGrid &PufitGrid::operator+=(const SignedKinematics &kin)
    {
      // Find the right tower to add to
      bool outOfRange = false;
      std::size_t index = getIndex(kin.eta(), kin.phi(), outOfRange);
      if (!outOfRange)
        operator[](index) += kin;
      return *this;
    }
    PufitGrid &PufitGrid::operator-=(const SignedKinematics &kin)
    {
      // Find the right tower to subtract from
      bool outOfRange = false;
      std::size_t index = getIndex(kin.eta(), kin.phi(), outOfRange);
      if (!outOfRange)
        operator[](index) -= kin;
      return *this;
    }

    PufitGrid::Tower &PufitGrid::operator[](
        const std::pair<std::size_t, std::size_t> &indices)
    {
      return operator[](globalIndex(indices.first, indices.second));
    }
    const PufitGrid::Tower &PufitGrid::operator[](
        const std::pair<std::size_t, std::size_t> &indices) const
    {
      return operator[](globalIndex(indices.first, indices.second));
    }

    PufitGrid::Tower &PufitGrid::operator[](std::size_t index)
    {
      return m_towers.at(index);
    }
    const PufitGrid::Tower &PufitGrid::operator[](std::size_t index) const
    {
      return m_towers.at(index);
    }

    std::vector<PufitGrid::Tower>::iterator PufitGrid::begin()
    {
      return m_towers.begin();
    }
    std::vector<PufitGrid::Tower>::const_iterator PufitGrid::begin() const
    {
      return m_towers.begin();
    }
    std::vector<PufitGrid::Tower>::iterator PufitGrid::end()
    {
      return m_towers.end();
    }
    std::vector<PufitGrid::Tower>::const_iterator PufitGrid::end() const
    {
      return m_towers.end();
    }

    METComponent PufitGrid::sum(SumStrategy strategy) const
    {
      METComponent total;
      switch (strategy)
      {
      case SumStrategy::All:
        for (const Tower &tower : *this)
          total += tower;
        break;
      case SumStrategy::Masked:
        for (const Tower &tower : *this)
          if (tower.masked())
            total += tower;
        break;
      case SumStrategy::Unmasked:
        for (const Tower &tower : *this)
          if (!tower.masked())
            total += tower;
        break;
      }
      return total;
    }

    PufitGrid &PufitGrid::operator+=(const PufitGrid &other)
    {
      if (parameters() != other.parameters())
        throw std::invalid_argument("Grid parameters do not match");
      auto itr = begin();
      auto otherItr = other.begin();
      for (; itr != end(); ++itr, ++otherItr)
        *itr += *otherItr;
      return *this;
    }
    PufitGrid &PufitGrid::operator-=(const PufitGrid &other)
    {
      if (parameters() != other.parameters())
        throw std::invalid_argument("Grid parameters do not match");
      auto itr = begin();
      auto otherItr = other.begin();
      for (; itr != end(); ++itr, ++otherItr)
        *itr -= *otherItr;
      return *this;
    }

    PufitGridSet::PufitGridSet(double maxEta, std::size_t nEta, std::size_t nPhi)
        : grids({PufitGrid(maxEta, nEta, nPhi, false, false),
                 PufitGrid(maxEta, nEta, nPhi, true, false),
                 PufitGrid(maxEta, nEta, nPhi, false, true),
                 PufitGrid(maxEta, nEta, nPhi, true, true)})
    {
    }

    PufitGridSet &PufitGridSet::operator+=(const SignedKinematics &kin)
    {
      for (PufitGrid &grid : grids)
        grid += kin;
      return *this;
    }
    PufitGridSet &PufitGridSet::operator-=(const SignedKinematics &kin)
    {
      for (PufitGrid &grid : grids)
        grid -= kin;
      return *this;
    }
    PufitGrid &PufitGridSet::operator[](GridDisplacement displacement)
    {
      return grids[displacement];
    }
    const PufitGrid &PufitGridSet::operator[](GridDisplacement displacement) const
    {
      return grids[displacement];
    }

    PufitGrid operator+(const PufitGrid &lhs, const PufitGrid &rhs)
    {
      PufitGrid ret(lhs);
      ret += rhs;
      return ret;
    }
    PufitGrid operator-(const PufitGrid &lhs, const PufitGrid &rhs)
    {
      PufitGrid ret(lhs);
      ret -= rhs;
      return ret;
    }

  } // namespace MET
} // namespace HLT
