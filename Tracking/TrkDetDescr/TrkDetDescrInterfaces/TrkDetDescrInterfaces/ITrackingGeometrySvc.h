/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackingGeometrySvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ITRACKINGGEOMETRYSERVICE_H
#define TRKDETDESCRINTERFACES_ITRACKINGGEOMETRYSERVICE_H

// Include Files
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"


/** Declaration of the interface ID ( interface id, major version, minor version) */
static const InterfaceID IID_ITrackingGeometrySvc(2211, 1 , 0);

/** @class ITrackingGeometrySvc 

    The interface implemented by the TrackingGeometrySvc.

    @author Andreas.Salzburger@cern.ch
*/

namespace Trk {
  class TrackingGeometry;

  class ITrackingGeometrySvc : virtual public IInterface {
   
   public:
    //!< Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_ITrackingGeometrySvc; }

    /** Provide the TrackingGeometry */
    virtual const Trk::TrackingGeometry* trackingGeometry() const = 0;

    //!< Returns the name of the TrackingGeometry built with this Svc
    virtual const std::string& trackingGeometryName() const = 0;
  };
}

#endif // TRKDETDESCRINTERFACES_ITRACKINGGEOMETRYSERVICE_H

