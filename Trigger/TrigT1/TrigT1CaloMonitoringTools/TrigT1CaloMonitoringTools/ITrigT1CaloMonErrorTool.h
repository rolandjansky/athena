/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ITRIGT1CALOMONERRORTOOL_H
#define TRIGT1CALOBYTESTREAM_ITRIGT1CALOMONERRORTOOL_H

#include <string>
#include <vector>

#include "AsgTools/IAsgTool.h"
#include "GaudiKernel/IInterface.h"

class StatusCode;

namespace LVL1 {

/**
  Interface definition for TrigT1CaloMonErrorTool
*/

class ITrigT1CaloMonErrorTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(ITrigT1CaloMonErrorTool)
 public:

   /// Retrieve error vector
   virtual StatusCode retrieve(const std::vector<unsigned int>*& errColl) = 0;
   /// Return true if current event has any corruption errors
   virtual bool corrupt() = 0;
   /// Return true if current event has Full Event status generic timeout bit set
   virtual bool fullEventTimeout() = 0;
   /// Return true if any ROB/ROD fragments are missing
   virtual bool missingFragment() = 0;
   /// Return true if current event has any ROB or unpacking errors
   virtual bool robOrUnpackingError() = 0;
   /// Return corrupt events flag string
   virtual const std::string& flagCorruptEvents() const = 0;

};

} // end namespace

#endif
