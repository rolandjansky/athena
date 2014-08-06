/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_IELEMNTTOOL_H
#define TRIG_IELEMNTTOOL_H

// $Id: $$
//
// Base class for tool which saves data from trigger elements
//

// Framework
#include "GaudiKernel/IAlgTool.h"

namespace HLT {
  class TriggerElement;
}

class TrigMonConfig;
class TrigMonTE;

namespace Trig
{
  static const InterfaceID IID_IElemNtTool ( "Trig::IElemNtTool", 1, 0 );
  
  class IElemNtTool : virtual public IAlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_IElemNtTool; }
        
    virtual bool Fill(TrigMonTE &, const TrigConfSeq &, const HLT::TriggerElement *) = 0;
  };
}

#endif
