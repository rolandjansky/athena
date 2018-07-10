#ifndef GEOMACTS_ITRACKINGGEOMETRYSVC_H
#define GEOMACTS_ITRACKINGGEOMETRYSVC_H

#include "GaudiKernel/IInterface.h"

class EventContext;
class GeoAlignmentStore;

namespace Acts {
  
  class TrackingGeometry;


  class ITrackingGeometrySvc : virtual public IInterface {
    public:

    DeclareInterfaceID(ITrackingGeometrySvc, 1, 0);

    ITrackingGeometrySvc() {;}

    virtual
    std::shared_ptr<const Acts::TrackingGeometry>
    trackingGeometry() = 0;

    virtual
    void 
    setGeoAlignmentStore(const GeoAlignmentStore* gas, const EventContext& ctx) = 0;
    
    virtual
    const GeoAlignmentStore*
    getGeoAlignmentStore(const EventContext& ctx) const = 0;

  };
}

#endif
