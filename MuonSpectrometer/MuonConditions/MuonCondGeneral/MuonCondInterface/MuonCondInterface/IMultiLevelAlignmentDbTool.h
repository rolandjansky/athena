/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IMULTILEVELALIGNMENTDBTOOL_H
#define MUONCONDINTERFACE_IMULTILEVELALIGNMENTDBTOOL_H

#include "MuonCondInterface/IMuonAlignmentDbTool.h"
#include "GeoPrimitives/GeoPrimitives.h"

static const InterfaceID IID_IMultiLevelAlignmentDbTool("IMultiLevelAlignmentDbTool",1,0);

class IMultiLevelAlignmentDbTool: virtual public IMuonAlignmentDbTool {

 public:

  static const InterfaceID& interfaceID() { return IID_IMultiLevelAlignmentDbTool; }

  virtual const ALineMapContainer* level0Container()  const = 0;
  virtual const ALineMapContainer* level1Container()  const = 0;
  virtual const ALineMapContainer* level23Container() const = 0;
  virtual const ALineMapContainer* level3Container()  const = 0;

  virtual const Amg::Transform3D level0Transform() const = 0;
  virtual const Amg::Transform3D level1Transform(Identifier id) const = 0;
  virtual const Amg::Transform3D level2Transform(Identifier id) const = 0;
  virtual const Amg::Transform3D level3Transform(Identifier id) const = 0;

  virtual void decomposeTransform(const Amg::Transform3D& transform, double* values) = 0;
  
};
 
#endif // MUONCONDINTERFACE_IMULTILEVELALIGNMENTDBTOOL_H
