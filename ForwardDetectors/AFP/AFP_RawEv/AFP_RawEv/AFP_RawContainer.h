/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWCONTAINER_H
#define AFP_RAWCONTAINER_H

#include "AFP_RawEv/AFP_SiRawCollection.h"
#include "AFP_RawEv/AFP_ToFRawCollection.h"
#include "SGTools/CLASS_DEF.h"
#include "CLIDSvc/CLASS_DEF.h"

class AFP_RawContainer
{
public:
  uint32_t timeStamp () const {return m_timeStamp;}
  void setTimeStamp (const uint32_t timeStamp) {m_timeStamp = timeStamp;}

  uint16_t bcId () const {return m_bcId;}
  void setBCId (const uint32_t bcId) {m_bcId = bcId;}
  
  uint32_t timeStampNS () const {return m_timeStampNS;}
  void setTimeStampNS (const uint32_t timeStampNS) { m_timeStampNS = timeStampNS;}
  
  uint32_t lumiBlock () const {return m_lumiBlock;}
  void setLumiBlock (const uint32_t lumiBlock) {m_lumiBlock = lumiBlock;}

  uint32_t lvl1Id () const {return m_lvl1Id;}
  void setLvl1Id (const uint32_t lvl1ID) {m_lvl1Id = lvl1ID;}

  const std::list<AFP_SiRawCollection>& collectionsSi () const {return m_collectionSi;}
  AFP_SiRawCollection& newCollectionSi () {m_collectionSi.emplace_back(); return m_collectionSi.back();}
  
  const std::list<AFP_ToFRawCollection>& collectionsToF () const {return m_collectionToF;}
  AFP_ToFRawCollection& newCollectionToF () {m_collectionToF.emplace_back(); return m_collectionToF.back();}

  void clear ();
  
private:
  uint32_t m_lvl1Id;
  uint16_t m_bcId; 
  uint32_t m_timeStamp;
  uint32_t m_timeStampNS;
  uint32_t m_lumiBlock;

  std::list<AFP_SiRawCollection> m_collectionSi;
  std::list<AFP_ToFRawCollection> m_collectionToF;
};


CLASS_DEF(AFP_RawContainer, 1212355773, 1)



#endif
