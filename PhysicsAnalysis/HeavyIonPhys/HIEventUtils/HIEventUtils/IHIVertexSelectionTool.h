// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#ifndef HIEVENTUTILS_IHIVERTEXSELECTIONTOOL_H
#define HIEVENTUTILS_IHIVERTEXSELECTIONTOOL_H

#include "PATCore/IAsgSelectionTool.h"
#include "xAODTracking/Vertex.h"

namespace HI {

  class IHIVertexSelectionTool : public virtual ::IAsgSelectionTool {
    ASG_TOOL_INTERFACE( HI::IHIVertexSelectionTool )
  public:

    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;

    //R.Longo 13-10-2019 - Replacing PATCore/TAccept (inherited from 21.0 HI-equalization)
    virtual const asg::AcceptInfo& getAcceptInfo() const = 0;
    // we have to implement the IParticle function in order to inherit from IAsgSelectionTool
    // it will throw an exception
    virtual asg::AcceptData accept( const xAOD::IParticle* ) const = 0;
    virtual asg::AcceptData accept( const xAOD::Vertex& ) const = 0;
  };
}


#endif // HIEVENTUTILS_IHIVERTEXSELECTIONTOOL_H
