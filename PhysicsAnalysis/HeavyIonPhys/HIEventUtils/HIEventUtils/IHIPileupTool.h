/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIPileupTool.h

#ifndef IHIPileupTool_H
#define IHIPileupTool_H

#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"
#include "xAODForward/ZdcModuleContainer.h"
#include "xAODHIEvent/HIEventShapeContainer.h"


namespace HI

{

class IHIPileupTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE( HI::IHIPileupTool )

public:

  // Display a message.
  //virtual int talk() const =0;
  virtual const Root::TAccept& getTAccept(const xAOD::HIEventShapeContainer& evShCont, const xAOD::ZdcModuleContainer& ZdcCont) const = 0;
  //virtual const Root::TAccept& accept() const = 0;

};
}
#endif
