/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"

#include "NSWL1Simulation.h"

namespace NSWL1 {
  NSWL1Simulation::NSWL1Simulation( const std::string& name, ISvcLocator* pSvcLocator )
    : AthReentrantAlgorithm( name, pSvcLocator ),
      m_tree(nullptr)
  {}


  StatusCode NSWL1Simulation::initialize() {
    ATH_MSG_DEBUG( "initialize " << name() );
    ATH_CHECK( m_trigRdoContainer.initialize() );
    // Create an register the ntuple if requested, add branch for event and run number
    if ( m_doNtuple ) {
      ITHistSvc* tHistSvc;
      ATH_CHECK(service("THistSvc", tHistSvc));

      // create Ntuple and the branches
      std::string ntuple_name = name()+"Tree";
      m_tree = new TTree(ntuple_name.c_str(), "Ntuple of NSWL1Simulation");

      std::string tdir_name = "/"+name()+"/"+ntuple_name;
      ATH_CHECK(tHistSvc->regTree(tdir_name,m_tree));
    }

    // retrieving the private tools implementing the simulation
    if(m_dosTGC){
      if(m_doPad || m_doStrip) ATH_CHECK(m_pad_tds.retrieve());
      //if(m_useLookup){
      //  ATH_CHECK(m_pad_trigger_lookup.retrieve());
      //}
      //else{
        ATH_CHECK(m_pad_trigger.retrieve());
      //}
      if(m_doStrip){
        ATH_CHECK(m_strip_tds.retrieve());
        ATH_CHECK(m_strip_cluster.retrieve());
        ATH_CHECK(m_strip_segment.retrieve());
      }
    }

    if(m_doMM ){
      ATH_CHECK(m_mmtrigger.retrieve());
    }

    ATH_CHECK(m_trigProcessor.retrieve());
    return StatusCode::SUCCESS;
  }


  StatusCode NSWL1Simulation::execute(const EventContext& ctx) const {
    std::vector<std::shared_ptr<PadData>> pads;
    std::vector<std::unique_ptr<PadTrigger>> padTriggers;
    std::vector<std::unique_ptr<StripData>> strips;
    std::vector<std::unique_ptr<StripClusterData> > clusters;
    auto padTriggerContainer = std::make_unique<Muon::NSW_PadTriggerDataContainer>();
    auto stripTriggerContainer = std::make_unique<Muon::NSW_TrigRawDataContainer>();
    auto MMTriggerContainer = std::make_unique<Muon::NSW_TrigRawDataContainer>();

    if(m_dosTGC){
      if(m_doPad || m_doStrip) ATH_CHECK( m_pad_tds->gather_pad_data(pads) );
    //  if(m_useLookup){
    //    ATH_CHECK( m_pad_trigger_lookup->lookup_pad_triggers(pads, padTriggers) );
    //  }
    //  else{
        ATH_CHECK( m_pad_trigger->compute_pad_triggers(pads, padTriggers) );
    //  }
      if(m_doStrip){
        ATH_CHECK( m_strip_tds->gather_strip_data(strips,padTriggers) );
        ATH_CHECK( m_strip_cluster->cluster_strip_data(strips,clusters) );
        ATH_CHECK( m_strip_segment->find_segments(clusters,stripTriggerContainer) );
      }
      if(m_doPad) ATH_CHECK(PadTriggerAdapter::fillContainer(padTriggerContainer, padTriggers, ctx.eventID().event_number()));
    }

    //retrive the MM Strip hit data
    if(m_doMM){
      ATH_CHECK( m_mmtrigger->runTrigger(MMTriggerContainer.get(), m_doMMDiamonds) );
    }
    if(m_doNtuple && m_tree){
      static std::mutex mutex;
      std::scoped_lock lock(mutex);
      TTree* locked_tree ATLAS_THREAD_SAFE = m_tree;
      locked_tree->Fill();
    }

    SG::WriteHandle<Muon::NSW_TrigRawDataContainer> rdohandle( m_trigRdoContainer, ctx );
    auto trgContainer=std::make_unique<Muon::NSW_TrigRawDataContainer>();
    ATH_CHECK( m_trigProcessor->mergeRDO(padTriggerContainer.get(), stripTriggerContainer.get(), MMTriggerContainer.get(), trgContainer.get()) );
    ATH_CHECK(rdohandle.record(std::move(trgContainer)));
    return StatusCode::SUCCESS;
  }
}
