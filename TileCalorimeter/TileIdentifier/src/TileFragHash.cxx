/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TileIdentifier/TileFragHash.h" 
#include "TileIdentifier/TileHWID.h" 
#include <cassert>
#include <cstring>

// This class converts a Frag_ID into a integer, according to its
// value of (ROS and drawer number).

// default contructor 
TileFragHash::TileFragHash( ) 
  : m_size(0)
  , m_offset(0)
  , m_type(Default)
{
    memset(m_lookup,0,sizeof(m_lookup));
}

void TileFragHash::initialize( const TileHWID * tileHWID, TYPE type )
{
  // std::cout << " TileFragHash Constructor "<< std::endl; 

  m_type = type;
  m_size = 0;
  if (!tileHWID) return;
  m_int2id.clear();
  m_int2id.reserve(tileHWID->drawer_hash_max());
  
  for(int i = 0; i< m_table_size; ++i) { m_lookup[i]=-1; }

  /** iterator over all drawer Identifiers */
  std::vector<HWIdentifier>::const_iterator first = tileHWID->drawer_begin();
  std::vector<HWIdentifier>::const_iterator  last = tileHWID->drawer_end();

  switch (type) {
  case Beam:
    m_offset = 0;
    for ( ; first!=last; ++first) {
      ID id = tileHWID->frag(*first); 
      if (id >= 0x100) continue; // skip all real drawers
      assert(id>=0 && id < m_table_size);
      
      m_int2id.push_back(id); 
      m_lookup[id] = (m_size++); 
    }
    // std::cout << " Number of valid Beam frag IDs  "<< m_size << std::endl; 
    break;
  default:
    m_offset = (int)type * 0x1000;
    for ( ; first!=last; ++first) {
      ID id = tileHWID->frag(*first);
      if (id < 0x100) continue; // skip Beam ROD frags
      assert(id>=0 && id < m_table_size);
      
      m_int2id.push_back(id); 
      m_lookup[id] = (m_size++); 
    }
    // std::cout << " Number of valid drawer IDs  "<< m_size << std::endl; 
    break;
  }
}

TileFragHash::ID TileFragHash::identifier(int index) const
{
  assert(index>=0 && index < m_size);

  return m_int2id[index];
}

int TileFragHash::operator() (const ID& id) const
{
  assert(id>=0 && id < m_table_size);

  return m_lookup[id]; 
}

