/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigEFMissingET/PufitGrid.h"
#include "TrigEFMissingET/METComponent.h"
#include <algorithm>
#include <cmath>


namespace HLT { namespace MET {

  PufitGrid::Tower::Tower(const PufitGrid* parent, std::size_t index) :
    m_parent(parent),
    m_index(index)
  {
  }

  PufitGrid::Tower& PufitGrid::Tower::operator=(const Tower& other)
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

  double PufitGrid::Tower::towerPhi() const {
    return grid()->centralPhi(phiIndex() );
  }
  double PufitGrid::Tower::towerEta() const {
    return grid()->centralEta(etaIndex() );
  }
  std::size_t PufitGrid::Tower::index() const { return m_index; }
  std::size_t PufitGrid::Tower::etaIndex() const { 
    return etaPhiIndex().first;
  }
  std::size_t PufitGrid::Tower::phiIndex() const {
    return etaPhiIndex().second;
  }
  std::pair<std::size_t, std::size_t> PufitGrid::Tower::etaPhiIndex() const {
    return grid()->etaPhiIndex(index() );
  }
  const PufitGrid* PufitGrid::Tower::grid() const { return m_parent; }

  SignedKinematics PufitGrid::Tower::kinematics() const {
    return m_kin;
  }
  PufitGrid::Tower::operator SignedKinematics() const { return kinematics(); }

  PufitGrid::Tower& PufitGrid::Tower::operator+=(const SignedKinematics& kin)
  {
    m_kin += kin;
    m_sumEt += kin.pt();
    m_sumE += kin.energy();
    return *this;
  }
  PufitGrid::Tower& PufitGrid::Tower::operator-=(const SignedKinematics& kin)
  {
    m_kin -= kin;
    m_sumEt -= kin.pt();
    m_sumE -= kin.energy();
    return *this;
  }
  PufitGrid::Tower& PufitGrid::Tower::operator+=(const Tower& other)
  {
    m_kin += other.kinematics();
    m_sumEt += other.sumEt();
    m_sumE += other.sumE();
    return *this;
  }
  PufitGrid::Tower& PufitGrid::Tower::operator-=(const Tower& other)
  {
    m_kin -= other.kinematics();
    m_sumEt -= other.sumEt();
    m_sumE -= other.sumE();
    return *this;
  }

  bool PufitGrid::GridParameters::operator!=(const GridParameters& other) const
  {
    return maxEta != other.maxEta ||
      nEtaTowers != other.nEtaTowers ||
      nPhiTowers != other.nPhiTowers ||
      displaceEta != other.displaceEta ||
      displacePhi != other.displacePhi;
  }
  bool PufitGrid::GridParameters::operator==(const GridParameters& other) const
  {
    return !(*this != other);
  }

  PufitGrid::PufitGrid(
      double maxEta,
      std::size_t nEtaTowers,
      std::size_t nPhiTowers,
      bool displaceEta,
      bool displacePhi) :
    PufitGrid(
        GridParameters{maxEta, nEtaTowers, nPhiTowers, displaceEta, displacePhi})
  {}

  PufitGrid::PufitGrid(const GridParameters& parameters) :
    m_params(parameters),
    m_etaWidth(2*maxEta()/nEtaTowers() ),
    m_phiWidth(2*maxEta()/nPhiTowers() )
  {
    m_towers.reserve(nTowers());
    for (std::size_t index = 0; index < nTowers(); ++index)
      m_towers.emplace_back(this, index);
  }

  PufitGrid::PufitGrid(const PufitGrid& other) :
    PufitGrid(other.parameters() )
  {
    *this = other;
  }

  PufitGrid& PufitGrid::operator=(const PufitGrid& other)
  {
    if (parameters() != other.parameters() )
      throw std::invalid_argument("Grid parameters do not match!");
    std::copy(other.begin(), other.end(), m_towers.begin() );
    return *this;
  }

  void PufitGrid::reset()
  {
    // Here we construct a standalone tower to copy in 0 values to the tower
    // kinematics. This kind of object is safe here but in general it's not a
    // good idea to have one.
    std::fill(begin(), end(), Tower(nullptr, -1) );
  }

  PufitGrid& PufitGrid::operator+=(const SignedKinematics& kin)
  {
    // Find the right tower to add to
    bool outOfRange = false;
    std::size_t index = getIndex(kin.eta(), kin.phi(), outOfRange);
    if (!outOfRange)
      operator[](index) += kin;
    return *this;
  }
  PufitGrid& PufitGrid::operator-=(const SignedKinematics& kin)
  {
    // Find the right tower to subtract from
    bool outOfRange = false;
    std::size_t index = getIndex(kin.eta(), kin.phi(), outOfRange);
    if (!outOfRange)
      operator[](index) -= kin;
    return *this;
  }

  std::size_t PufitGrid::getIndex(
      double eta,
      double phi,
      bool& outOfRange) const
  {
    std::size_t etaIndex = getEtaIndex(eta, outOfRange);
    if (outOfRange)
      return nTowers();
    std::size_t phiIndex = getPhiIndex(phi);
    return globalIndex(etaIndex, phiIndex);
  }

  std::size_t PufitGrid::getEtaIndex(double eta, bool& outOfRange) const
  {
    if (fabs(eta) >= maxEta() ) {
      outOfRange = true;
      return nEtaTowers();
    }
    outOfRange = false;
    if (displaceEta() ) {
      // Apply the displacement by adding the displacement to the input
      // coordinate. This shifts the grid in the negative direction
      eta += etaWidth()/2;
      // If necessary apply eta periodicity here
      if (eta > + maxEta() )
        eta -= 2 * maxEta();
    }
    return (eta + maxEta() ) / etaWidth();
  }

