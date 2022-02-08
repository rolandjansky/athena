/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Simulation data associated with an InDetRawData object
 ------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: InDetSimData.cxx,v 1.4 2004-03-01 22:07:07 costanzo Exp $


#include "InDetSimData/InDetSimData.h"
#include <iostream>



InDetSimData::InDetSimData() :
  m_word(0)
{
  
}

InDetSimData::InDetSimData (const std::vector<Deposit>& deposits, 
			    int simDataWord)
  : m_word((deposits.size()<<29 & 0xe0000000) | (simDataWord & 0x1fffffff))
{
  //above we pack deposits.size() and the simDataWord into a single unsigned int (m_word).
  //The first 3 bits are used for deposits.size() and the last 29 are used for the simDataWord.

  /*
    
  m_p_deposits = new Deposit[deposits.size()];
  
  for (unsigned int i = 0; i!= deposits.size(); i++)
  {
  m_p_deposits[i] = deposits[i];
  }
  */
  
  m_deposits = deposits;
}

InDetSimData::InDetSimData (std::vector<Deposit>&& deposits, 
			    int simDataWord)
  : m_deposits (std::move (deposits))
{
  m_word = (m_deposits.size()<<29 & 0xe0000000) | (simDataWord & 0x1fffffff);
}

InDetSimData::InDetSimData (const InDetSimData& other)
  :m_word(other.m_word),
   m_deposits(other.m_deposits)
{

}

InDetSimData &InDetSimData::operator=(const InDetSimData& other)
{
  if(&other != this) 
    { 
      m_word=other.m_word;
      m_deposits=other.m_deposits;
    }
  return *this;
}

InDetSimData &InDetSimData::operator=(InDetSimData&& other) noexcept {
  if(&other != this) 
    { 
      m_word=other.m_word;
      m_deposits=std::move(other.m_deposits);
    }
  return *this;
}

InDetSimData::~InDetSimData()
{
    //    delete[] m_p_deposits;
}
