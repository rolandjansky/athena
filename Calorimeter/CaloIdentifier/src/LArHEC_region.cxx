/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHEC_region.h"

#include <math.h>
#include <vector>


LArHEC_region::LArHEC_region():
   m_hashMin(0)
  ,m_nbEtaBins(0)
  ,m_nPhi(0)
  ,m_minEta(99)
  ,m_granulEta(0)
  ,m_minPhi(99)
  ,m_granulPhi(0)
  ,m_fullSym(true)
  ,m_prevEtaReg(63)
  ,m_nextEtaReg(63)
{
}

LArHEC_region::LArHEC_region(unsigned int& minHash, short int& nbEtaBins, short int& nPhi,
			     float minEta, float granulEta, float minPhi, float granulPhi, bool fullSym,
			     short int& prevEtaReg, short int& nextEtaReg, 
			     std::vector<short int>& prevSampReg, std::vector<short int>& nextSampReg)
  : m_hashMin    (minHash),
    m_nbEtaBins  (nbEtaBins),
    m_nPhi       (nPhi),
    m_minEta     (minEta),
    m_granulEta  (granulEta),
    m_minPhi     (minPhi),
    m_granulPhi  (granulPhi),
    m_fullSym    (fullSym),
    m_prevEtaReg (prevEtaReg),
    m_nextEtaReg (nextEtaReg),
    m_prevSampReg(prevSampReg),
    m_nextSampReg(nextSampReg)
{
 
 
}

LArHEC_region::~LArHEC_region()
{
}

short int LArHEC_region::numberOfEta() const
{
  return m_nbEtaBins ;
}

short int LArHEC_region::phiN() const
{
  return m_nPhi ;
}

float LArHEC_region::etaGranularity() const
{
  return m_granulEta ;
}

float LArHEC_region::phiGranularity() const
{
  return m_granulPhi ;
}

bool LArHEC_region::fullTwoPi() const
{
  return m_fullSym ;
}

float LArHEC_region::etaMin() const
{
  return m_minEta ;
}

float LArHEC_region::etaMax() const
{
  return (m_minEta + m_nbEtaBins*m_granulEta) ;
}

float LArHEC_region::phiMin() const
{
  return m_minPhi ;
}

float LArHEC_region::phiMax() const
{
  return (m_minPhi + m_nPhi*m_granulPhi) ;
}

unsigned int   LArHEC_region::hashMin() const
{
  return m_hashMin ;
}

unsigned int   LArHEC_region::hashMax() const
{
  return (m_hashMin + m_nbEtaBins * m_nPhi) ;
}

short int LArHEC_region::prevEtaRegion() const
{
  return m_prevEtaReg;
}

short int LArHEC_region::nextEtaRegion() const
{
  return m_nextEtaReg;
}

bool LArHEC_region::isEtaMin(const unsigned int& index) const
{
  bool result=false;
  if( ((m_hashMin<=index) && (index<(m_hashMin+m_nPhi))))result=true;
  return result;
}

bool LArHEC_region::isEtaMax(const unsigned int& index) const
{
  bool result=false;
  if( ((hashMax()-m_nPhi)<=index) && (index<hashMax()))result=true;
  return result;
}

bool LArHEC_region::isPhiMin(const unsigned int& index) const
{
  bool result=false;
  if( !m_fullSym && (index-m_hashMin)%m_nPhi == 0 )result=true; 
  return result;
}

bool LArHEC_region::isPhiMax(const unsigned int& index) const
{
  bool result=false;
  if( !m_fullSym && (index-m_hashMin+1)%m_nPhi == 0 )result=true;  
  return result;
}

const std::vector<short int>& LArHEC_region::nextSamplingRegion() const
{
  return m_nextSampReg;
}

const std::vector<short int>& LArHEC_region::prevSamplingRegion() const
{
  return m_prevSampReg;
}






