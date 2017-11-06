#include "AFP_RawEv/AFP_RawContainer.h"

void AFP_RawContainer::clear ()
{
   m_lvl1Id = 0;
   m_bcId = 0; 
   m_timeStamp = 0;
   m_timeStampNS = 0;
   m_lumiBlock = 0;

   m_collectionSi.clear();
   m_collectionToF.clear();
}
