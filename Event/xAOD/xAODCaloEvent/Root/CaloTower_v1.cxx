/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODCaloEvent/versions/CaloTower_v1.h"
#include "xAODCaloEvent/versions/CaloTowerContainer_v1.h"


#include <cmath>

double xAOD::CaloTower_v1::m_towerMass = 0.;

xAOD::CaloTower_v1::CaloTower_v1() 
  : IParticle() 
  , m_fourmom(0.,0.,0.,0.)
  , m_eta(0.)
  , m_phi(0.)
  , m_invcosheta(0.)
  , m_isComplete(false) 
  {}


xAOD::CaloTower_v1::~CaloTower_v1()
{ }

void xAOD::CaloTower_v1::addEnergy(double energy) { 
  f_ref_e() += energy; 
  m_isComplete=false;
}

void xAOD::CaloTower_v1::setEnergy(double energy) { 
  f_ref_e()  = energy; 
  m_isComplete=false;
}

void xAOD::CaloTower_v1::reset() { 
  f_ref_e()  = 0.;     
  m_isComplete=false;
}

const xAOD::IParticle::FourMom_t& xAOD::CaloTower_v1::p4() const { this->setupFourMom(); return m_fourmom; }

double xAOD::CaloTower_v1::e()        const { return f_val_e(); }
double xAOD::CaloTower_v1::eta()      const { this->setupFourMom(); return m_eta; }
double xAOD::CaloTower_v1::phi()      const { this->setupFourMom(); return m_phi; }
double xAOD::CaloTower_v1::m()        const { return m_towerMass; }
double xAOD::CaloTower_v1::rapidity() const { return this->eta(); }
double xAOD::CaloTower_v1::pt()       const { this->setupFourMom(); return this->e()*m_invcosheta; } 

// FIXME!!!
xAOD::Type::ObjectType xAOD::CaloTower_v1::type() const { return Type::ObjectType::Other; }


void xAOD::CaloTower_v1::setupFourMom() const {
  if (m_isComplete) return;

  //const CaloTowerContainer_v1* pTowCont = dynamic_cast<const CaloTowerContainer_v1*>(this->container());
  //assert (pTowCont);
  const CaloTowerContainer_v1* pTowCont=static_cast<const CaloTowerContainer_v1*>(this->container());


  m_eta=pTowCont->eta(this->index());
  m_phi=pTowCont->phi(this->index());
  m_invcosheta = 1./std::cosh(m_eta);

  if (this->e() <= 0. ) {// negative energy towers do not have a valid four-momentum representation (but a valid energy, eta, phi)
     m_fourmom = FourMom_t(0.,0.,0.,0.); 
  }
  else {
    const double pt=this->e()*m_invcosheta;
    m_fourmom.SetPtEtaPhiM(pt,m_eta,m_phi,this->m()); 
  }
  m_isComplete=true;
  return;
}
