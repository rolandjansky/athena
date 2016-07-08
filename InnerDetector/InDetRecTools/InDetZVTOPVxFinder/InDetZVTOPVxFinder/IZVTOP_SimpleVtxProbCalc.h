/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IZVTOP_SimpleVtxProbCalc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IINDETZVTOP_SIMPLEVTXPROBCALC_H
#define IINDETZVTOP_SIMPLEVTXPROBCALC_H

#include "GaudiKernel/IAlgTool.h"


namespace InDet 
{


  static const InterfaceID IID_IZVTOP_SimpleVtxProbCalc("InDet::IZVTOP_SimpleVtxProbCalc", 1, 0);

  class IZVTOP_SimpleVtxProbCalc : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual double calcVtxProb(double & trk_func_sum, double & trk_func_sum2) = 0;
  };

  inline const InterfaceID& InDet::IZVTOP_SimpleVtxProbCalc::interfaceID()
    { 
      return IID_IZVTOP_SimpleVtxProbCalc; 
    }

} // end of namespace

#endif 
