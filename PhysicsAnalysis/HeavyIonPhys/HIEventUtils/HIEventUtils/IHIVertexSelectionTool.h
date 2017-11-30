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

    virtual const Root::TAccept& getTAccept() const = 0;
    // we have to implement the IParticle function in order to inherit from IAsgSelectionTool
    // it will throw an exception
    virtual const Root::TAccept& accept( const xAOD::IParticle* ) const = 0;
    virtual const Root::TAccept& accept( const xAOD::Vertex& ) const = 0;
  };
}


#endif // HIEVENTUTILS_IHIVERTEXSELECTIONTOOL_H
