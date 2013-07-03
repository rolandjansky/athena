/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileTTL1Hash.cxx
//  Author   : jose.maneira@cern.ch
//  Created  : Nov 2005
//
//  DESCRIPTION:
//     Provides a hash (integer) conversion for the TileTTL1 identifiers.
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Calo include
#include "CaloIdentifier/CaloLVL1_ID.h"
                                                                                         
//TileCal include
#include "TileIdentifier/TileTTL1Hash.h" 

#include "assert.h"

// This class converts a TTL1_ID into a integer, according to its
// value of (ROS and drawer number).

// default contructor 
TileTTL1Hash::TileTTL1Hash( ) 
  : m_size(0),
    m_lookup()
{
}

void TileTTL1Hash::initialize( const CaloLVL1_ID * caloLVL1_ID)
{
  std::cout << "Initialization started" << std::endl;

  m_size = 0;
  for(int i = 0; i< m_table_size; ++i) { m_lookup[i]=-1; }

  /** iterator over all drawer Identifiers */
  std::vector<Identifier>::const_iterator first = caloLVL1_ID->tower_begin();
  std::vector<Identifier>::const_iterator  last = caloLVL1_ID->tower_end();

   int maxid = 0;
   for ( ; first!=last; ++first) {
      if(caloLVL1_ID->is_tile(*first)){
           ID id = caloLVL1_ID->tower_hash(*first);
           if (id > maxid) maxid = id;
           assert(id>=0 && id < m_table_size);
           m_int2id.push_back(id); 
           m_lookup[id] = (m_size++);
       } 
    }
  std::cout << " Number of valid tower IDs  "<< m_size << std::endl;
  std::cout << " Highest tower ID  "<< maxid  << std::endl;

  
}

TileTTL1Hash::ID TileTTL1Hash::identifier(int index) const
{
  assert(index>=0 && index < m_size);

  return m_int2id[index];
}

int TileTTL1Hash::operator() (const ID& id) const
{
  assert(id>=0 && id < m_table_size);

  return m_lookup[id]; 
}

