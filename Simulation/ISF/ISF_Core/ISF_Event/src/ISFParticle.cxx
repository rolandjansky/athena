/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFParticle.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "ISF_Event/ISFParticle.h"

#include <cmath> // fabs

ISF::ISFParticle::ISFParticle(
  const Amg::Vector3D& pos,
  const Amg::Vector3D& mom,
  double mass,
  double charge,
  int pdgCode,
  double time,
  const ISFParticle &parent,
  Barcode::ParticleBarcode barcode,
  TruthBinding* truth):
 m_position(pos),
 m_momentum(mom),
 m_mass(mass),
 m_charge(charge),
 m_pdgCode(pdgCode),
 m_tstamp(time),
 m_history(parent.history()),
 m_barcode(barcode),
 m_bcid(parent.getBCID()),
 m_truth(truth),
 m_order(ISF::DefaultParticleOrder),
 m_userInfo(nullptr)
{

}

ISF::ISFParticle::ISFParticle(
  const HepGeom::Point3D<double>& pos,
  const HepGeom::Vector3D<double>& mom,
  double mass,
  double charge,
  int pdgCode,
  double time,
  const ISFParticle &parent,
  Barcode::ParticleBarcode barcode,
  TruthBinding* truth):
 m_position( pos.x(), pos.y(), pos.z()),
 m_momentum( mom.x(), mom.y(), mom.z()),
 m_mass(mass),
 m_charge(charge),
 m_pdgCode(pdgCode),
 m_tstamp(time),
 m_history(parent.history()),
 m_barcode(barcode),
 m_bcid(parent.getBCID()),
 m_truth(truth),
 m_order(ISF::DefaultParticleOrder),
 m_userInfo(nullptr)
{

}

ISF::ISFParticle::ISFParticle(
  const Amg::Vector3D& pos,
  const Amg::Vector3D& mom,
  double mass,
  double charge,
  int pdgCode,
  double time,
  const DetRegionSvcIDPair &origin,
  int bcid,
  Barcode::ParticleBarcode barcode,
  TruthBinding* truth):
 m_position(pos),
 m_momentum(mom),
 m_mass(mass),
 m_charge(charge),
 m_pdgCode(pdgCode),
 m_tstamp(time),
 m_history(1, origin),
 m_barcode(barcode),
 m_bcid(bcid),
 m_truth(truth),
 m_order(ISF::DefaultParticleOrder),
 m_userInfo(nullptr)
{

}


ISF::ISFParticle::ISFParticle(const ISFParticle& isfp):
  m_position(isfp.position()),
  m_momentum(isfp.momentum()),
  m_mass(isfp.mass()),
  m_charge(isfp.charge()),
  m_pdgCode(isfp.pdgCode()),
  m_tstamp(isfp.timeStamp()),
  m_history(isfp.history()),
  m_barcode(isfp.barcode()),
  m_bcid(0),
  m_truth(nullptr),
  m_order(ISF::DefaultParticleOrder),
  m_userInfo(nullptr)
{
  TruthBinding *truth = isfp.getTruthBinding();
  if (truth) {
      m_truth = new TruthBinding(*truth);
  }
}

ISF::ISFParticle::ISFParticle(ISFParticle&& isfp):
  m_position(isfp.position()),
  m_momentum(isfp.momentum()),
  m_mass(isfp.mass()),
  m_charge(isfp.charge()),
  m_pdgCode(isfp.pdgCode()),
  m_tstamp(isfp.timeStamp()),
  m_history(isfp.history()),
  m_barcode(isfp.barcode()),
  m_bcid(0),
  m_truth(isfp.getTruthBinding()),
  m_order(isfp.getOrder()),
  m_userInfo(isfp.getUserInformation())
{
}

ISF::ISFParticle::~ISFParticle() {
  delete m_truth;
  delete m_userInfo;
}

ISF::ISFParticle& ISF::ISFParticle::operator=(const ISF::ISFParticle& rhs)
{
  if (this != &rhs) {
    m_position     = rhs.position();
    m_momentum     = rhs.momentum();
    m_mass         = rhs.mass();
    m_charge       = rhs.charge();
    m_pdgCode      = rhs.pdgCode();
    m_tstamp       = rhs.timeStamp();
    m_history      = rhs.history();
    m_barcode      = rhs.barcode();
    m_bcid         = rhs.getBCID();

    delete m_truth;
    m_truth = nullptr;

    TruthBinding *rhsTruth = rhs.getTruthBinding();
    if (rhsTruth) {
        m_truth = new TruthBinding(*rhsTruth);
    }

    delete m_userInfo;
    m_userInfo = nullptr;
  }

  return *this;
}

ISF::ISFParticle& ISF::ISFParticle::operator=(ISF::ISFParticle&& rhs)
{
  m_position     = rhs.position();
  m_momentum     = rhs.momentum();
  m_mass         = rhs.mass();
  m_charge       = rhs.charge();
  m_pdgCode      = rhs.pdgCode();
  m_tstamp       = rhs.timeStamp();
  m_history      = rhs.history();
  m_barcode      = rhs.barcode();
  m_bcid         = rhs.getBCID();
  delete m_truth;
  m_truth        = rhs.getTruthBinding();
  delete m_userInfo;
  m_userInfo     = rhs.getUserInformation();

  return *this;
}

bool ISF::ISFParticle::operator==(const ISF::ISFParticle& rhs) const
{
  return isEqual(rhs);
}

bool ISF::ISFParticle::isEqual(const ISF::ISFParticle& rhs) const
{
  double epsilon = 1e-6;

  bool pass = true;
  pass &= m_position == rhs.position();
  pass &= m_momentum == rhs.momentum();
  pass &= std::fabs(m_mass-rhs.mass()) < epsilon;
  pass &= std::fabs(m_charge-rhs.charge()) < epsilon;
  pass &= m_pdgCode == rhs.pdgCode();
  pass &= std::fabs(m_tstamp-rhs.timeStamp()) < epsilon;
  pass &= m_history == rhs.history();
  pass &= m_barcode == rhs.barcode();
  pass &= m_bcid == rhs.getBCID();

  {
    const auto rhsTruthPtr = rhs.getTruthBinding();
    if (m_truth && rhsTruthPtr) {
      pass &= *m_truth == *rhsTruthPtr;
    } else {
      pass &= m_truth == rhsTruthPtr; // must be both nullptr to pass
    }
  }

  {
    const auto rhsUserInfoPtr = rhs.getUserInformation();
    if (m_userInfo && rhsUserInfoPtr) {
      pass &= *m_userInfo == *rhsUserInfoPtr;
    } else {
      pass &= m_userInfo == rhsUserInfoPtr; // must be both nullptr to pass
    }
  }

  return pass;
}

bool ISF::ISFParticle::isIdent(const ISF::ISFParticle& rhs) const
{
  bool pass = true;
  pass &= m_position == rhs.position();
  pass &= m_momentum == rhs.momentum();
  pass &= m_mass == rhs.mass();
  pass &= m_charge == rhs.charge();
  pass &= m_pdgCode == rhs.pdgCode();
  pass &= m_tstamp == rhs.timeStamp();
  pass &= m_history == rhs.history();
  pass &= m_barcode == rhs.barcode();
  pass &= m_bcid == rhs.getBCID();
  pass &= m_truth && rhs.getTruthBinding();
  pass &= m_userInfo == rhs.getUserInformation();
  return pass;
}
