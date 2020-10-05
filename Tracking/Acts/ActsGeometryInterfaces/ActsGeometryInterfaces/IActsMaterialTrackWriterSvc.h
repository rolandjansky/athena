/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_IACTSMATERIALTRACKWRITERSVC_H
#define ACTSGEOMETRY_IACTSMATERIALTRACKWRITERSVC_H

#include "GaudiKernel/IInterface.h"
#include "Acts/EventData/TrackParameters.hpp"

#include "Acts/Propagator/MaterialInteractor.hpp"

class IActsMaterialTrackWriterSvc : virtual public IInterface {
public:

  DeclareInterfaceID(IActsMaterialTrackWriterSvc, 1, 0);

  IActsMaterialTrackWriterSvc() {;}

  void
  virtual
  write(const Acts::RecordedMaterialTrack& mTrack) = 0;

};

#endif
