/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class LWHist1D                          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWHists/LWHist1D.h"
#include "LWHistRootUtils.h"
#include "TH1.h"

//____________________________________________________________________
bool LWHist1D::apply(TH1*h) const
{
  if (!LWHist::apply(h))
    return false;
  double stats[4];
  getSums(stats[0],stats[1],stats[2],stats[3]);
  h->PutStats(stats);
  return true;
}
