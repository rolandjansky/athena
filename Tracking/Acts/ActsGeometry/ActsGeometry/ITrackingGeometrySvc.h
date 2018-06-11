#ifndef GEOMACTS_ITRACKINGGEOMETRYSVC_H
#define GEOMACTS_ITRACKINGGEOMETRYSVC_H

#include "GaudiKernel/IInterface.h"


namespace Acts {
  
  class TrackingGeometry;


  class ITrackingGeometrySvc : virtual public IInterface {
    public:

    DeclareInterfaceID(ITrackingGeometrySvc, 1, 0);

    ITrackingGeometrySvc() {;}

    virtual
    std::shared_ptr<const Acts::TrackingGeometry>
    trackingGeometry() = 0;


  };
}

#endif
