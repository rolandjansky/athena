/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "NSWL1Simulation.h"

namespace NSWL1 {
  NSWL1Simulation::NSWL1Simulation( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator ),
      m_tree(nullptr),
      m_current_run(-1),
      m_current_evt(-1)
  {}


  StatusCode NSWL1Simulation::initialize() {
    ATH_MSG_DEBUG( "initialize " << name() );
    ATH_CHECK( m_trigRdoContainer.initialize() );
    // Create an register the ntuple if requested, add branch for event and run number
    if ( m_doNtuple ) {
      ITHistSvc* tHistSvc;
      ATH_CHECK(service("THistSvc", tHistSvc));
      m_current_evt = 0, m_current_run = 0;

      // create Ntuple and the branches
      std::string ntuple_name = name()+"Tree";
      m_tree = new TTree(ntuple_name.c_str(), "Ntuple of NSWL1Simulation");
      m_tree->Branch("runNumber",   &m_current_run, "runNumber/i");
      m_tree->Branch("eventNumber", &m_current_evt, "eventNumber/i");

      std::string tdir_name = "/"+name()+"/"+ntuple_name;
      ATH_CHECK(tHistSvc->regTree(tdir_name,m_tree));
    }

    // retrieving the private tools implementing the simulation
    if(m_dosTGC){
      ATH_CHECK(m_pad_tds.retrieve());
      if(m_useLookup){
        ATH_CHECK(m_pad_trigger_lookup.retrieve());
      }
      else{
        ATH_CHECK(m_pad_trigger.retrieve());
      }
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


  StatusCode NSWL1Simulation::start() {
    ATH_MSG_DEBUG("start " << name() );
    return StatusCode::SUCCESS;
  }


  StatusCode NSWL1Simulation::execute() {
    auto ctx = Gaudi::Hive::currentContext();
    m_current_evt = ctx.eventID().event_number();
    m_current_run = ctx.eventID().run_number();

    std::vector<std::shared_ptr<PadData>> pads;
    std::vector<std::unique_ptr<PadTrigger>> padTriggers;
    std::vector<std::unique_ptr<StripData>> strips;
    std::vector<std::unique_ptr<StripClusterData> > clusters;
    auto padTriggerContainer = std::make_unique<Muon::NSW_PadTriggerDataContainer>();
    auto stripTriggerContainer = std::make_unique<Muon::NSW_TrigRawDataContainer>();
    auto MMTriggerContainer = std::make_unique<Muon::NSW_TrigRawDataContainer>();

    if(m_dosTGC){
      ATH_CHECK( m_pad_tds->gather_pad_data(pads) );
      if(m_useLookup){
        ATH_CHECK( m_pad_trigger_lookup->lookup_pad_triggers(pads, padTriggers) );
      }
      else{
        ATH_CHECK( m_pad_trigger->compute_pad_triggers(pads, padTriggers) );
      }
      if(m_doStrip){
        ATH_CHECK( m_strip_tds->gather_strip_data(strips,padTriggers) );
        ATH_CHECK( m_strip_cluster->cluster_strip_data(strips,clusters) );
        ATH_CHECK( m_strip_segment->find_segments(clusters,stripTriggerContainer) );
      }
      ATH_CHECK(PadTriggerAdapter::fillContainer(padTriggerContainer, padTriggers, m_current_evt));
    }

    //retrive the MM Strip hit data
    if(m_doMM){
      ATH_CHECK( m_mmtrigger->runTrigger(MMTriggerContainer.get(), m_doMMDiamonds) );
    }
    if(m_doNtuple){
      if (m_tree) m_tree->Fill();
    }

    SG::WriteHandle<Muon::NSW_TrigRawDataContainer> rdohandle( m_trigRdoContainer );
    auto trgContainer=std::make_unique<Muon::NSW_TrigRawDataContainer>();
    ATH_CHECK( m_trigProcessor->mergeRDO(padTriggerContainer.get(), MMTriggerContainer.get(), trgContainer.get()) );
    ATH_CHECK(rdohandle.record(std::move(trgContainer)));
    return StatusCode::SUCCESS;
  }


  StatusCode NSWL1Simulation::finalize() {
    ATH_MSG_DEBUG( "finalize" << name() );
    return StatusCode::SUCCESS;
  }
}
