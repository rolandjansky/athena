/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class LWPools               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2009                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWPools.h"

//____________________________________________________________________
LWPools::PoolList LWPools::s_pools;
long long LWPools::s_bytesDynAlloc = 0;

//____________________________________________________________________
LWPools::PoolList::PoolList()
{
  nullout();
}

//____________________________________________________________________
void LWPools::PoolList::nullout()
{
  memset(m_poolarr,0,LWPoolSelector::nPools()*sizeof(m_poolarr[0]));
}

//____________________________________________________________________
void LWPools::PoolList::cleanup()
{
#ifdef LW_DEBUG_POOLS_DEBUG_USAGE
  if (!m_memoryHandedOut.empty())
    std::cout<<"LWPools::PoolList::cleanup() WARNING: "<<m_memoryHandedOut.size()
	     <<" unreleased pool allocations"<<std::endl;
#endif
  for (unsigned i=0;i<LWPoolSelector::nPools();++i) {
    if (m_poolarr[i])
      s_bytesDynAlloc -= sizeof(LWPool);
    delete m_poolarr[i];
  }
  nullout();
  LWPool::forceCleanupMotherPool();
}

//____________________________________________________________________
LWPool * LWPools::initPool(unsigned idx,unsigned length)
{
  assert(idx<LWPoolSelector::numberOfPools);
  assert(!s_pools[idx]);
  assert(LWPoolSelector::poolIndex(length)==idx);
  LWPool * pool = new LWPool(LWPoolSelector::poolSize(length));
  s_bytesDynAlloc += sizeof(LWPool);
  s_pools[idx] = pool;
  return pool;
}

//____________________________________________________________________
void LWPools::cleanup()
{
  s_pools.cleanup();
  //Fixme: something here to flush mother pool also
}

//____________________________________________________________________
long long LWPools::getTotalPoolMemAllocated()
{
  return LWPool::getMotherMemOwned()+s_bytesDynAlloc;
}

//____________________________________________________________________
long long LWPools::getTotalPoolMemUsed()
{
  long long l(s_bytesDynAlloc);
  for (unsigned i=0;i<LWPoolSelector::nPools();++i)
    if (s_pools[i]) l+=s_pools[i]->getMemDishedOut();
  return l;
}
