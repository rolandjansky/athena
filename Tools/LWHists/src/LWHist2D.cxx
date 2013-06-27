/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class LWHist2D                          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWHists/LWHist2D.h"
#include "TH1.h"
#include "LWHistRootUtils.h"

//____________________________________________________________________
bool LWHist2D::apply(TH1*h) const
{
  TH2 * h2 = dynamic_cast<TH2*>(h);
  if (!h2||!LWHist::apply(h))
    return false;

  double stats[7];
  getSums(stats[0],stats[1],stats[2],stats[3],stats[4],stats[5],stats[6]);
  h->PutStats(stats);
  return true;
}
