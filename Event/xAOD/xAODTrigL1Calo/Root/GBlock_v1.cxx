/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigL1Calo/versions/GBlock_v1.h"
#include "xAODCore/AuxStoreAccessorMacros.h"

namespace
{
  const static SG::AuxElement::ConstAccessor<ElementLink<xAOD::JGTowerContainer>> accSeedTower("seedJGTower");
  const static SG::AuxElement::Decorator<ElementLink<xAOD::JGTowerContainer>> decSeedTower("seedJGTower");
  const static SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::JGTowerContainer>>> accTowers("JGTowers");
  const static SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::JGTowerContainer>>> decTowers("JGTowers");
} // namespace

namespace xAOD
{
  GBlock_v1::GBlock_v1(const GBlock_v1 &other)
      : IParticle(other),
        m_p4(other.m_p4),
        m_p4Cached(other.m_p4Cached)
  {
  }

  GBlock_v1 &GBlock_v1::operator=(const GBlock_v1 &other)
  {
    if (this != &other)
    {
      if (!container() && !hasStore())
      {
        makePrivateStore();
      }
      IParticle::operator=(other);
      m_p4 = other.m_p4;
      m_p4Cached = other.m_p4Cached;
    }
    return *this;
  }

  GBlock_v1::~GBlock_v1() {}

  double GBlock_v1::pt() const { return et(); }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(GBlock_v1, float, double, eta)
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST(GBlock_v1, float, double, eta, setEta)

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(GBlock_v1, float, double, phi)
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST(GBlock_v1, float, double, phi, setPhi)

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(GBlock_v1, float, deta, setdEta)

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(GBlock_v1, float, dphi, setdPhi)

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(GBlock_v1, float, et, setEt)

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(GBlock_v1, float, area, setArea)

  double GBlock_v1::m() const { return 0; }

  double GBlock_v1::e() const { return p4().E(); }

  double GBlock_v1::rapidity() const { return p4().Rapidity(); }

  const GBlock_v1::FourMom_t &GBlock_v1::p4() const
  {
    if (!m_p4Cached)
    {

      double PT = (pt() > 0) ? 1000. * static_cast<double>(pt()) : 0.01;

      m_p4.SetPtEtaPhiM(PT, eta(), phi(), m());
      m_p4Cached = true;
    }
    return m_p4;
  }

  Type::ObjectType GBlock_v1::type() const
  {
    return Type::Other;
  }

  const ElementLink<JGTowerContainer> &GBlock_v1::seedTowerLink() const
  {
    return accSeedTower(*this);
  }

  const JGTower *GBlock_v1::seedTower() const
  {
    const ElementLink<JGTowerContainer> &link = seedTowerLink();
    return link.isValid() ? *link : nullptr;
  }

  const std::vector<ElementLink<JGTowerContainer>> &GBlock_v1::towerLinks() const
  {
    return accTowers(*this);
  }

  const JGTower *GBlock_v1::getTower(std::size_t idx) const
  {
    const std::vector<ElementLink<JGTowerContainer>> &links = towerLinks();
    return idx >= links.size() ? nullptr : *links[idx];
  }

  void GBlock_v1::setSeedTowerLink(const ElementLink<JGTowerContainer> &link)
  {
    decSeedTower(*this) = link;
  }

  /// set the tower links
  void GBlock_v1::setTowerLinks(const std::vector<ElementLink<JGTowerContainer>> &links)
  {
    decTowers(*this) = links;
  }

} // namespace xAOD