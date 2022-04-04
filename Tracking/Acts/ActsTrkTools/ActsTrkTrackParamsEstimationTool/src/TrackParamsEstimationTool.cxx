/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "src/TrackParamsEstimationTool.h"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"
#include "Acts/Seeding/EstimateTrackParamsFromSeed.hpp"

namespace ActsTrk {
  TrackParamsEstimationTool::TrackParamsEstimationTool(const std::string& type,
						       const std::string& name,
						       const IInterface* parent)
    : base_class(type, name, parent)  
  {}

  StatusCode TrackParamsEstimationTool::initialize() 
  {
    ATH_MSG_INFO( "Initializing " << name() << "..." );

    ATH_MSG_DEBUG( "Properties Summary:" );
    ATH_MSG_DEBUG( "   " << m_bFieldMin );
    ATH_MSG_DEBUG( "   " << m_sigmaLoc0 );
    ATH_MSG_DEBUG( "   " << m_sigmaLoc1 );
    ATH_MSG_DEBUG( "   " << m_sigmaPhi );
    ATH_MSG_DEBUG( "   " << m_sigmaTheta );
    ATH_MSG_DEBUG( "   " << m_sigmaQOverP );
    ATH_MSG_DEBUG( "   " << m_sigmaT0 );
    ATH_MSG_DEBUG( "   " << m_initialVarInflation );

    m_covariance(Acts::eBoundLoc0, Acts::eBoundLoc0) =
      m_initialVarInflation[Acts::eBoundLoc0] * m_sigmaLoc0 * m_sigmaLoc0;
    m_covariance(Acts::eBoundLoc1, Acts::eBoundLoc1) =
      m_initialVarInflation[Acts::eBoundLoc1] * m_sigmaLoc1 * m_sigmaLoc1;
    m_covariance(Acts::eBoundPhi, Acts::eBoundPhi) =
      m_initialVarInflation[Acts::eBoundPhi] * m_sigmaPhi * m_sigmaPhi;
    m_covariance(Acts::eBoundTheta, Acts::eBoundTheta) =
      m_initialVarInflation[Acts::eBoundTheta] * m_sigmaTheta * m_sigmaTheta;
    m_covariance(Acts::eBoundQOverP, Acts::eBoundQOverP) =
      m_initialVarInflation[Acts::eBoundQOverP] * m_sigmaQOverP * m_sigmaQOverP;
    m_covariance(Acts::eBoundTime, Acts::eBoundTime) =
      m_initialVarInflation[Acts::eBoundTime] * m_sigmaT0 * m_sigmaT0;

    return StatusCode::SUCCESS;
  }

  std::optional<Acts::BoundTrackParameters>
  TrackParamsEstimationTool::estimateTrackParameters(const EventContext& ctx,
						     const ActsTrk::Seed& seed,
						     const Acts::GeometryContext& geoContext,
						     const Acts::MagneticFieldContext& magFieldContext,
						     std::function<const Acts::Surface&(const ActsTrk::Seed&)> retrieveSurface) const 
  {
    const auto& sp_collection = seed.sp();
    if ( sp_collection.size() < 3 ) return std::nullopt;
    const auto& bottom_sp = sp_collection.front();

    // Magnetic Field
    ATLASMagneticFieldWrapper magneticField;
    Acts::MagneticFieldProvider::Cache magFieldCache = magneticField.makeCache( magFieldContext );
    Acts::Vector3 bField = *magneticField.getField( Acts::Vector3(bottom_sp->x(), bottom_sp->y(), bottom_sp->z()),
                                                    magFieldCache );

    // Get the surface
    const Acts::Surface& surface = retrieveSurface(seed);

    return estimateTrackParameters(ctx,
				   seed,
				   geoContext,
				   surface,
				   m_covariance,
				   bField,
				   m_bFieldMin);
  }

  std::optional<Acts::BoundTrackParameters>
  TrackParamsEstimationTool::estimateTrackParameters(const EventContext& /*ctx*/,
						     const ActsTrk::Seed& seed,
						     const Acts::GeometryContext& geoContext,
						     const Acts::Surface& surface,
						     const Acts::BoundSymMatrix& covariance,
						     const Acts::Vector3& bField,
						     double bFieldMin) const 
  {
    // Get SPs
    const auto& sp_collection = seed.sp();
    if ( sp_collection.size() < 3 ) return std::nullopt;

    // Compute Bound parameters at surface
    std::optional<Acts::BoundVector> params_opt =
      Acts::estimateTrackParamsFromSeed(geoContext,
                                        sp_collection.begin(),
                                        sp_collection.end(),
                                        surface,
                                        bField,
                                        bFieldMin);

    if ( not params_opt.has_value() )
      return std::nullopt;

    const auto& params = params_opt.value();
    double charge = std::copysign(1, params[Acts::eBoundQOverP]);

    // Create BoundTrackParameters
    return Acts::BoundTrackParameters(surface.getSharedPtr(),
                                      params,
                                      charge,
                                      covariance);
  }
  
}
// namespace ActsTrk
