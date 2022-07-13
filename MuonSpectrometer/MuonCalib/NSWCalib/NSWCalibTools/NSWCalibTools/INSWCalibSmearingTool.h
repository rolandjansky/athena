/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INSWCalibSmearingTool_h
#define INSWCalibSmearingTool_h

#include "GaudiKernel/IAlgTool.h"

#include "CLHEP/Random/RandomEngine.h"

class Identifier;

namespace Muon {
  
  class INSWCalibSmearingTool : virtual public IAlgTool {
    
  public:  // static methods

    static const InterfaceID& interfaceID()  {static const InterfaceID IID_INSWCalibSmearingTool("Muon::INSWCalibSmearingTool",1,0); return IID_INSWCalibSmearingTool;}

  public:  // interface methods 

    virtual StatusCode  isAccepted(const Identifier id, bool& accepted, CLHEP::HepRandomEngine* rndmEngine) const  = 0;
  
    virtual StatusCode  smearTimeAndCharge(const Identifier id, float& time, float& charge, bool& accepted, CLHEP::HepRandomEngine* rndmEngine) const = 0;

    virtual StatusCode  smearCharge(const Identifier id, float& charge, bool& accepted, CLHEP::HepRandomEngine* rndmEngine) const = 0;

    virtual StatusCode  getGainFraction(const Identifier id, float& gainFraction) = 0;
    

  };
  
}


#endif
