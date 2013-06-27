/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class LWBinUtils                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2009                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWBinUtils.h"
#include <algorithm>

//____________________________________________________________________
unsigned LWBinUtils::valueToVarBin( const double& x,
				    float * varBinnings,
				    unsigned nbinsplus1 )
{
  //varBinnings has nbinsplus1 entries
  assert(!(x!=x));
  assert(varBinnings);
  assert(nbinsplus1>=2);
  if (x<varBinnings[0])
    return 0;
  if (x>varBinnings[nbinsplus1-1])
    return nbinsplus1;
  const unsigned result = std::upper_bound(varBinnings,&(varBinnings[nbinsplus1]),x)-varBinnings;
  assert(result<=nbinsplus1);
  return result;
}
