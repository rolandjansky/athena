/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWPool                              //
//                                                            //
//  Description: ...                                          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWPOOL_H
#define LWPOOL_H

#include "LWPoolArea.h"
#include <vector>

class LWPool {
public:

  LWPool(unsigned chunksize);
  ~LWPool();

  char* acquire();
  char* acquireClean();//Same as acquire(), but zeroes out the acquired memory
  void release(char* );

  long long getMemDishedOut() const;
  long long getMemOwned() const;
  static long long getMotherMemOwned();
  static void forceCleanupMotherPool();
private:
  const unsigned m_chunksize;
  const unsigned m_growsize;
  unsigned m_nchunksPerArea;
  std::vector<LWPoolArea*> m_areas;
  LWPoolArea* m_likelyNonEmptyArea;//A good guess of where to find available chunks.
  LWPoolArea* m_likelyReleaseArea;//Last ::release(..) was to this area.
  mutable std::vector<LWPoolArea*>::iterator m_areaIt;
  std::vector<LWPoolArea*>::iterator m_areaItB;
  std::vector<LWPoolArea*>::iterator m_areaItE;

private:
  LWPool( const LWPool & );
  LWPool & operator= ( const LWPool & );
  LWPoolArea* grow();//Returns fresh area
  void freeArea(LWPoolArea*);//invalidates iterators
  unsigned growSize() const { return m_growsize; }
  unsigned chunkSize() const { return m_chunksize; }
  void init();
  bool belongsInArea(char*,LWPoolArea*) const;
private:
  static LWPool * s_motherPool;
  static LWPool * getMotherPool();
  char * searchAcquire();//goes through all areas until something is acquired.
  LWPoolArea* findArea(char*);//binary search for correct area
  bool isMotherPool() const { return this==s_motherPool; }
  LWPool(unsigned chunksize,unsigned growsize);//Becomes a mother mem-pool (bool parameter
                                                     //is just to trigger this constructor.
};

#include "LWPool.icc"

//For debugging allocations:
#ifdef LWHISTMALLOCDEBUG
#include <iostream>
#define LWHISTMALLOC(s) {std::cout<<"LWHISTMALLOC "<<__FILE__<<":"<<__LINE__<<" : "<<s<<" bytes"<<std::endl;}
#else
#define LWHISTMALLOC(s) {}
#endif

#endif
