/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODCaloEvent/versions/CaloTowerContainer_v1.h"

#include "CaloGeoHelpers/CaloPhiRange.h"
#include <cmath>



xAOD::CaloTowerContainer_v1::CaloTowerContainer_v1(int nEtaBins,double etaMin,double etaMax,int nPhiBins, SG::OwnershipPolicy ownPolicy,SG::IndexTrackingPolicy trackIndices)
  : CaloTowerContainerBase_v1(ownPolicy,trackIndices)
  , m_nEtaBins(nEtaBins)
  , m_etaMin(etaMin)
  , m_etaMax(etaMax)
  , m_nPhiBins(nPhiBins)
  , m_deltaEta(0.)
  , m_deltaPhi(0.)
{ this->configureGrid(); }

xAOD::CaloTowerContainer_v1::CaloTowerContainer_v1(SG::OwnershipPolicy ownPolicy,SG::IndexTrackingPolicy trackIndices)
  : CaloTowerContainerBase_v1(ownPolicy,trackIndices)
  , m_nEtaBins(0)
  , m_etaMin(0)
  , m_etaMax(0)
  , m_nPhiBins(0)
  , m_deltaEta(0.)
  , m_deltaPhi(0.)
  , m_isConfigured(false)
{ }



xAOD::CaloTowerContainer_v1::~CaloTowerContainer_v1()
{ }

bool xAOD::CaloTowerContainer_v1::configureGrid(int nEtaBins,double etaMin,double etaMax,int nPhiBins)
{ 
  // reject configuration update!
  if ( m_isConfigured ) { return false; }
  // store configuration
  m_nEtaBins = nEtaBins;
  m_etaMin   = etaMin;
  m_etaMax   = etaMax;
  m_nPhiBins = nPhiBins;
  return this->configureGrid();
}

bool xAOD::CaloTowerContainer_v1::configureGrid()
{
  if (m_nEtaBins > 0 && m_etaMin < m_etaMax && m_nPhiBins > 0) {
    m_deltaEta=(m_etaMax-m_etaMin)/m_nEtaBins;
    m_deltaPhi=(2*M_PI)/m_nPhiBins;
    m_isConfigured=true;
  }
  else
    m_isConfigured=false;
  return m_isConfigured;

}


int xAOD::CaloTowerContainer_v1::index(double eta, double phi) const {
  if (eta<m_etaMin || eta>m_etaMax) {
    return CaloTower_v1::INVALIDINDEX;
  }
  phi=CaloPhiRange::fix(phi);
  //  int iphi=(phi-m_phiMin)/m_deltaPhi;
  int iphi=(phi+M_PI)/m_deltaPhi;
  int ieta=(eta-m_etaMin)/m_deltaEta;
  return ieta*m_nPhiBins+iphi; 
}


double xAOD::CaloTowerContainer_v1::eta(size_t index) const {

  assert((int)index < m_nPhiBins * m_nEtaBins);

  int ieta=index/m_nPhiBins;
  return (0.5+ieta)*m_deltaEta+m_etaMin;
}


double xAOD::CaloTowerContainer_v1::phi(size_t index) const {

  assert((int)index < m_nPhiBins * m_nEtaBins);

  int iphi=index % m_nPhiBins;
  return (0.5 + iphi)*m_deltaPhi-M_PI;
}
