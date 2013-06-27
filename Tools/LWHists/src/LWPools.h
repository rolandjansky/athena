/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWPools                             //
//                                                            //
//  Description: ...                                          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2009                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWPOOLS_H
#define LWPOOLS_H

#include "LWPoolSelector.h"
#include "LWPool.h"
#include <cassert>

#define MP_NEW(Class) new(LWPools::acquire(sizeof(Class))) Class
#define MP_DELETE(Ptr) LWPools::deleteObject(Ptr)

class LWPools {
public:

  //The fundamental interface:
  static char * acquire(unsigned length);
  static void release(char*,unsigned length);
  static void cleanup();//Returns all acquired memory to the system.

  //Convenience:
  template<unsigned length> static char* acquire() { return acquire(length); }
  template<unsigned length> static void release(char*c) { return release(c,length); }
  template<class T, unsigned length> static T* acquire();
  template<class T, unsigned length> static void release(T*);
  template<class T> static T * acquire(unsigned length);
  template<class T> static void release(T*,unsigned length);

  //For MP_DELETE (to ensure the destructor gets called):
  template<class T> static void deleteObject(T*);

  //statistics:
  static long long getTotalPoolMemAllocated();
  static long long getTotalPoolMemUsed();

private:
  LWPool * m_pools[LWPoolSelector::numberOfPools];
  LWPools();
  ~LWPools();
private:
  class PoolList;
  static PoolList s_pools;
  static long long s_bytesDynAlloc;
  static LWPool * initPool(unsigned poolIndex,unsigned length);
  static LWPool * getPool(unsigned length);
  LWPools( const LWPools & );
  LWPools & operator= ( const LWPools & );
};

#include "LWPools.icc"

#endif
