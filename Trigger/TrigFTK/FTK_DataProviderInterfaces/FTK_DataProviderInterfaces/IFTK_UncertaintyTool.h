/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//abstract interface

#ifndef __IFTK_UNCERTAINTY_TOOL_H__
#define __IFTK_UNCERTAINTY_TOOL_H__

#include "GaudiKernel/IAlgTool.h"

class FTK_RawTrack;

//
// Name the track paramaters
//
namespace FTKTrackParam {
  enum TP {
    d0          = 0,
    z0          = 1,
    phi         = 2,
    eta         = 3,
    Ipt         = 4,
    qOp         = 5,
    theta       = 6,
    pt          = 7,
  };
}

static const InterfaceID IID_IFTK_UncertaintyTool("IFTK_UncertaintyTool",1,0);

/** @class IFTK_UncertaintyTool

    provides the abstract interface for FTK track uncertainty Tool

    @author John Alison John.Alison@cern.ch
*/

class IFTK_UncertaintyTool : virtual public IAlgTool { 

public:
  /** other standard AlgTool methods */

  static const InterfaceID& interfaceID()   //!< the Tool's interface
  {  return IID_IFTK_UncertaintyTool; }  	

  virtual double getParamCovMtx(const FTK_RawTrack& trk, bool hasIBL, int id0, int id1) = 0;
};

#endif
