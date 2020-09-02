/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSMATERIALSTEPCONVERTERTOOL_H
#define ACTSGEOMETRY_ACTSMATERIALSTEPCONVERTERTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

// PACKAGE
#include "ActsGeometry/ActsAlignmentStore.h" // ReadCondHandleKey wants complete type
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometryInterfaces/IActsMaterialStepConverterTool.h"

class ActsMaterialStepConverterTool : public extends<AthAlgTool, IActsMaterialStepConverterTool>
{

public:
  virtual StatusCode initialize() override;

  ActsMaterialStepConverterTool(const std::string &type, const std::string &name,
                                const IInterface *parent);

  virtual
  const Acts::RecordedMaterialTrack
  convertToMaterialTrack(const Trk::MaterialStepCollection &colStep) const override;

};
#endif
