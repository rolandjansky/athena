/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class LWHistStats                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: October 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWHists/LWHistStats.h"
#include "LWPools.h"

long LWHistStats::s_nActiveHists = 0;

//____________________________________________________________________
long long LWHistStats::getTotalPoolMemAllocated()
{
  return LWPools::getTotalPoolMemAllocated();
}

//____________________________________________________________________
long long LWHistStats::getTotalPoolMemUsed()
{
  return LWPools::getTotalPoolMemUsed();
}

//____________________________________________________________________
long long LWHistStats::getMemUnusedButAllocatedInPools()
{
  return getTotalPoolMemAllocated()-getTotalPoolMemUsed();
}

//____________________________________________________________________
double LWHistStats::poolWasteFraction()
{
  long long l(getTotalPoolMemAllocated());
  return l ? 1.0-getTotalPoolMemUsed()*1.0/l : 0;
}
