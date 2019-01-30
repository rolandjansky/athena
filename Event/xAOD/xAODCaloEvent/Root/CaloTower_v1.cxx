/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODCaloEvent/versions/CaloTower_v1.h"
#include "xAODCaloEvent/versions/CaloTowerContainer_v1.h"
#include <cmath>

namespace{
const double s_towerMass{0.};
}

xAOD::CaloTower_v1::CaloTower_v1() 
  : IParticle() 
  {}


xAOD::CaloTower_v1::~CaloTower_v1()
{ }

void xAOD::CaloTower_v1::addEnergy(double energy) { 
  f_ref_e() += energy; 
}

void xAOD::CaloTower_v1::setEnergy(double energy) { 
  f_ref_e()  = energy; 
}

void xAOD::CaloTower_v1::reset() { 
  f_ref_e()  = 0.;     
}



double xAOD::CaloTower_v1::e()        const { return f_val_e(); }

double xAOD::CaloTower_v1::eta()      const { 
  const CaloTowerContainer_v1* pTowCont =
      static_cast<const CaloTowerContainer_v1*>(container());
  return pTowCont->eta(index());
}
 
double xAOD::CaloTower_v1::phi()      const { 
  const CaloTowerContainer_v1* pTowCont =
      static_cast<const CaloTowerContainer_v1*>(container());
  return pTowCont->phi(index());
}

double xAOD::CaloTower_v1::m()        const { return s_towerMass; }
double xAOD::CaloTower_v1::rapidity() const { return eta(); }
double xAOD::CaloTower_v1::pt()       const { return genvecP4().Pt(); } 

// FIXME!!!
xAOD::Type::ObjectType xAOD::CaloTower_v1::type() const { return Type::ObjectType::Other; }


xAOD::CaloTower_v1::FourMom_t xAOD::CaloTower_v1::p4() const { 

  FourMom_t p4;

  if (e() <= 0) {
    // negative energy towers do not have a valid four-momentum representation (but a valid energy, eta, phi)
    return p4;
  }

  const CaloTowerContainer_v1* pTowCont =
    static_cast<const CaloTowerContainer_v1*>(container());


  const double eta=pTowCont->eta(index());
  const double phi=pTowCont->phi(index());
  const double invcosheta = 1./std::cosh(eta);
  const double pt=e()*invcosheta;
  p4.SetPtEtaPhiM(pt,eta,phi,m()); 
  return p4;
}

xAOD::CaloTower_v1::GenVecFourMom_t xAOD::CaloTower_v1::genvecP4() const { 

  if (e() <= 0) {
    // negative energy towers do not have a valid four-momentum representation (but a valid energy, eta, phi)
    return GenVecFourMom_t();
  }

  const CaloTowerContainer_v1* pTowCont =
    static_cast<const CaloTowerContainer_v1*>(container());


  const double eta=pTowCont->eta(index());
  const double phi=pTowCont->phi(index());
  const double invcosheta = 1./std::cosh(eta);
  const double pt=e()*invcosheta;
  return GenVecFourMom_t(pt,eta,phi,m()); 
}
