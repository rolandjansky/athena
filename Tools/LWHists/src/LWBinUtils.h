/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWBinUtils                          //
//                                                            //
//  Description: Helper methods for binning related           //
//               calculations.                                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2009                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWBINUTILS_H
#define LWBINUTILS_H

#include <algorithm>
#include <cassert>
#include "LWHistBitUtils.h"
#ifndef NDEBUG
#include <iostream>
#include <climits>
#endif

class LWBinUtils {
public:

  static unsigned valueToBin( const double& x, float*varBinnings,
			      const double& invDelta,
			      const double& xmin,const double& xmax,
			      unsigned nbinsplus1 );
  static double getBinCenter( int bin, float*varBinnings,
			      const double& invDelta, const double& xmin,
			      unsigned nbinsplus1);

  static unsigned internal_bin(unsigned binx, unsigned biny, unsigned nbinsxPlus2);
  static void unpack_internal_bin(unsigned internalbin, unsigned& binx, unsigned& biny, unsigned nbinsxPlus2);

private:

  //We don't inline this, since we want to keep the inlined valueToBin
  //code-size small:
  static unsigned valueToVarBin( const double&x,
				 float*varBinnings,
				 unsigned nbinsplus1 );

  LWBinUtils();
  ~LWBinUtils();
};

///////////////
//  INLINES  //
///////////////

inline unsigned LWBinUtils::valueToBin( const double& x, float*varBinnings,
					const double& invDelta,const double& xmin,const double& xmax,
					unsigned nbinsplus1 ) {
#ifdef LW_STRICT_ROOT_BEHAVIOUR
  //In ROOT, NaN's go in overflow bin.
  if (x!=x)
    return nbinsplus1;
#endif
#ifndef NDEBUG
  if (x!=x) {
    std::cout<<"LWHisto ERROR: Saw NaN in input axis position"<<std::endl;
    return USHRT_MAX;
  }
#endif
  assert(!(x!=x));
  if (x<xmin)
    return 0;
  if (x>xmax)
    return nbinsplus1;
  if (!varBinnings)
    return static_cast<unsigned>(LWHistBitUtils::minZero(std::min<int>(nbinsplus1,int(1+invDelta*(x-xmin)))));
  return valueToVarBin(x,varBinnings,nbinsplus1);
}


inline double LWBinUtils::getBinCenter( int bin, float*varBinnings,
					const double& invDelta, const double& xmin,
					unsigned nbinsplus1)
{
  if (!varBinnings || bin<1 || bin>=int(nbinsplus1))
    return xmin + (bin-0.5)/invDelta;
  else
    return 0.5*(varBinnings[bin-1] + varBinnings[bin]);
}

inline unsigned LWBinUtils::internal_bin( unsigned binx,
					  unsigned biny,
					  unsigned nbinsxPlus2 )
{
  //Same mapping as in ROOT (mapping is important for copyContents):
  assert(binx<nbinsxPlus2);
  return biny * (nbinsxPlus2) + binx;
}

inline void LWBinUtils::unpack_internal_bin( unsigned internalbin,
					     unsigned& binx,
					     unsigned& biny,
					     unsigned nbinsxPlus2 )
{
  binx = internalbin % nbinsxPlus2;
  biny = (internalbin-binx) / nbinsxPlus2;
  assert(internal_bin(binx,biny,nbinsxPlus2)==internalbin);
}

#endif
