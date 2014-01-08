/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyTrmusc_H
#define MuonboyTrmusc_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyTrmusc

   this class is performing matter collection

  @author samusog@cern.ch
  
  */

class MuonboyTrmusc {
public:
   MuonboyTrmusc();
  ~MuonboyTrmusc();

public:
///////////////////////////////////

   void DoIt(
             double X0,
	     double Y0,
	     double Z0,
             double Ux,
	     double Uy,
	     double Uz,
             double P,
	     double Charge,
             int Nsens
	    );

};
#endif
