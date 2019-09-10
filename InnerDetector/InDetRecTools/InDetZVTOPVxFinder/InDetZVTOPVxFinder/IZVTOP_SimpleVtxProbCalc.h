/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IZVTOP_SimpleVtxProbCalc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IINDETZVTOP_SIMPLEVTXPROBCALC_H
#define IINDETZVTOP_SIMPLEVTXPROBCALC_H

#include "GaudiKernel/IAlgTool.h"


namespace InDet 
{


  class IZVTOP_SimpleVtxProbCalc : virtual public IAlgTool {
  public:
    DeclareInterfaceID (IZVTOP_SimpleVtxProbCalc, 1, 0);

    // enter declaration of your interface-defining member functions here
    virtual double calcVtxProb(double & trk_func_sum, double & trk_func_sum2) const = 0;
  };

} // end of namespace

#endif 
