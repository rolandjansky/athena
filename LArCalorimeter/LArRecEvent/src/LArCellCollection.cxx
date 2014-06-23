/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// implementation of LArRawChannelCollection 

#include "LArRecEvent/LArCellCollection.h"
#include "LArRecEvent/LArCell.h"

// 
void LArCellCollection::setTT(const TT_ID& id, 
		    const_iterator b, const_iterator e)
{
  m_Map[id] = ITPAIR(b,e); 
  return ; 

}

void LArCellCollection::getTT(const TT_ID& id, 
	const_iterator& b,  const_iterator& e) const 
{

   ID_IT_MAP::const_iterator itPair = m_Map.find(id); 
   if(itPair!=m_Map.end()){
	// get the begin and end iterator 
	b = (*itPair).second.first; 
	e = (*itPair).second.second; 
	return; 
   }

// can't find it, return end() ; 

   b = end(); 
   e = end(); 

   return ; 
}
