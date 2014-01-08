/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyPosmusc_H
#define MuonboyPosmusc_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyPosmusc

   this class is performing matter collection

  @author samusog@cern.ch
  
  */

class MuonboyPosmusc {
public:
   MuonboyPosmusc();
  ~MuonboyPosmusc();

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
	     double Charge
	    );

};
#endif
