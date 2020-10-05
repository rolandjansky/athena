/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRYINTERFACES_IACTSMATERIALSTEPCONVERTERTOOL_H
#define ACTSGEOMETRYINTERFACES_IACTSMATERIALSTEPCONVERTERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "Acts/Propagator/MaterialInteractor.hpp"
#include "TrkGeometry/MaterialStepCollection.h"

class IActsMaterialStepConverterTool : virtual public IAlgTool {
  public:

  DeclareInterfaceID(IActsMaterialStepConverterTool, 1, 0);

  virtual
  const Acts::RecordedMaterialTrack
  convertToMaterialTrack(const Trk::MaterialStepCollection &colStep) const = 0;
};

#endif
