/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsSpacePointMonitoringAlgorithm.h"

namespace ActsTrk {

  ActsSpacePointMonitoringAlgorithm::ActsSpacePointMonitoringAlgorithm(const std::string& name, ISvcLocator* pSvcLocator)
    : AthMonitorAlgorithm(name, pSvcLocator) 
  {}

  StatusCode ActsSpacePointMonitoringAlgorithm::initialize() {
    ATH_MSG_INFO("Initializing " << name() << " ...");

    ATH_CHECK( m_inputSpacePointColletionKey.initialize() );

    return AthMonitorAlgorithm::initialize();
  }

  StatusCode ActsSpacePointMonitoringAlgorithm::fillHistograms(const EventContext& ctx) const {
    ATH_MSG_DEBUG( "Filling Histograms for " << name() << " ... " );

    ATH_MSG_DEBUG( "Reading input collection with key " << m_inputSpacePointColletionKey.key() );
    SG::ReadHandle< ActsTrk::SpacePointContainer > handle = SG::makeHandle( m_inputSpacePointColletionKey, ctx );
    ATH_CHECK( handle.isValid() );
    const ActsTrk::SpacePointContainer* sp_collection = handle.get();
    ATH_MSG_DEBUG( "Retrieved " << sp_collection->size() << " input elements from key " << m_inputSpacePointColletionKey.key() );

    auto monitor_nsp = Monitored::Scalar("Nsp", sp_collection->size());

    auto monitor_pos_x = Monitored::Collection("sp_pos_x", *sp_collection,
					       [] (const auto* obj) { return obj->x(); });
    auto monitor_pos_y = Monitored::Collection("sp_pos_y", *sp_collection,
					       [] (const auto* obj) { return obj->y(); });
    auto monitor_pos_z = Monitored::Collection("sp_pos_z", *sp_collection,
					       [] (const auto* obj) { return obj->z(); });
    auto monitor_pos_r = Monitored::Collection("sp_pos_r", *sp_collection,
					       [] (const auto* obj) { 
						 double x2 = obj->x() * obj->x();
						 double y2 = obj->y() * obj->y();
						 return std::sqrt(x2 + y2); });
    auto monitoring_pos_signed_r = Monitored::Collection("sp_pos_signed_r", *sp_collection,
							 [] (const auto* obj) {
							   double x2 = obj->x() * obj->x();
							   double y2 = obj->y() * obj->y();
							   double r = std::sqrt(x2 + y2);
							   return obj->y() >= 0 ? r : -r; 
							 });
         
    auto monitor_cov_r = Monitored::Collection("sp_cov_r", *sp_collection,
					       [] (const auto* obj) { return obj->varianceR(); });
    auto monitor_cov_z = Monitored::Collection("sp_cov_z", *sp_collection,
					       [] (const auto* obj) { return obj->varianceZ(); });


    fill("ActsMonitoring", monitor_nsp);
    fill("ActsMonitoring", monitor_pos_x, monitor_pos_y, monitor_pos_z, 
	 monitor_pos_r, monitoring_pos_signed_r,
	 monitor_cov_r, monitor_cov_z);

    return StatusCode::SUCCESS;
  }

}
