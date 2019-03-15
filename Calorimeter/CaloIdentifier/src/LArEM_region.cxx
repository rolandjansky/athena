/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEM_region.h"

#include <iostream>
#include <math.h>
#include <vector>


LArEM_region::LArEM_region():
  m_hashMin(0)
  ,m_nbEtaBins(0)
  ,m_nPhi(0)
  ,m_minEta(99)
  ,m_granulEta(0)
  ,m_minPhi(99)
  ,m_granulPhi(0)
  ,m_fullSym(true)
  ,m_isFirstBarrelRegion(false)
  ,m_prevEtaReg(63)
  ,m_nextEtaReg(63)
{
}

LArEM_region::LArEM_region(unsigned int& minHash, short int& nbEtaBins, short int& nPhi,
			   float minEta, float granulEta, float minPhi, float granulPhi, bool fullSym, bool isBarrelMiddle,
			   short int& prevEtaReg, short int& nextEtaReg, 
			   std::vector<short int>& prevSampReg, std::vector<short int>& nextSampReg,
			   std::vector<short int>& prevSubdetReg, std::vector<short int>& nextSubdetReg)
  : m_hashMin             (minHash),
    m_nbEtaBins           (nbEtaBins),
    m_nPhi                (nPhi),
    m_minEta              (minEta),
    m_granulEta           (granulEta),
    m_minPhi              (minPhi),
    m_granulPhi           (granulPhi),
    m_fullSym             (fullSym),
    m_isFirstBarrelRegion (isBarrelMiddle),
    m_prevEtaReg          (prevEtaReg),
    m_nextEtaReg          (nextEtaReg),
    m_prevSampReg         (prevSampReg),
    m_nextSampReg         (nextSampReg),
    m_prevSubdetReg       (prevSubdetReg),
    m_nextSubdetReg       (nextSubdetReg)
{
 
 
}

LArEM_region::~LArEM_region()
{
}