  std::size_t PufitGrid::getPhiIndex(double phi) const
  {
    if (displacePhi() )
      // Apply the displacement by adding the displacement to the input
      // coordinate. This shifts the grid in the negative direction
      phi += phiWidth()/2;
    // Apply periodicity
    phi = fmod(phi, 2*TMath::Pi() );
    if (phi < 0)
      phi += 2*TMath::Pi();
    return phi / phiWidth();
  }

  PufitGrid::Tower& PufitGrid::operator[](
      const std::pair<std::size_t, std::size_t>& indices)
  {
    return operator[](globalIndex(indices.first, indices.second) );
  }
  const PufitGrid::Tower& PufitGrid::operator[](
      const std::pair<std::size_t, std::size_t>& indices) const
  {
    return operator[](globalIndex(indices.first, indices.second) );
  }

  PufitGrid::Tower& PufitGrid::operator[](std::size_t index)
  {
    return m_towers.at(index);
  }
  const PufitGrid::Tower& PufitGrid::operator[](std::size_t index) const
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

  std::size_t PufitGrid::globalIndex(std::size_t iEta, std::size_t iPhi) const
  {
    return iEta*nPhiTowers() + iPhi;
  }
  std::pair<std::size_t, std::size_t> PufitGrid::etaPhiIndex(std::size_t index) const
  {
    return std::make_pair(index/nPhiTowers(), index%nPhiTowers() );
  }

  double PufitGrid::centralEta(std::size_t iEta) const
  {
    return -maxEta() + etaWidth()*(iEta+0.5) + (displaceEta() ? etaWidth()/2 : 0);
  }
  double PufitGrid::centralPhi(std::size_t iPhi) const
  {
    return phiWidth()*(iPhi+0.5) + (displacePhi() ? phiWidth()/2 : 0);
  }

  const PufitGrid::GridParameters& PufitGrid::parameters() const { return m_params; }
  double PufitGrid::maxEta() const { return parameters().maxEta; }
  std::size_t PufitGrid::nEtaTowers() const { return parameters().nEtaTowers; }
  std::size_t PufitGrid::nPhiTowers() const { return parameters().nPhiTowers; }
  std::size_t PufitGrid::nTowers() const { return nEtaTowers()*nPhiTowers(); }
  bool PufitGrid::displaceEta() const { return parameters().displaceEta; }
  bool PufitGrid::displacePhi() const { return parameters().displacePhi; }
  double PufitGrid::etaWidth() const { return 2*maxEta() / nEtaTowers(); }
  double PufitGrid::phiWidth() const { return 2*TMath::Pi() / nPhiTowers(); }

  METComponent PufitGrid::sum(SumStrategy strategy) const
  {
    METComponent total;
    switch (strategy) {
      case SumStrategy::All:
        for (const Tower& tower : *this) total += tower;
        break;
      case SumStrategy::Masked:
        for (const Tower& tower : *this)
          if (tower.masked() )
            total += tower;
        break;
      case SumStrategy::Unmasked:
        for (const Tower& tower : *this)
          if (!tower.masked() )
            total += tower;
        break;
    }
    return total;
  }

  PufitGrid& PufitGrid::operator+=(const PufitGrid& other)
  {
    if (parameters() != other.parameters() )
      throw std::invalid_argument("Grid parameters do not match");
    auto itr = begin();
    auto otherItr = other.begin();
    for (; itr != end(); ++itr, ++otherItr)
      *itr += *otherItr;
    return *this;
  }
  PufitGrid& PufitGrid::operator-=(const PufitGrid& other)
  {
    if (parameters() != other.parameters() )
      throw std::invalid_argument("Grid parameters do not match");
    auto itr = begin();
    auto otherItr = other.begin();
    for (; itr != end(); ++itr, ++otherItr)
      *itr -= *otherItr;
    return *this;
  }

  PufitGridSet::PufitGridSet(double maxEta, std::size_t nEta, std::size_t nPhi) :
    grids({
      PufitGrid(maxEta, nEta, nPhi, false, false),
      PufitGrid(maxEta, nEta, nPhi, true,  false),
      PufitGrid(maxEta, nEta, nPhi, false, true),
      PufitGrid(maxEta, nEta, nPhi, true,  true)})
  {}

  PufitGridSet& PufitGridSet::operator+=(const SignedKinematics& kin)
  {
    for (PufitGrid& grid : grids)
      grid += kin;
    return *this;
  }
  PufitGridSet& PufitGridSet::operator-=(const SignedKinematics& kin)
  {
    for (PufitGrid& grid : grids)
      grid += kin;
    return *this;
  }
  PufitGrid& PufitGridSet::operator[](GridDisplacement displacement)
  {
    return grids[displacement];
  }
  const PufitGrid& PufitGridSet::operator[](GridDisplacement displacement) const
  {
    return grids[displacement];
  }

  PufitGrid operator+(const PufitGrid& lhs, const PufitGrid& rhs)
  {
    PufitGrid ret(lhs);
    ret += rhs;
    return ret;
  }
  PufitGrid operator-(const PufitGrid& lhs, const PufitGrid& rhs)
  {
    PufitGrid ret(lhs);
    ret -= rhs;
    return ret;
  }

} } //> end namespace HLT::MET
