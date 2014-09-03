// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARATHENAPOOL_CALOTOWERSEG_P1_H
#define LARATHENAPOOL_CALOTOWERSEG_P1_H

class CaloTowerSeg_p1 
{
  public:
  CaloTowerSeg_p1()
    : m_neta(0),
      m_nphi(0),
      m_etamin(0),
      m_etamax(0),
      m_phimin(0),
      m_phimax(0),
      m_deta(0),
      m_dphi(0)
  {} 

  /*! \brief Number of \f$ \eta \f$ bins */
  unsigned short  m_neta;    
  /*! \brief Number of \f$ \varphi \f$ bins */
  unsigned short  m_nphi; 

  //these floats are double in transient class
  /*! \brief Lower boundary \f$ \eta \f$ value range */
  float m_etamin; 
  /*! \brief Upper boundary \f$ \eta \f$ value range */
  float m_etamax;
  /*! \brief Lower boundary \f$ \varphi \f$ value range */
  float m_phimin;
  /*! \brief Upper boundary \f$ \varphi \f$ value range */
  float m_phimax;

  /*! \brief Bin size \f$ \Delta \eta \f$ */
  float m_deta;  
  /*! \brief Bin size \f$ \Delta \varphi \f$ */
  float m_dphi;
    
};
#endif 
