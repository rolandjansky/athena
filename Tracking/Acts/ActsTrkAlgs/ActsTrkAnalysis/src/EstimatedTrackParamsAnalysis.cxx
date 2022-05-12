/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "EstimatedTrackParamsAnalysis.h"

namespace ActsTrk {

  EstimatedTrackParamsAnalysis::EstimatedTrackParamsAnalysis(const std::string& name, 
							     ISvcLocator* pSvcLocator)
    : AthMonitorAlgorithm(name, pSvcLocator) 
  {}
  
  StatusCode EstimatedTrackParamsAnalysis::initialize() {
    ATH_MSG_INFO("Initializing " << name() << " ...");

    ATH_CHECK( m_inputTrackParamsColletionKey.initialize() );

    return AthMonitorAlgorithm::initialize();
  }

  StatusCode EstimatedTrackParamsAnalysis::fillHistograms(const EventContext& ctx) const {
    ATH_MSG_DEBUG( "Filling Histograms for " << name() << " ... " );

    SG::ReadHandle< ActsTrk::BoundTrackParametersContainer > trackParamsHandle = SG::makeHandle( m_inputTrackParamsColletionKey, ctx);
    ATH_CHECK( trackParamsHandle.isValid() );
    const ActsTrk::BoundTrackParametersContainer *trackParams = trackParamsHandle.get();
    ATH_MSG_DEBUG( "Retrieved " << trackParams->size() << " input parameters with key " << m_inputTrackParamsColletionKey.key() );

    auto monitor_nparams = Monitored::Scalar<int>("Nparams", trackParams->size());
    fill("ActsTrkEstimatedTrackParamsAnalysis", monitor_nparams);

    auto monitor_pt = Monitored::Collection("track_param_pt", *trackParams,
					    [] (const auto* param) -> double
					    { return param->transverseMomentum(); });
    auto monitor_eta = Monitored::Collection("track_param_eta", *trackParams,
					     [] (const auto* param) -> double
					     { return -std::log( std::tan(0.5 * param->parameters()[Acts::eBoundTheta]) ); });
    auto monitor_loc0 = Monitored::Collection("track_param_loc0", *trackParams,
					      [] (const auto* param) -> double
					      { return param->parameters()[Acts::eBoundLoc0]; });
    auto monitor_loc1 = Monitored::Collection("track_param_loc1", *trackParams,
					      [] (const auto* param) -> double
					      { return param->parameters()[Acts::eBoundLoc1]; });
    auto monitor_phi = Monitored::Collection("track_param_phi", *trackParams,
					     [] (const auto* param) -> double
					     { return param->parameters()[Acts::eBoundPhi]; });
    auto monitor_theta = Monitored::Collection("track_param_theta", *trackParams,
					       [] (const auto* param) -> double 
					       { return  param->parameters()[Acts::eBoundTheta]; });
    auto monitor_qOverP = Monitored::Collection("track_param_qoverp", *trackParams,
						[] (const auto* param) -> double
						{ return param->parameters()[Acts::eBoundQOverP]; });
    auto monitor_time = Monitored::Collection("track_param_time", *trackParams,
					      [] (const auto* param) -> double
					      { return  param->parameters()[Acts::eBoundTime]; });

    auto monitor_charge = Monitored::Collection("track_param_charge", *trackParams,
						[] (const auto* param) -> int
						{ return param->charge(); });

    fill("ActsTrkEstimatedTrackParamsAnalysis",
	 monitor_pt, monitor_eta,
	 monitor_loc0, monitor_loc1,
	 monitor_phi, monitor_theta,
	 monitor_qOverP, 
	 monitor_time,
	 monitor_charge);
    
    return StatusCode::SUCCESS;
  }

}
