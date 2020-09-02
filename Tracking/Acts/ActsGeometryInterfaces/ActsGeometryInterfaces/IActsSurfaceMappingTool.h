/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRYINTERFACES_IACTSSURFACEMAPPINGTOOL_H
#define ACTSGEOMETRYINTERFACES_IACTSSURFACEMAPPINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

#include "Acts/Material/SurfaceMaterialMapper.hpp"

class IActsTrackingGeometryTool;

class IActsSurfaceMappingTool : virtual public IAlgTool {
  public:

  DeclareInterfaceID(IActsSurfaceMappingTool, 1, 0);


  virtual
  std::shared_ptr<Acts::SurfaceMaterialMapper>
  mapper() const = 0;

  virtual
  Acts::SurfaceMaterialMapper::State
  mappingState() const = 0;

  virtual
  const IActsTrackingGeometryTool*
  trackingGeometryTool() const = 0;

};

#endif
