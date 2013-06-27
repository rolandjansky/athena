/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWPoolArea                 //
//                                                            //
//  Description: ...                                          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2009                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWPOOLAREA_H
#define LWPOOLAREA_H

#include "LWPoolAreaBookKeeper.h"
#include <climits>//UINT_MAX
#include <new>//placement new
#include <cassert>

class LWPoolArea {
public:

  //Create an LWPoolArea at a given address (mem will include the
  //class itself, bytes for book-keeping and finally the actual pool
  //area:
  static LWPoolArea* create( unsigned chunksize,
			     char* mem,
			     unsigned length_of_mem,
			     unsigned nchunks = UINT_MAX );
  //If nchunks==UINT_MAX, a few calculations will be performed, to fit
  //as many chunks into length_of_mem as possible (non-trivial due to
  //variable length of book-keeping bytes). If you already have at
  //least one area with the same (chunksize,length_of_mem), simply ask
  //it for the nchunks and pass here.

  char* acquire();
  void release(char*c);
  bool isUnused() const;

  long long getMemDishedOut() const;
  //For debug build only:
#ifndef NDEBUG
  long long getMemUnusedButAllocated() const;
  bool belongsInArea(char*c) const;//only in debug mode!
#endif

  unsigned totalNumberOfChunks() const { return m_nchunks; }
private:

  LWPoolArea( const LWPoolArea & );
  LWPoolArea & operator= ( const LWPoolArea & );

  //class Imp;
  LWPoolArea(unsigned chunksize,unsigned nchunks);
  ~LWPoolArea(){}//Not needed - can simply release
  const unsigned m_chunksize;
  const unsigned m_nchunks;
  LWPoolAreaBookKeeper * m_bookkeep;
  char * m_area;
  static unsigned calcNChunks(unsigned chunksize,unsigned length_of_mem);
};

#include "LWPoolArea.icc"

#endif
