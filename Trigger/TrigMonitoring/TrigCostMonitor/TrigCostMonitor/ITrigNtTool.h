/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_ITRIGNTTOOL_H
#define TRIG_ITRIGNTTOOL_H

//
// Base class for internal TrigCostMonitor tools
//

// Framework
#include "GaudiKernel/IAlgTool.h"

namespace HLT
{
  class TrigSteer;
  class AlgoConfig;
  class Navigation;
}

class TrigMonConfig;
class TrigMonEvent;
class MsgStream;

namespace Trig
{
  static const InterfaceID IID_ITrigNtTool ( "Trig::ITrigNtTool", 1, 0 );
  
  class ITrigNtTool : virtual public IAlgTool {
  public:
    
    ITrigNtTool() {}
    virtual ~ITrigNtTool() {}
    
    static const InterfaceID& interfaceID() { return IID_ITrigNtTool; }
    
    virtual void SetSteer(const HLT::TrigSteer *) {}

    // Optional way for derived tools to be passed arbitrary data
    virtual void SetOption(unsigned option) { (void)(option); /*unused here*/ }

    virtual bool Fill(TrigMonConfig *) = 0;
    virtual bool Fill(TrigMonEvent &)  = 0;

    virtual void Done() {}
  };
}

#endif
