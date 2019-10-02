/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INSWCalibSmearingTool_h
#define INSWCalibSmearingTool_h

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_INSWCalibSmearingTool("Muon::INSWCalibSmearingTool",1,0);

class Identifier;

namespace Muon {
  
  class INSWCalibSmearingTool : virtual public IAlgTool {
    
  public:  // static methods

    static const InterfaceID& interfaceID()  {return IID_INSWCalibSmearingTool;}

  public:  // interface methods 
  
    virtual StatusCode  smearTimeAndCharge(Identifier id, double& time, double& charge, bool& accepted) = 0;

    virtual StatusCode  smearCharge(Identifier id, double& charge, bool& accepted) = 0;
    
  };
  
}


#endif
