/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
// IEMVertexBuilder
////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEMVERTEXBUILDER_H
#define EGAMMAINTERFACES_IEMVERTEXBUILDER_H

/**
   @class IEMVertexBuilder
   Interface for the Reconstruction/egamma/egammaTools/EMVertexBuilder

   CREATED: Nov 2011
   MODIFIED:
*/

#include "egammaInterfaces/IegammaBaseTool.h"

class egamma;

static const InterfaceID IID_IEMVertexBuilder("IEMVertexBuilder",1,0);

class IEMVertexBuilder : virtual public IegammaBaseTool
{

 public:

  /**Virtual Destructor*/
  virtual ~IEMVertexBuilder() {};

  /**AlgTool interface methods*/
  static const InterfaceID& interfaceID();

  /**Gaudi algorithm hooks*/
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;
  virtual StatusCode contExecute() = 0;

};

inline const InterfaceID& IEMVertexBuilder::interfaceID()
{
  return IID_IEMVertexBuilder;
}

#endif
