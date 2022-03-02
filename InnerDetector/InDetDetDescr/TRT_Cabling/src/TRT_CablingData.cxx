/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//
// Implementation file for TRT_CablingData class
//     
//

#include "TRT_CablingData.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

  //Constructor
TRT_CablingData::TRT_CablingData()
  : m_bofai_sorted (true)
{
}

  //Destructor
TRT_CablingData::~TRT_CablingData()
{
  for (std::map< uint32_t, std::vector<IdentifierHash> *>::iterator i =
         m_collID.begin();
       i != m_collID.end();
       ++i)
    delete i->second;
}

  // Fill m_identifierForAllStraws with zeros (TB case)
void TRT_CablingData::zero_identifierForAllStraws
(int, const std::vector<Identifier>& )
{  
  return;
}

  // Fill m_identifierHashForAllStraws with zeros (TB case)
void TRT_CablingData::zero_identifierHashForAllStraws
(int, const std::vector<IdentifierHash>& )
{
  return;
}


TRT_CablingData::idandhash_t&
TRT_CablingData::find_idandhash (int rod, int bufferPosition)
{
  assert (rod >= 0);
  assert (bufferPosition >= 0);
  unsigned int rodhi = ((unsigned)rod & 0xffff0000) >> 16;
  unsigned int rodlo = (rod & 0x0000ffff);
  rodvec_t& rodvec = m_rodoffset_to_id[rodhi];
  if (rodvec.size() <= rodlo)
    rodvec.resize (rodlo + 1);
  offsetvec_t& offsetvec = rodvec[rodlo];
  if (offsetvec.size() <= (unsigned)bufferPosition)
    offsetvec.resize (bufferPosition+1);
  return offsetvec[bufferPosition];
}

const TRT_CablingData::idandhash_t&
TRT_CablingData::find_idandhash (int rod, int bufferPosition) const
{
  assert (rod >= 0);
  assert (bufferPosition >= 0);
  unsigned int rodhi = ((unsigned)rod & 0xffff0000) >> 16;
  unsigned int rodlo = (rod & 0x0000ffff);
  auto it = m_rodoffset_to_id.find (rodhi);
  if (it != m_rodoffset_to_id.end()) {
    const rodvec_t& rodvec = it->second;
    if (rodlo < rodvec.size()) {
      const offsetvec_t& offsetvec = rodvec[rodlo];
      if ((unsigned)bufferPosition < offsetvec.size())
        return offsetvec[bufferPosition];
    }
  }

  static const idandhash_t dum;
  return dum;
}

  // Set value of Identifier for each straw (TB case)
void TRT_CablingData::set_identifierForAllStraws(int rod, 
  int bufferPosition, Identifier strawID)
{    
  find_idandhash (rod, bufferPosition).first = strawID;
  m_BufferOffsetForAllIdentifiers.emplace_back(strawID, bufferPosition);
  m_bofai_sorted = false;

   //   cout << "set m_BufferOffsetForAllIdentifiers[" 
   //	<< std::hex << strawID << std::dec 
   //	<< "] = " << bufferPosition << endl;
}


 // Set value of Identifier for each straw (DC1, DC2)
void TRT_CablingData::set_identifierForAllStraws(Identifier strawID)
{
  m_identifierForAllStraws.push_back(strawID);
}


  // Set value of IdentifierHash for each straw (TB case)
void TRT_CablingData::set_identifierHashForAllStraws(int rod, 
  int bufferPosition, IdentifierHash hashId)
{    
  find_idandhash (rod, bufferPosition).second = hashId;
}

  // Set value of IdentifierHash for each straw (DC1, DC2)
void TRT_CablingData::set_identifierHashForAllStraws(IdentifierHash hashId)
{
  m_identifierHashForAllStraws.push_back(hashId);
}


Identifier TRT_CablingData::get_identifierForAllStraws(int rod, 
  int bufferPosition) const
{
  return find_idandhash (rod, bufferPosition).first;
}

IdentifierHash TRT_CablingData::get_identifierHashForAllStraws
  (int rod, int bufferPosition) const
{
  return find_idandhash (rod, bufferPosition).second;
}


  // Get value of Identifier for each straw (DC1, DC2)
Identifier TRT_CablingData::get_identifierForAllStraws(int shift) const
{
  return m_identifierForAllStraws[shift];
}

  // Get value of IdentifierHash for each straw (DC1, DC2)
IdentifierHash TRT_CablingData::get_identifierHashForAllStraws(int shift) const
{
  return m_identifierHashForAllStraws[shift];
}

// Get buffer offset from Identifier
uint32_t TRT_CablingData::get_BufferOffset( Identifier StrawId )
{
  //   cout << "get m_BufferOffsetForAllIdentifiers[" 
  //	<< std::hex << StrawId << std::dec 
  //	<< "] = " << m_BufferOffsetForAllIdentifiers[StrawId] << endl;

  if (!m_bofai_sorted) {
    std::sort (m_BufferOffsetForAllIdentifiers.begin(),
               m_BufferOffsetForAllIdentifiers.end());
    m_bofai_sorted = true;
  }

  std::vector<idpair>::iterator i =
    std::lower_bound (m_BufferOffsetForAllIdentifiers.begin(),
                      m_BufferOffsetForAllIdentifiers.end(),
                      StrawId);
  assert (i != m_BufferOffsetForAllIdentifiers.end() && i->first == StrawId);
  return i->second;
}


    // Add collection IDs for ROB
void TRT_CablingData::add_collID(uint32_t rob_id, std::vector<IdentifierHash> * vectID)
{
  m_collID[rob_id] = vectID;
}
  

  // Get Identifiers of Collections for the given ROBID
const std::vector<IdentifierHash>& TRT_CablingData::get_collID(int shift)
{
  return *m_collID[shift];
}

  // Add SourceID
void TRT_CablingData::add_allRobs(uint32_t sidROB)
{
  m_allRobs.push_back(sidROB);
}

  // Get all RODIDs for TRT
const std::vector<uint32_t>& TRT_CablingData::get_allRods() const
{
  return m_allRobs;
}

