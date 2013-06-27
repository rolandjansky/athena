/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWPoolSelector                      //
//                                                            //
//  Description: Helper class doing the math which finds      //
//               the actual memory pool to use for a given    //
//               request.                                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWPOOLSELECTOR_H
#define LWPOOLSELECTOR_H

#include <cassert>
#include <climits>

class LWPoolSelector {
public:

  static unsigned poolSize(unsigned requestsize);//UINT_MAX indicates need for dynamic new[]
  static unsigned poolIndex(unsigned requestsize);//UINT_MAX indicates need for dynamic delete[]
  static const unsigned numberOfPools = 58;
  static unsigned nPools() { return numberOfPools; }
  static unsigned maxNonDynamicRequest() { return 5400; }

private:
  LWPoolSelector();
  ~LWPoolSelector();
  static unsigned roundup(unsigned l, unsigned n);
};

///////////////
//  Inlines  //
///////////////

// NB: It is cheap to check if an integer, i satisfies: i>=2**n,
// or i<2**n.  It is likewise cheap to divide by 2**n.

inline unsigned LWPoolSelector::roundup(unsigned l, unsigned n) {
  //Round up to nearest multiple of n without branching
  assert(l>0&&n>0);
  return n+l-1-(l-1)%n;
}

inline unsigned LWPoolSelector::poolSize(unsigned l)
{
  assert(l>0);
  if (l<16)
    return l;
  if (l<64)
    return roundup(l,4);
  if (l<128)
    return roundup(l,8);
  if (l<256)
    return roundup(l,16);
  if (l<=640)//640 should have room for sizeof(THnX)
    return roundup(l,32);
  if (l<=1024)
    return 1024;
  if (l<=maxNonDynamicRequest())
    return maxNonDynamicRequest();
  return UINT_MAX;
}

inline unsigned LWPoolSelector::poolIndex(unsigned l)
{
  assert(l>0);
  if (l<128) {
    if (l<16)
      return l-1;
    if (l<64)
      return roundup(l,4)/4+11;
    return roundup(l,8)/8+19;
  }
  if (l<256)
    return roundup(l,16)/16+27;
  if (l<=640)
    return roundup(l,32)/32+35;
  if (l<=1024)
    return nPools()-2;
  if (l<=maxNonDynamicRequest())
    return nPools()-1;
  return UINT_MAX;
}


#endif


