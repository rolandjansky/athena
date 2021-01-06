/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1Interfaces/TrigT1MuonRecRoiData.h"
#include <math.h>  /* M_PI */

namespace LVL1{

  TrigT1MuonRecRoiData::TrigT1MuonRecRoiData(const double eta, const double phi):
    m_eta(eta),
    m_phi(phi)
  {
  }
  TrigT1MuonRecRoiData::TrigT1MuonRecRoiData()
  {
  }
  TrigT1MuonRecRoiData::TrigT1MuonRecRoiData(const double eta, const double phi,
					     const double etaMin, const double etaMax,
					     const double phiMin, const double phiMax ):
    m_eta(eta),
    m_phi(phi),
    m_etaMin(etaMin),
    m_etaMax(etaMax),
    m_phiMin(phiMin),
    m_phiMax(phiMax)
  {
  }
  void TrigT1MuonRecRoiData::set_side(unsigned short side){m_side = side;}
  void TrigT1MuonRecRoiData::set_sector(unsigned short sector){m_sector=sector;}
  void TrigT1MuonRecRoiData::set_roi(unsigned short roi){m_roi=roi;}
  void TrigT1MuonRecRoiData::set_eta(double eta){m_eta=eta;}
  void TrigT1MuonRecRoiData::set_phi(double phi){m_phi=phi;}
  void TrigT1MuonRecRoiData::set_etaMin(double etaMin){m_etaMin=etaMin;}
  void TrigT1MuonRecRoiData::set_etaMax(double etaMax){m_etaMax=etaMax;}
  void TrigT1MuonRecRoiData::set_phiMin(double phiMin){m_phiMin=phiMin;}
  void TrigT1MuonRecRoiData::set_phiMax(double phiMax){m_phiMax=phiMax;}
  void TrigT1MuonRecRoiData::update(){
    if (m_etaMin>m_etaMax) {
      double tmp = m_etaMax;
      m_etaMax = m_etaMin;
      m_etaMin  = tmp;
    }
    if(m_phi < -M_PI) {
      m_phi += 2*M_PI;
    }else if (m_phi > M_PI) {
      m_phi -= 2*M_PI;
    }
    if (m_phiMin>m_phiMax&&m_phiMin-m_phiMax<M_PI) {
      double tmp = m_phiMax;
      m_phiMax = m_phiMin;
      m_phiMin  = tmp;
    }
  }
  
}
