/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSATLASCONVERTERTOOL_H
#define ACTSGEOMETRY_ACTSATLASCONVERTERTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/EventContext.h"
#include "TrkParameters/TrackParameters.h"

// PACKAGE
#include "ActsGeometryInterfaces/IActsATLASConverterTool.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"

#include "Acts/EventData/TrackParameters.hpp"

namespace Trk {
  class Surface;
  class Track;
  class MeasurementBase;
}

namespace Acts {
  class Surface;
}

class ATLASSourceLinkContainer;

class ActsATLASConverterTool : public extends<AthAlgTool, IActsATLASConverterTool>
{

public:
  virtual StatusCode initialize() override;

  ActsATLASConverterTool(const std::string& type, const std::string& name,
	           const IInterface* parent);


  /// Find the ATLAS surface corresponding to the Acts surface 
  /// Only work if the Acts surface has an associated detector element
  /// (Pixel and SCT)
  virtual 
  const Trk::Surface&
  ActsSurfaceToATLAS(const Acts::Surface &actsSurface) const override;

  /// Find the Acts surface corresponding to the ATLAS surface 
  /// Use a map associating ATLAS ID to Acts surfaces
  /// (Pixel and SCT)
  virtual
  const Acts::Surface&
  ATLASSurfaceToActs(const Trk::Surface &atlasSurface) const override;

  /// Create an SourceLink from an ATLAS measurment
  /// Works for 1 and 2D measurmenent. 
  /// A pointer to the measurment is kept in the SourceLink
  virtual 
  const ATLASSourceLink
  ATLASMeasurementToSourceLink(const Trk::MeasurementBase *measurement) const override;

  /// Transform an ATLAS track into a vector of SourceLink to be use in the avts tracking
  /// Transform both measurement and outliers.
  virtual 
  const std::vector<ATLASSourceLink>
  ATLASTrackToSourceLink(const Trk::Track &track) const override;

  /// Create Acts TrackParameter from ATLAS one.
  /// Take care of unit conversion between the two.  
  virtual
  const Acts::BoundTrackParameters
  ATLASTrackParameterToActs(const Trk::TrackParameters *atlasParameter) const override;

  /// Create ATLAS TrackParameter from Acts one.
  /// Take care of unit conversion between the two.  
  virtual
  const Trk::TrackParameters*
  ActsTrackParameterToATLAS(const Acts::BoundTrackParameters &actsParameter, const Acts::GeometryContext& gctx) const override;

  virtual
  const IActsTrackingGeometryTool*
  trackingGeometryTool() const override
  {
    return m_trackingGeometryTool.get();
  };

private:
  ToolHandle<IActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "ActsTrackingGeometryTool"};
  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;
  std::map<Identifier, const Acts::Surface&> m_actsSurfaceMap;
};



#endif
