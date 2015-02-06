/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IHISubtractorTool.h

#ifndef __HIJETREC_IHISUBTRACTORTOOL_H__
#define __HIJETREC_IHISUBTRACTORTOOL_H__

#include "AsgTools/IAsgTool.h"
#include "xAODBase/IParticle.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

////////////////////////////////////////////////////////////////////////////////
///
/// \class IHISubtractorTool
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief Abstract interface for tools that implement constituent based
/// subtraction.
///
////////////////////////////////////////////////////////////////////////////////



class IHISubtractorTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IHISubtractorTool)

  public:

  virtual ~IHISubtractorTool() { };

  /// \brief Abstract method where particle itself is not modified
  /// IParticle::FourMom_t containing kinematics after subtraction is passed by reference
  virtual void Subtract(xAOD::IParticle::FourMom_t&, const xAOD::IParticle*, const xAOD::HIEventShapeContainer*) = 0;
  


};

#endif
