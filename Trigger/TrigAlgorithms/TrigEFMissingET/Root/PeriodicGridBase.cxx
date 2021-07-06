/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
#include "TrigEFMissingET/PeriodicGridBase.h"
#include <cmath>

namespace
{
  constexpr double pi = 3.141592653589793238462643383279502884;
}

namespace HLT
{
  namespace MET
  {
    bool GridParameters::operator!=(const GridParameters &other) const
    {
      return maxEta != other.maxEta ||
             nEtaTowers != other.nEtaTowers ||
             nPhiTowers != other.nPhiTowers ||
             displaceEta != other.displaceEta ||
             displacePhi != other.displacePhi;
    }

    bool GridParameters::operator==(const GridParameters &other) const
    {
      return !(*this != other);
    }

    GridDisplacement GridParameters::displacement() const
    {
      return static_cast<GridDisplacement>(
          (displaceEta ? EtaDisplaced : NoDisplacement) |
          (displacePhi ? PhiDisplaced : NoDisplacement));
    }

    PeriodicGridBase::Tower::Tower(std::size_t index) : m_index(index) {}

    double PeriodicGridBase::Tower::towerPhi() const
    {
      return grid()->centralPhi(m_index);
    }

    double PeriodicGridBase::Tower::towerEta() const
    {
      return grid()->centralEta(m_index);
    }

    std::size_t PeriodicGridBase::Tower::index() const
    {
      return m_index;
    }

    std::size_t PeriodicGridBase::Tower::etaIndex() const
    {
      return etaPhiIndex().first;
    }

    std::size_t PeriodicGridBase::Tower::phiIndex() const
    {
      return etaPhiIndex().second;
    }

    std::pair<std::size_t, std::size_t> PeriodicGridBase::Tower::etaPhiIndex() const
    {
      return grid()->etaPhiIndex(m_index);
    }

    PeriodicGridBase::PeriodicGridBase(const GridParameters &parameters)
        : m_params(parameters)
    {
    }

    PeriodicGridBase::PeriodicGridBase(
        double maxEta,
        std::size_t nEtaTowers,
        std::size_t nPhiTowers,
        bool displaceEta,
        bool displacePhi)
        : PeriodicGridBase(GridParameters{maxEta, nEtaTowers, nPhiTowers, displaceEta, displacePhi})
    {
    }

    std::size_t PeriodicGridBase::getIndex(double eta, double phi, bool &outOfRange) const
    {
      std::size_t etaIndex = getEtaIndex(eta, outOfRange);
      if (outOfRange)
        return nTowers();
      return globalIndex(etaIndex, getPhiIndex(phi));
    }

    std::size_t PeriodicGridBase::getEtaIndex(double eta, bool &outOfRange) const
    {
      if (std::abs(eta) >= maxEta())
      {
        outOfRange = true;
        return nEtaTowers();
      }
      outOfRange = false;
      if (displaceEta())
      {
        // Apply the displacement by adding the displacement to the input
        // coordinate. This shifts the grid in the negative direction
        eta += etaWidth() / 2;
        // If necessary apply eta periodicity here
        if (eta >= maxEta())
          eta -= 2 * maxEta();
      }
      return (eta + maxEta()) / etaWidth();
    }

    std::size_t PeriodicGridBase::getPhiIndex(double phi) const
    {
      if (displacePhi())
        // Apply the displacement by adding the displacement to the input
        // coordinate. This shifts the grid in the negative direction
        phi += phiWidth() / 2;
      // Apply periodicity
      phi = std::fmod(phi, 2 * pi);
      if (phi < 0)
        phi += 2 * pi;
      return phi / phiWidth();
    }

    std::size_t PeriodicGridBase::globalIndex(std::size_t iEta, std::size_t iPhi) const
    {
      return iEta * nPhiTowers() + iPhi;
    }
    std::pair<std::size_t, std::size_t> PeriodicGridBase::etaPhiIndex(std::size_t index) const
    {
      return std::make_pair(index / nPhiTowers(), index % nPhiTowers());
    }

    double PeriodicGridBase::centralEta(std::size_t iEta) const
    {
      return -maxEta() + etaWidth() * (iEta + 0.5) + (displaceEta() ? etaWidth() / 2 : 0);
    }
    double PeriodicGridBase::centralPhi(std::size_t iPhi) const
    {
      return phiWidth() * (iPhi + 0.5) + (displacePhi() ? phiWidth() / 2 : 0);
    }

    const GridParameters &PeriodicGridBase::parameters() const { return m_params; }
    double PeriodicGridBase::maxEta() const { return parameters().maxEta; }
    std::size_t PeriodicGridBase::nEtaTowers() const { return parameters().nEtaTowers; }
    std::size_t PeriodicGridBase::nPhiTowers() const { return parameters().nPhiTowers; }
    std::size_t PeriodicGridBase::nTowers() const { return nEtaTowers() * nPhiTowers(); }
    bool PeriodicGridBase::displaceEta() const { return parameters().displaceEta; }
    bool PeriodicGridBase::displacePhi() const { return parameters().displacePhi; }
    GridDisplacement PeriodicGridBase::displacement() const
    {
      return parameters().displacement();
    }
    double PeriodicGridBase::etaWidth() const { return 2 * maxEta() / nEtaTowers(); }
    double PeriodicGridBase::phiWidth() const { return 2 * pi / nPhiTowers(); }
  } // namespace MET
} // namespace HLT