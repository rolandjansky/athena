/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Local
#include "TrigCostRate/LumiBlock.h"

using namespace std;

//-----------------------------------------------------------------------------
Anp::LumiBlock::LumiBlock()
  :fRun(-1),
   fLB(-1),
   fLuminosity(0),
   fFirstSec(0),
   fFirstNSec(0),
   fLastSec(0),
   fLastNSec(0),
   fNextSec(0),
   fNextNSec(0),
   fNEvent(0),
   fNEventRead(0),
   fNEventEstm(0)
{
}

//-----------------------------------------------------------------------------
Anp::LumiBlock::LumiBlock(int run, int lb)
  :fRun(run),
   fLB(lb), 
   fLuminosity(0),
   fFirstSec(0),
   fFirstNSec(0),
   fLastSec(0),
   fLastNSec(0),
   fNextSec(0),
   fNextNSec(0),
   fNEvent(0),
   fNEventRead(0),
   fNEventEstm(0)
{
}

//-----------------------------------------------------------------------------
long double Anp::LumiBlock::GetDeltaT() const
{
  //
  // Get luminosity block time length in seconds
  //
  if(fLB < 0) return 0.0; 

  long double beg_sec  = fFirstSec;
  long double beg_nsec = fFirstNSec;

  long double end_sec  = fLastSec;
  long double end_nsec = fLastNSec;

  if(fNextSec > 0) {
    end_sec  = fNextSec;
    end_nsec = fNextNSec;    
  }
  
  const long double t0 = beg_sec + 1.0e-9*beg_nsec;
  const long double t1 = end_sec + 1.0e-9*end_nsec;
  
  if(t1 > t0) return t1 - t0;
  
  return 0.0;
}
