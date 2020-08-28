/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFCAL_region.h"

#include <cmath>
#include <iostream>
#include <vector>


LArFCAL_region::LArFCAL_region():
  m_hashMin(0)
  ,m_nbEtaBins(0)
  ,m_nPhi(0)
  ,m_minEta(99)
  ,m_granulEta(0)
  ,m_minPhi(99)
  ,m_granulPhi(0)
  ,m_fullSym(true)
  ,m_prevSampReg(63)
  ,m_nextSampReg(63)
{
}

LArFCAL_region::LArFCAL_region(unsigned int& minHash, short int& nbEtaBins, short int& nPhi,
			       float minEta, float granulEta, float minPhi, float granulPhi, bool fullSym,
			       short int& prevSampReg, short int& nextSampReg)
{
 
  m_hashMin     = minHash;
  m_nbEtaBins   = nbEtaBins;
  m_nPhi        = nPhi;
  m_minEta      = minEta;
  m_granulEta   = granulEta;
  m_minPhi      = minPhi;
  m_granulPhi   = granulPhi;
  m_fullSym     = fullSym;
  m_prevSampReg = prevSampReg;
  m_nextSampReg = nextSampReg;
 
}

LArFCAL_region::~LArFCAL_region()
{
}

short int LArFCAL_region::numberOfEta() const
{
  return m_nbEtaBins ;
}

short int LArFCAL_region::phiN() const
{
  return m_nPhi ;
}

float LArFCAL_region::etaGranularity() const
{
  return m_granulEta ;
}

float LArFCAL_region::phiGranularity() const
{
  return m_granulPhi ;
}

bool LArFCAL_region::fullTwoPi() const
{
  return m_fullSym ;
}

float LArFCAL_region::etaMin() const
{
  return m_minEta ;
}

float LArFCAL_region::etaMax() const
{
  return (m_minEta + m_nbEtaBins*m_granulEta) ;
}

float LArFCAL_region::phiMin() const
{
  return m_minPhi ;
}

float LArFCAL_region::phiMax() const
{
  return (m_minPhi + m_nPhi*m_granulPhi) ;
}

unsigned int   LArFCAL_region::hashMin() const
{
  return m_hashMin ;
}

unsigned int   LArFCAL_region::hashMax() const
{
  return (m_hashMin + m_nbEtaBins * m_nPhi) ;
}

bool LArFCAL_region::isEtaMin(const unsigned int& index) const
{
  bool result=false;
  if( ((m_hashMin<=index) && (index<(m_hashMin+m_nPhi))))result=true;
  return result;
}

bool LArFCAL_region::isEtaMax(const unsigned int& index) const
{
  bool result=false;
  if( ((hashMax()-m_nPhi)<=index) && (index<hashMax()))result=true;
  return result;
}

bool LArFCAL_region::isPhiMin(const unsigned int& index) const
{
  bool result=false;
  if( !m_fullSym && (index-m_hashMin)%m_nPhi == 0 )result=true; 
  return result;
}

bool LArFCAL_region::isPhiMax(const unsigned int& index) const
{
  bool result=false;
  if( !m_fullSym && (index-m_hashMin+1)%m_nPhi == 0 )result=true;  
  return result;
}

short int LArFCAL_region::nextSamplingRegion() const
{
  return  m_nextSampReg;
}

short int LArFCAL_region::prevSamplingRegion() const
{
  return  m_prevSampReg;
}





