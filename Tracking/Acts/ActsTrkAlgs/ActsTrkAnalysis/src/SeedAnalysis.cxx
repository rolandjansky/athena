/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SeedAnalysis.h"

namespace ActsTrk {

  SeedAnalysis::SeedAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
    : AthMonitorAlgorithm(name, pSvcLocator) 
  {}

  StatusCode SeedAnalysis::initialize() {
    ATH_MSG_INFO("Initializing " << name() << " ...");

    ATH_CHECK( m_inputSeedColletionKey.initialize() );

    return AthMonitorAlgorithm::initialize();
  }

  StatusCode SeedAnalysis::fillHistograms(const EventContext& ctx) const {
    ATH_MSG_DEBUG( "Filling Histograms for " << name() << " ... " );

    ATH_MSG_DEBUG( "Reading input collection with key " << m_inputSeedColletionKey.key() );
    SG::ReadHandle< ActsTrk::SeedContainer > handle = SG::makeHandle( m_inputSeedColletionKey, ctx );
    ATH_CHECK( handle.isValid() );
    const ActsTrk::SeedContainer* seed_collection = handle.get();
    ATH_MSG_DEBUG( "Retrieved " << seed_collection->size() << " input elements from key " << m_inputSeedColletionKey.key() );
    
    auto monitor_nseed = Monitored::Scalar<int>("Nseed", seed_collection->size());
    fill("ActsTrkSeedAnalysis", monitor_nseed);

    // bottom  
    auto monitor_globalX_B = 
      Monitored::Collection("globalX_B", *seed_collection,
			    [] (const auto* seed) -> double
			    { return seed->sp()[0]->x(); });
    auto monitor_globalY_B = 
      Monitored::Collection("globalY_B", *seed_collection,
			    [] (const auto* seed) -> double
			    { return seed->sp()[0]->y(); });
    auto monitor_globalZ_B = 
      Monitored::Collection("globalZ_B", *seed_collection,
			    [] (const auto* seed) -> double
			    { return seed->sp()[0]->z(); });
    auto monitor_perp_B =
      Monitored::Collection("perp_B", *seed_collection,
			    [] (const auto* seed) -> double
			    {
			      const auto* sp = seed->sp()[0];
			      return std::sqrt(sp->x()*sp->x() + sp->y()*sp->y());
			    });

    // middle
    auto monitor_globalX_M =
      Monitored::Collection("globalX_M", *seed_collection,
			    [] (const auto* seed) -> double
			    { return seed->sp()[1]->x(); });
    auto monitor_globalY_M =
      Monitored::Collection("globalY_M", *seed_collection,
                            [] (const auto* seed) -> double
                            { return seed->sp()[1]->y(); });
    auto monitor_globalZ_M =
      Monitored::Collection("globalZ_M", *seed_collection,
                            [] (const auto* seed) -> double
                            { return seed->sp()[1]->z(); });
    auto monitor_perp_M =
      Monitored::Collection("perp_M", *seed_collection,
                            [] (const auto* seed) -> double
                            {
                              const auto* sp = seed->sp()[1];
                              return std::sqrt(sp->x()*sp->x() + sp->y()*sp->y());
                            });
    
    // top
    auto monitor_globalX_T =
      Monitored::Collection("globalX_T", *seed_collection,
			    [] (const auto* seed) -> double
			    { return seed->sp()[2]->x(); });
    auto monitor_globalY_T =
      Monitored::Collection("globalY_T", *seed_collection,
                            [] (const auto* seed) -> double
                            { return seed->sp()[2]->y(); });
    auto monitor_globalZ_T =
      Monitored::Collection("globalZ_T", *seed_collection,
                            [] (const auto* seed) -> double
                            { return seed->sp()[2]->z(); });
    auto monitor_perp_T =
      Monitored::Collection("perp_T", *seed_collection,
                            [] (const auto* seed) -> double
                            {
                              const auto* sp = seed->sp()[2];
                              return std::sqrt(sp->x()*sp->x() + sp->y()*sp->y());
                            });

    fill("ActsTrkSeedAnalysis",
	 monitor_globalX_B, monitor_globalY_B, monitor_globalZ_B, monitor_perp_B,
	 monitor_globalX_M, monitor_globalY_M, monitor_globalZ_M, monitor_perp_M,
	 monitor_globalX_T, monitor_globalY_T, monitor_globalZ_T, monitor_perp_T);

    return StatusCode::SUCCESS;
  }

}
