/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//abstract interface

#ifndef __ITRIG_FTK_UNCERTIANTY_TOOL_H__
#define __ITRIG_FTK_UNCERTIANTY_TOOL_H__

#include "GaudiKernel/IAlgTool.h"

class FTKTrack;

//
// Name the track paramaters
//
namespace FTKTrackParam{
  enum TP {
    d0          = 0,
    z0          = 1,
    phi         = 2,
    eta         = 3,
    Ipt         = 4,
    qOp         = 5,
    theta       = 6,
    pt          = 7
  };
}

static const InterfaceID IID_ITrigFTKUncertiantyTool("ITrigFTKUncertiantyTool",1,0);

/** @class ITrigFTKUncertiantyTool

    provides the abstract interface for FTK track uncertianty Tool

    @author John Alison John.Alison@cern.ch
*/

class ITrigFTKUncertiantyTool : virtual public IAlgTool { 

 public:
  /** other standard AlgTool methods */

  static const InterfaceID& interfaceID ()   //!< the Tool's interface
  {  return IID_ITrigFTKUncertiantyTool; }  	

  virtual double getParamCovMtx  (const FTKTrack* trk, int id0, int id1) = 0;
  
};

#endif
