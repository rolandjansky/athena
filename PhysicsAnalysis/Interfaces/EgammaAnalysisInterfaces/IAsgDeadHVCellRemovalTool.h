/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef __IASGDEADHVCELLREMOVALTOOL__
#define __IASGDEADHVCELLREMOVALTOOL__

#include "AsgTools/IAsgTool.h"
#include "xAODEgamma/EgammaFwd.h"

class IAsgDeadHVCellRemovalTool : virtual public asg::IAsgTool
{
  ASG_TOOL_INTERFACE(IAsgDeadHVCellRemovalTool)
public:

  /**Virtual Destructor*/
  virtual ~IAsgDeadHVCellRemovalTool() {};

  virtual bool accept( const xAOD::Egamma* part ) const = 0;

  virtual bool accept( const xAOD::Egamma& part ) const = 0;

}; 
// End: class definition

#endif

