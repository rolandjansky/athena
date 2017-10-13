/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IHIUEModulatorTool.h

#ifndef HIJETREC_IHIUEMODULATORTOOL_H
#define HIJETREC_IHIUEMODULATORTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODHIEvent/HIEventShape.h"


////////////////////////////////////////////////////////////////////////////////
///
/// \class IHIUEModulatorTool
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Aug, 2015
///
/// \brief Abstract interface for tools that can provide UE modulation given 
/// an HIEventShape
///
////////////////////////////////////////////////////////////////////////////////

class IHIUEModulatorTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IHIUEModulatorTool)

  public:

  virtual ~IHIUEModulatorTool() { };

  virtual float getModulation(float, const xAOD::HIEventShape* shape = nullptr) const = 0;

  virtual StatusCode setEventShapeForModulation(const xAOD::HIEventShape*) = 0;

  virtual void setHarmonics(const std::vector<unsigned int>&) = 0;

  virtual StatusCode retrieveShape() = 0;
  virtual StatusCode getShape(const xAOD::HIEventShape* & shape) const = 0;
  
  
};

#endif
