/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTREAMER_ITRIGSTREAMERHYPOTOOLMT_H
#define TRIGSTREAMER_ITRIGSTREAMERHYPOTOOLMT_H

#include "AsgTools/IAsgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"


class ITrigStreamerHypoToolMT : virtual public::IAlgTool {
    
public:
  DeclareInterfaceID(ITrigStreamerHypoToolMT, 1, 0);


  virtual ~ITrigStreamerHypoToolMT(){};
  virtual StatusCode decide(bool&) const = 0;
  virtual const HLT::Identifier& getId() const = 0; 


};
#endif
