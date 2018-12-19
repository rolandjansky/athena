/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/EnergyCMXData.h"

namespace LVL1 {

// Default constructor
EnergyCMXData::EnergyCMXData() : m_crate(0), m_module(0), m_Ex(0), m_Ey(0), m_Et(0)
{
   m_DataWords.clear();
   m_DataWords.resize(4);
}

// Construct from pre-calculated data
EnergyCMXData::EnergyCMXData(int crate, int module,
                       unsigned int ex, unsigned int ey, unsigned int et) :
  m_crate(crate),
  m_module(module),
  m_Ex(ex),
  m_Ey(ey),
  m_Et(et)
{
   m_DataWords.clear();
   m_DataWords.resize(4);
   
   /** Fill backplane data words from Ex/Ey/ET values */
   m_DataWords[0] = m_Ex;
   m_DataWords[1] = m_Ey;
   m_DataWords[2] = m_Et;
   
   // Finally set parity bits. Will assume we used odd parity here (flip initial assignment to change)
   for (unsigned int word = 0; word < 4; ++word) {
     unsigned int parity = 1;
     for (unsigned int bit = 0; bit < 24; ++bit) if ( ( (m_DataWords[word]>>bit) & 1) > 0 ) parity++;
     parity &= 1;
     m_DataWords[word] |= (parity<<23);
   }
   
}

EnergyCMXData::~EnergyCMXData()
{
}


/** Data access methods */

/** Crate number */
int LVL1::EnergyCMXData::crate() const {
  return m_crate;
}

/** module number */
int LVL1::EnergyCMXData::module() const {
  return m_module;
}


/** the 4 raw backplane data words */
std::vector<unsigned int> LVL1::EnergyCMXData::DataWords() const {
  return m_DataWords;
}

/** Ex value for module */
unsigned int LVL1::EnergyCMXData::Ex() const {  
  return m_Ex;
}

/** Ey value for module */
unsigned int LVL1::EnergyCMXData::Ey() const {  
  return m_Ey;
}

/** ET value for module */
unsigned int LVL1::EnergyCMXData::Et() const {  
  return m_Et;
}


} // end namespace
