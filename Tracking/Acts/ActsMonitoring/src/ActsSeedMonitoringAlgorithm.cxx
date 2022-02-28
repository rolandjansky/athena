/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsSeedMonitoringAlgorithm.h"

namespace ActsTrk {

  ActsSeedMonitoringAlgorithm::ActsSeedMonitoringAlgorithm(const std::string& name, ISvcLocator* pSvcLocator)
    : AthMonitorAlgorithm(name, pSvcLocator) 
  {}

  StatusCode ActsSeedMonitoringAlgorithm::initialize() {
    ATH_MSG_INFO("Initializing " << name() << " ...");

    ATH_CHECK( m_inputSeedColletionKey.initialize() );

    return AthMonitorAlgorithm::initialize();
  }

  StatusCode ActsSeedMonitoringAlgorithm::fillHistograms(const EventContext& ctx) const {
    ATH_MSG_DEBUG( "Filling Histograms for " << name() << " ... " );

    ATH_MSG_DEBUG( "Reading input collection with key " << m_inputSeedColletionKey.key() );
    SG::ReadHandle< ActsTrk::SeedContainer > handle = SG::makeHandle( m_inputSeedColletionKey, ctx );
    ATH_CHECK( handle.isValid() );
    const ActsTrk::SeedContainer* seed_collection = handle.get();
    ATH_MSG_DEBUG( "Retrieved " << seed_collection->size() << " input elements from key " << m_inputSeedColletionKey.key() );
    
    auto monitor_nseed = Monitored::Scalar("Nseed", seed_collection->size());
    fill("ActsMonitoring", monitor_nseed);

    for (const ActsTrk::Seed* seed : *seed_collection) {
      const auto* bottom = seed->sp()[0];
      const auto* middle = seed->sp()[1];
      const auto* top = seed->sp()[2];

      float r_bottom = std::sqrt(bottom->x()*bottom->x() + bottom->y()*bottom->y());
      float r_middle = std::sqrt(middle->x()*middle->x() + middle->y()*middle->y());
      float r_top = std::sqrt(top->x()*top->x() + top->y()*top->y());

      // bottom
      auto monitor_seed_bottom_sp_pos_x = Monitored::Scalar("seed_bottom_sp_pos_x", bottom->x());
      auto monitor_seed_bottom_sp_pos_y = Monitored::Scalar("seed_bottom_sp_pos_y", bottom->y());
      auto monitor_seed_bottom_sp_pos_z = Monitored::Scalar("seed_bottom_sp_pos_z", bottom->z());
      auto monitor_seed_bottom_sp_pos_r = Monitored::Scalar("seed_bottom_sp_pos_r", r_bottom);
      auto monitor_seed_bottom_sp_pos_signed_r = Monitored::Scalar("seed_bottom_sp_pos_signed_r", bottom->y() > 0 ? r_bottom : -r_bottom);

      // middle
      auto monitor_seed_middle_sp_pos_x = Monitored::Scalar("seed_middle_sp_pos_x", middle->x());
      auto monitor_seed_middle_sp_pos_y = Monitored::Scalar("seed_middle_sp_pos_y", middle->y());
      auto monitor_seed_middle_sp_pos_z = Monitored::Scalar("seed_middle_sp_pos_z", middle->z());
      auto monitor_seed_middle_sp_pos_r = Monitored::Scalar("seed_middle_sp_pos_r", r_middle);
      auto monitor_seed_middle_sp_pos_signed_r = Monitored::Scalar("seed_middle_sp_pos_signed_r", middle->y() > 0 ? r_middle : -r_middle);

      // top
      auto monitor_seed_top_sp_pos_x = Monitored::Scalar("seed_top_sp_pos_x", top->x());
      auto monitor_seed_top_sp_pos_y = Monitored::Scalar("seed_top_sp_pos_y", top->y());
      auto monitor_seed_top_sp_pos_z = Monitored::Scalar("seed_top_sp_pos_z", top->z());
      auto monitor_seed_top_sp_pos_r = Monitored::Scalar("seed_top_sp_pos_r", r_top);
      auto monitor_seed_top_sp_pos_signed_r = Monitored::Scalar("seed_top_sp_pos_signed_r", top->y() > 0 ? r_top : -r_top);

      fill("ActsMonitoring", 
	   monitor_seed_bottom_sp_pos_x, monitor_seed_bottom_sp_pos_y, monitor_seed_bottom_sp_pos_z,
	   monitor_seed_bottom_sp_pos_r, monitor_seed_bottom_sp_pos_signed_r,
	   monitor_seed_middle_sp_pos_x, monitor_seed_middle_sp_pos_y, monitor_seed_middle_sp_pos_z,
	   monitor_seed_middle_sp_pos_r, monitor_seed_middle_sp_pos_signed_r,
	   monitor_seed_top_sp_pos_x, monitor_seed_top_sp_pos_y, monitor_seed_top_sp_pos_z,
	   monitor_seed_top_sp_pos_r, monitor_seed_top_sp_pos_signed_r);
    }

    return StatusCode::SUCCESS;
  }

}
