/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class LWHistControls                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWHists/LWHistControls.h"
#include "LWPools.h"

//____________________________________________________________________
class LWHistControls::Imp {
public:
  static bool s_cleanupOnGetROOT;
  static bool s_rootBackend;
};
bool LWHistControls::Imp::s_cleanupOnGetROOT = true;
bool LWHistControls::Imp::s_rootBackend = false;

//____________________________________________________________________
void LWHistControls::setCleanupOnGetROOT(bool b)
{
  Imp::s_cleanupOnGetROOT = b;
}

//____________________________________________________________________
bool LWHistControls::cleanupOnGetROOT()
{
  return Imp::s_cleanupOnGetROOT;
}

//____________________________________________________________________
void LWHistControls::releaseAllHeldMemory()
{
  LWPools::cleanup();
}

//____________________________________________________________________
bool LWHistControls::hasROOTBackend()
{
  return Imp::s_rootBackend;
}

//____________________________________________________________________
void LWHistControls::setROOTBackend(bool b)
{
  Imp::s_rootBackend = b;
}

//____________________________________________________________________
long long LWHistControls::getTotalPoolMemAllocated()
{
  return LWPools::getTotalPoolMemAllocated();
}

//____________________________________________________________________
long long LWHistControls::getTotalPoolMemUsed()
{
  return LWPools::getTotalPoolMemUsed();
}

//____________________________________________________________________
long long LWHistControls::getMemUnusedButAllocatedInPools()
{
  return getTotalPoolMemAllocated()-getTotalPoolMemUsed();
}

//____________________________________________________________________
double LWHistControls::poolWasteFraction()
{
  long long l(getTotalPoolMemAllocated());
  return l ? 1.0-getTotalPoolMemUsed()*1.0/l : 0;
}
