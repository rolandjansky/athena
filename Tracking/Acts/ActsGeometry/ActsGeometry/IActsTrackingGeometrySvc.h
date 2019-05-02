/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_IACTSTRACKINGGEOMETRYSVC_H
#define ACTSGEOMETRY_IACTSTRACKINGGEOMETRYSVC_H

#include "GaudiKernel/IInterface.h"

class EventContext;
class ActsAlignmentStore;

namespace Acts {
  class TrackingGeometry;
}


class IActsTrackingGeometrySvc : virtual public IInterface {
  public:

  DeclareInterfaceID(IActsTrackingGeometrySvc, 1, 0);

  IActsTrackingGeometrySvc() {;}

  virtual
  std::shared_ptr<const Acts::TrackingGeometry>
  trackingGeometry() = 0;

  virtual
  void
  populateAlignmentStore(ActsAlignmentStore *store) const = 0;

  virtual
  const ActsAlignmentStore*
  getNominalAlignmentStore() const = 0;

};

#endif
