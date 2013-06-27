/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class LWPoolArea                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWPoolArea.h"
#ifndef NDEBUG
#include <iostream>
#endif

//____________________________________________________________________
unsigned LWPoolArea::calcNChunks(unsigned chunksize,unsigned length_of_mem)
{
  assert(chunksize<UINT_MAX);
  unsigned nchunks;
  //ntotbytes will be used like this:
  //
  // [The AllocArea class]
  // [The book-keeper class + the internal fields in the book-keeper class]
  // [area=nchunks*chunksize][<chunksize bytes]
  //
  // The difficult part is the second field, which depends on nchunks.

  //1) Trivially remove the bytes needed for AllocArea:
  const unsigned ntotbytes = length_of_mem-sizeof(LWPoolArea);

  //2) Get an upper limit for nchunks, assuming the internal book-keeper fields are zero length:
  nchunks = ntotbytes/chunksize;

  //3) How many bytes, if any, are we in the red now?

  int balance = ntotbytes - nchunks*chunksize - LWPoolAreaBookKeeper::nBytesNeededForCreate(nchunks);
  if (balance<0) {
    nchunks -= (-balance)/chunksize;
    //Should be ok now, but now we might have undershot a bit, since
    //the number of internal bins might have come down! Jeeesus...:
    while (ntotbytes >= nchunks*chunksize + LWPoolAreaBookKeeper::nBytesNeededForCreate(nchunks))
      ++nchunks;
    --nchunks;
  }
  assert(ntotbytes >= nchunks*chunksize + LWPoolAreaBookKeeper::nBytesNeededForCreate(nchunks));
#ifndef NDEBUG
  long wastage = ntotbytes - nchunks*chunksize-LWPoolAreaBookKeeper::nBytesNeededForCreate(nchunks);
  assert(wastage>=0);
  assert(wastage<long(chunksize+sizeof(LWPoolAreaBookKeeper::BitField)));
  if (wastage*1.0>0.05*ntotbytes||wastage>640) {
    std::cout<<"LWHist MemPool (chunksize: "<<chunksize<<") WARNING: Wasting "
	     <<wastage<<" bytes per area ("<<(wastage*100.0/ntotbytes)<<" %)"<<std::endl;

    std::cout<<"chunksize "<<chunksize<<": totsize:"<<length_of_mem<<std::endl;
    std::cout<<"chunksize "<<chunksize<<": sizeof(LWPoolArea):"<<sizeof(LWPoolArea)<<std::endl;
    std::cout<<"chunksize "<<chunksize<<": bytes for bookkeep:"<<LWPoolAreaBookKeeper::nBytesNeededForCreate(nchunks)<<std::endl;
    unsigned bytes_useful(nchunks*chunksize);
    unsigned bytes_overhead(length_of_mem-bytes_useful);
    std::cout<<"chunksize "<<chunksize<<": nchunks:"<<nchunks<<" => "<<bytes_useful<<" bytes"<<std::endl;
    std::cout<<"chunksize "<<chunksize<<": rounding wastage:"<<wastage<<std::endl;
    std::cout<<"chunksize "<<chunksize<<": total overhead:"
	     <<bytes_overhead<<" ("<<bytes_overhead*100.0/bytes_useful<<"% ~ "
	     <<bytes_overhead*CHAR_BIT*1.0/nchunks<<" bits/chunk)"<<std::endl;
  }
#endif
  return nchunks;

}
