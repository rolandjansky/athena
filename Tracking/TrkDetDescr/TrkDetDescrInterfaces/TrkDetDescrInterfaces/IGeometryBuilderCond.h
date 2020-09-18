/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IGeometryBuilderCond.hm (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_IGEOMETRYBUILDERCOND_H
#define TRKDETDESCRINTERFACES_IGEOMETRYBUILDERCOND_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
// Trk - enum
#include "TrkDetDescrUtils/GeometrySignature.h"
// STL
#include <vector>

#include "CxxUtils/checker_macros.h"

class EventIDRange;
namespace Trk {

  class TrackingGeometry;
  class TrackingVolume;

  /** Interface ID for IGeometryBuilderConds*/  
  static const InterfaceID IID_IGeometryBuilderCond("IGeometryBuilderCond", 1, 0);
  
  /** @class IGeometryBuilderCond
    
    Interface class IGeometryBuilderCond,
    the GeometryBuilder inherits from this one.
    
    VolumeBounds can be given optionally to force a specific size/shape/boundary

    This interface class implements protected glue and surface
    exchange methods, that require friend rights to the classes
    
    @author Andreas.Salzburger@cern.ch
    */
  class IGeometryBuilderCond : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~IGeometryBuilderCond(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_IGeometryBuilderCond; }

      /** TrackingGeometry Interface methode -
       *   - Event context
       *   - pair with EventIDRange and corresponding TrackingVolume, from which
       *     we retrieve the volume to wrap the TrackingGeometry around. If
       *     TrackingGeometry is nullptr, range should be infinite (so
       *     intersecting with another range has no effect)
       */
      virtual std::pair<EventIDRange, const TrackingGeometry*> trackingGeometry ATLAS_NOT_THREAD_SAFE (const EventContext& ctx, std::pair<EventIDRange, const Trk::TrackingVolume*> tVolPair) const = 0;

      /** The unique signature */
      virtual GeometrySignature geometrySignature() const = 0;
      
  };

} // end of namespace


#endif // TRKDETDESCRINTERFACES_IGEOMETRYBUILDERCOND_H


