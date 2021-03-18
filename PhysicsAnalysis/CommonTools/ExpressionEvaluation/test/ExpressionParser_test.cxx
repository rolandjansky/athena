/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "TestTools/initGaudi.h"
#include "GaudiKernel/EventContext.h"
#include "TestTools/FLOATassert.h"
#include "AthenaKernel/Units.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/CloneService.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SGHiveEventSlot.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>

// for populating conditions store
//#include "SGTools/TestStore.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/ReadDecorHandle.h"

#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

// expression = "(count(Muons.muonType == 0 && Muons.pt > 4*GeV && Muons.pt < 15*GeV) > 0)";
// expression = "(count(Muons.muonType == 0 && Muons.pt >= 15*GeV) > 0)"
// expression = "((count(Muons.muonType == 0 && Muons.pt > 25*GeV)  > 0) && (count(Muons.muonType == 0 && Muons.pt > 10*GeV)  > 1) )"
// expression = "(count(Muons.muonType == 0 && Muons.pt > 5*GeV) > 0)"
std::unique_ptr<EventContext> createEvent(unsigned int run_number, EventContext::ContextEvt_t evt, EventContext::ContextID_t id, unsigned int time_in_sec, IProxyDict &store) {
   EventIDBase event_id(run_number, evt, time_in_sec);
   std::unique_ptr<EventContext> ctx(std::make_unique<EventContext>(evt, id));
   ctx->setEventID( event_id );
   ctx->setExtension( Atlas::ExtendedEventContext(&store) );
   return ctx;
}


void fill( xAOD::TrackParticle& tp ) {

   tp.setDefiningParameters(std::rand()/(RAND_MAX/50.), //d0
                            std::rand()/(RAND_MAX/150.), //z0
                            std::rand()/(RAND_MAX/(2.*M_PI)), //phi
                            2*atan(exp(-(std::rand()/(RAND_MAX/(2.5+2.5))-2.5))), //theta
                            1/(std::rand()/(RAND_MAX/100e3))); // qoverp
   xAOD::ParametersCovMatrix_t covMatrix;
   covMatrix <<
      1.0, 2.0,  3.0,  4.0,  5.0,
      2.0, 4.0,  6.0,  8.0,  10.0,
      3.0, 6.0,  9.0,  12.0, 15.0,
      4.0, 8.0,  12.0, 16.0, 20.0,
      5.0, 10.0, 15.0, 20.0, 25.0;
   tp.setDefiningParametersCovMatrix( covMatrix );

   tp.setParametersOrigin( 0.0, 1.0, 2.0 );

   static const float parameters[ 2 ][ 6 ] = {
      { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 },
      { 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 }
   };
   std::vector< std::vector< float > > parametersVec;
   if( ! parametersVec.size() ) {
      for( int i = 0; i < 2; ++i ) {
         std::vector< float > temp( parameters[ i ],
                                    parameters[ i ] + 6 );
         parametersVec.push_back( temp );
      }
   }
   tp.setTrackParameters( parametersVec );

   tp.setParameterPosition( 0, xAOD::FirstMeasurement );
   tp.setParameterPosition( 1, xAOD::CalorimeterEntrance );

   return;
}
void createTrackParticles(EventContext &ctx, const SG::WriteHandleKey<xAOD::TrackParticleContainer> &key) {

   SG::WriteHandle<xAOD::TrackParticleContainer> out(key,ctx);
   assert( out.record( std::make_unique<xAOD::TrackParticleContainer>(), std::make_unique<xAOD::TrackParticleAuxContainer>()).isSuccess());

   unsigned int n=std::rand()%300;
   for(unsigned int i=0; i<n; ++i) {
      // Add one track particle to the container:
      out->push_back( new xAOD::TrackParticle );
      fill( *(out->back()));
   }
}
void decorateTrackParticles(EventContext &ctx,
                            const SG::ReadHandleKey<xAOD::TrackParticleContainer> &key,
                            const SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> &decor_key) {

   SG::ReadHandle<xAOD::TrackParticleContainer> track_particles(key,ctx);
   assert( track_particles.isValid() );
   SG::WriteDecorHandle<xAOD::TrackParticleContainer,float> decor(decor_key,ctx);
   assert( decor.isValid());
   for (const xAOD::TrackParticle *particle : *track_particles) {
      float val = std::rand()/(RAND_MAX/1.1);
      decor(*particle)=(val > 1. ? -1. : val);
   }
}

void fill( xAOD::Muon& muon ) {
   double pt=(std::rand()/(RAND_MAX/(100e3-2.5e3)))+2.5e3; // p
   double phi=std::rand()/(RAND_MAX/(2.*M_PI)); //phi
   double eta=std::rand()/(RAND_MAX/(2.7+2.7))-2.7; //eta
   muon.setP4( pt,eta,phi );
   muon.setAuthor(xAOD::Muon::MuidCo);
   muon.setAllAuthors(0x10);
   muon.setMuonType(xAOD::Muon::Combined);
   float value=1.0;
   muon.setParameter(value, xAOD::Muon::spectrometerFieldIntegral);
   muon.setParameter(value, xAOD::Muon::momentumBalanceSignificance);
   muon.setQuality(xAOD::Muon::Medium);
}

void createMuons(EventContext &ctx, const SG::WriteHandleKey<xAOD::MuonContainer> &key) {

   SG::WriteHandle<xAOD::MuonContainer> out(key,ctx);
   assert( out.record( std::make_unique<xAOD::MuonContainer>(), std::make_unique<xAOD::MuonAuxContainer>()).isSuccess());

   if (ctx.slot()>0) {
   unsigned int n=std::rand()%20;
   for(unsigned int i=0; i<n; ++i) {
      // Add one track particle to the container:
      out->push_back( new xAOD::Muon );
      fill( *(out->back()));
   }
   }
}

void decorateMuons(EventContext &ctx,
                   const SG::ReadHandleKey<xAOD::MuonContainer> &key,
                   const SG::WriteDecorHandleKey<xAOD::MuonContainer> &decor_key) {

   SG::ReadHandle<xAOD::MuonContainer> muons(key,ctx);
   assert( muons.isValid() );
   SG::WriteDecorHandle<xAOD::MuonContainer,bool> decor(decor_key,ctx);
   assert( decor.isValid());
   for (const xAOD::Muon *a_muon : *muons) {
      float val = std::rand()/(RAND_MAX/1.);
      decor(*a_muon)=(val > 0.5 ? true : false);
   }
}

class DummyParent : public ExpressionParsing::SGxAODProxyLoader::IParentHelper
{
public:
   virtual StatusCode declare(SG::VarHandleKey &handle) const override {
      return handle.initialize();
   }

};

template <class T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &a) {
   for (const auto &elm : a) {
      std::cout << " " << elm;
   }
   return out;
}

class Task
{
public:
   Task(const EventContext &event,
        SG::HiveEventSlot &store,
        const ExpressionParsing::ExpressionParser &parser,
        const std::vector<int> &ref,
        bool verbose=false)
      : m_event(&event),
        m_store(&store),
        m_parser(&parser),
        m_ref(&ref),
        m_verbose(verbose)
   {}

   Task(const EventContext &event,
        SG::HiveEventSlot &store,
        const ExpressionParsing::ExpressionParser &parser,
        bool ref,
        bool verbose=false)
      : m_event(&event),
        m_store(&store),
        m_parser(&parser),
        m_refPassed(ref),
        m_isBool(true),
        m_verbose(verbose)
   {}

   void operator()() {
      StoreGateSvc::setSlot(m_store);
      Gaudi::Hive::setCurrentContext(*m_event);
      if (!m_isBool) {
         std::vector<int> entries =  m_parser->evaluateAsVector();
         if (m_verbose) {
            std::cout << "evaluateAsVector: " << entries << std::endl;
         }
         assert( entries == *m_ref );
      }
      else {
         bool passed =  m_parser->evaluateAsBool();
         if (m_verbose) {
            std::cout << "evaluateAsVector: " << passed << " / " << m_refPassed<< std::endl;
         }
         assert( passed == m_refPassed );
      }
   }
   void setVerbose(bool verbose) {m_verbose=verbose; }
   const EventContext                        *m_event;
   SG::HiveEventSlot                         *m_store;
   const ExpressionParsing::ExpressionParser *m_parser;
   const std::vector<int>                    *m_ref;
   bool                                       m_refPassed;
   bool                                       m_isBool=false;
   bool                                       m_verbose;
};

int main(int argc, char **argv)
{
  bool verbose=false;
  unsigned int n_events=100;
  unsigned int n_threads=1;
  const char *n_proc_str=getenv("ATHENA_NUM_PROC");
  if (n_proc_str) {
     n_threads=atoi(n_proc_str);
  }
  for (int arg_i=1; arg_i<argc; ++arg_i) {
     if      (strcmp(argv[arg_i],"-v")==0) verbose=true;
     else if ((strcmp(argv[arg_i],"-n")==0 || strcmp(argv[arg_i],"--events")==0) && arg_i+1<argc) {
        n_events=atoi(argv[++arg_i]);
     }
     else if ((strcmp(argv[arg_i],"-t")==0 || strcmp(argv[arg_i],"--threads")==0) && arg_i+1<argc) {
        n_threads=atoi(argv[++arg_i]);
     }
     else {
        std::cerr << "ERROR: unsupported argument " << argv[arg_i] << std::endl;
        return 1;
     }
  }



  std::cout << "ExpressionParser_test\n";
  //  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  //  Athena_test::initGaudi ("ExpressionParser_test.txt", svcloc);
  Athena_test::initGaudi ("", svcloc);
  StoreGateSvc *storegate_svc;
  assert( svcloc->service("StoreGateSvc", storegate_svc).isSuccess());
  //  SGTest::TestStore dumstore;
  unsigned int run_number=100000;
  EventContext::ContextEvt_t event_id=1;
  EventContext::ContextID_t slot_id=0;
  unsigned int time_in_sec=1590583868;

  // track particle keys
  SG::WriteHandleKey<xAOD::TrackParticleContainer> track_particle_key("InDetTrackParticles");
  assert( track_particle_key.initialize().isSuccess());

  SG::ReadHandleKey<xAOD::TrackParticleContainer> read_track_particle_key(track_particle_key.key());
  assert( read_track_particle_key.initialize().isSuccess());
  SG::ReadHandleKey<SG::AuxVectorBase> read_key(track_particle_key.key());
  assert( read_key.initialize().isSuccess());

  SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> pid_track_particle_decor_key("InDetTrackParticles.PID");
  assert( pid_track_particle_decor_key.initialize().isSuccess());
  SG::ReadDecorHandleKey<xAOD::TrackParticleContainer> read_pid_track_particle_decor_key(pid_track_particle_decor_key.key());
  assert( read_pid_track_particle_decor_key.initialize().isSuccess());

  // muon keys
  SG::WriteHandleKey<xAOD::MuonContainer> muon_key("Muons");
  assert( muon_key.initialize().isSuccess());

  SG::ReadHandleKey<xAOD::MuonContainer> read_muon_key(muon_key.key());
  assert( read_muon_key.initialize().isSuccess());
  SG::WriteDecorHandleKey<xAOD::MuonContainer> is_tight_muon_decor_key("Muons.isTight");
  assert( is_tight_muon_decor_key.initialize().isSuccess());
  SG::ReadDecorHandleKey<xAOD::MuonContainer> read_is_tight_muon_decor_key(is_tight_muon_decor_key.key());
  assert( read_is_tight_muon_decor_key.initialize().isSuccess());

  // all handles
  std::vector<const DataObjID *> input_data;
  std::vector<const DataObjID *> output_data;
  output_data.push_back( &track_particle_key.fullKey() );
  output_data.push_back( &pid_track_particle_decor_key.fullKey() );
  output_data.push_back( &muon_key.fullKey() );
  output_data.push_back( &is_tight_muon_decor_key.fullKey() );
  std::vector<Gaudi::DataHandle *> new_input_handles;
  std::vector<Gaudi::DataHandle *> new_output_handles;

  DummyParent dummy_parent;

  // track particle expression
  //  ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
  ServiceHandle<StoreGateSvc> unused("StoreGateSvc","");
  ExpressionParsing::SGxAODProxyLoader *xAODProxyLoader=new ExpressionParsing::SGxAODProxyLoader(unused);
  //  proxyLoaders->push_back(xAODProxyLoader);

  std::unique_ptr<ExpressionParsing::ExpressionParser> parser( std::make_unique<ExpressionParsing::ExpressionParser>(xAODProxyLoader));
  parser->loadExpression("InDetTrackParticles.pt>10*GeV && abs(InDetTrackParticles.z0)<100*mm && InDetTrackParticles.PID>0.");
  xAODProxyLoader->updateDataDependencies(dummy_parent,
                                          parser->getVariables(),
                                          input_data,
                                          output_data,
                                          new_input_handles,
                                          new_output_handles);

  // muon expression
  ExpressionParsing::SGxAODProxyLoader *muon_xAODProxyLoader=new ExpressionParsing::SGxAODProxyLoader(unused);
  std::unique_ptr<ExpressionParsing::ExpressionParser> muon_parser( std::make_unique<ExpressionParsing::ExpressionParser>(muon_xAODProxyLoader));
  muon_parser->loadExpression("Muons.pt>25*GeV && abs(Muons.eta)<2.5 && Muons.isTight");
  muon_xAODProxyLoader->updateDataDependencies(dummy_parent,
                                               muon_parser->getVariables(),
                                               input_data,
                                               output_data,
                                               new_input_handles,
                                               new_output_handles);

  ExpressionParsing::SGxAODProxyLoader *event_xAODProxyLoader=new ExpressionParsing::SGxAODProxyLoader(unused);
  std::unique_ptr<ExpressionParsing::ExpressionParser> event_parser( std::make_unique<ExpressionParsing::ExpressionParser>(event_xAODProxyLoader));
  event_parser->loadExpression("count(InDetTrackParticles.pt>10*GeV && abs(InDetTrackParticles.z0)<100*mm && InDetTrackParticles.PID>0.)>50"
                               "&& count(Muons.pt>25*GeV && abs(Muons.eta)<2.5 && Muons.isTight)>=2"
                               " && !(count(Muons.pt>25*GeV && abs(Muons.eta)<2.5 && Muons.isTight)+(-(count(Muons.pt>25*GeV && abs(Muons.eta)<2.5 && Muons.isTight)/2)*2)==0)");
  event_xAODProxyLoader->updateDataDependencies(dummy_parent,
                                               event_parser->getVariables(),
                                               input_data,
                                               output_data,
                                               new_input_handles,
                                               new_output_handles);

  // prepare StoreGates
  std::vector<SG::HiveEventSlot> event_slots;
  event_slots.reserve(n_events);
  for (unsigned int i=0; i<n_events ; ++i) {
     std::stringstream name;
     name << storegate_svc->name() << "_slot" << i;
     Service *a_service;
     SGImplSvc*sg_current=dynamic_cast<SGImplSvc*>(storegate_svc->hiveProxyDict());
     assert( sg_current);
     assert( CloneService::clone(sg_current, name.str(), a_service).isSuccess());
     SGImplSvc*sg_clone=dynamic_cast<SGImplSvc*>(a_service);
     assert(sg_clone);
     sg_clone->setSlotNumber(i,n_events);
     event_slots.push_back(SG::HiveEventSlot(sg_clone));
  }
  std::vector<std::unique_ptr<EventContext> > events;
  events.reserve(n_events);
  std::vector<std::vector<int> > should_entries;
  should_entries.reserve(n_events);
  std::vector<std::vector<int> > muon_should_entries;
  muon_should_entries.reserve(n_events);
  std::vector<bool> events_passed;
  events_passed.reserve(n_events);

  // create events
  for (unsigned int i=0; i<n_events; ++i) {
     events.push_back( createEvent(run_number, event_id, slot_id, time_in_sec,*(event_slots.at(i).pEvtStore) ) );
     EventContext *ctx=events.back().get();
     StoreGateSvc::setSlot(&event_slots[ctx->slot()]);
     Gaudi::Hive::setCurrentContext(*ctx);
     createTrackParticles(*ctx,track_particle_key);
     decorateTrackParticles(*ctx,read_track_particle_key, pid_track_particle_decor_key);
     createMuons(*ctx,muon_key);
     decorateMuons(*ctx,read_muon_key, is_tight_muon_decor_key);

     // reference for track particle condition
     std::size_t tp_sum=0;
     std::size_t muon_sum=0;
     {
        SG::ReadHandle<xAOD::TrackParticleContainer> track_particles(read_track_particle_key,*ctx);
        assert( track_particles.isValid() );
        SG::ReadDecorHandle<xAOD::TrackParticleContainer,float> tp_pid(read_pid_track_particle_decor_key,*ctx);
        assert( tp_pid.isValid());

        //        std::cout << "DEBUG " << read_track_particle_key.key() << " has " << track_particles->size() << " elements." << std::endl;
        should_entries.push_back(std::vector<int>());
        should_entries.back().reserve(track_particles->size());
        for (const xAOD::TrackParticle *particle : *track_particles) {
           should_entries.back().push_back( ((particle->pt()>10e3 && std::abs(particle->z0())<100 && tp_pid(*particle)>0. ) ? 1 : 0));
        }
        tp_sum = std::accumulate(should_entries.back().begin(), should_entries.back().end(),
                                 static_cast<std::size_t>(0),
                                 [](std::size_t sum, int val) -> std::size_t { return sum + (val>0 ? 1 : 0);} );
        if (verbose) {
           std::cout << "DEBUG event " << i << " tp:" << tp_sum << std::endl;
        }
        //        std::cout << "Should entries: " << should_entries << std::endl;

        // make sure that retrieval via base class works :
        {
           SG::ReadHandle<SG::AuxVectorBase> tmp(read_key,*ctx);
           assert( tmp.isValid() );
           //  std::cout << "DEBUG " << read_key.key() << " has " << tmp->size_v() << " elements." << std::endl;
        }
     }

     // reference for muons
     {
        SG::ReadHandle<xAOD::MuonContainer> muons(read_muon_key,*ctx);
        assert( muons.isValid() );
        SG::ReadDecorHandle<xAOD::MuonContainer,bool> is_tight_muon(read_is_tight_muon_decor_key,*ctx);
        assert( is_tight_muon.isValid());

        muon_should_entries.push_back(std::vector<int>());
        muon_should_entries.back().reserve(muons->size());
        for (const xAOD::Muon *a_muon : *muons) {
           muon_should_entries.back().push_back( ((a_muon->pt()>25e3 && abs(a_muon->eta())<2.5 && is_tight_muon(*a_muon) ) ? 1 : 0));
        }
        muon_sum = std::accumulate(muon_should_entries.back().begin(), muon_should_entries.back().end(),
                                               static_cast<std::size_t>(0),
                                               [](std::size_t sum, int val) -> std::size_t { return sum + (val>0 ? 1 : 0);} );
        if (verbose) {
           std::cout << "DEBUG event " << i << " muon:" << muon_sum << std::endl;
        }
     }
     events_passed.push_back( tp_sum > 50 && muon_sum>=2 && muon_sum - (muon_sum/2)*2==1);
     if (verbose) {
     std::cout << "DEBUG event " << i << " event passed:" << events_passed.back()
               << " even:" << (muon_sum/2)*2  << " vs " << muon_sum
               << std::endl;
     }
     ++slot_id;
     ++event_id;
     ++time_in_sec;
  }

  std::vector<Task> tasks;
  tasks.reserve(events.size()*3);
  for (const std::unique_ptr<EventContext> &ctx : events) {
     tasks.emplace_back( *ctx, event_slots.at(ctx->slot()), *parser,      should_entries.at(ctx->slot()));
     tasks.emplace_back( *ctx, event_slots.at(ctx->slot()), *muon_parser, muon_should_entries.at(ctx->slot()));
     tasks.emplace_back( *ctx, event_slots.at(ctx->slot()), *event_parser, events_passed.at(ctx->slot()));
  }

  for (Task &a_task : tasks) {
     a_task.setVerbose(verbose);
     a_task();
     a_task.setVerbose(false);
  }

  std::atomic<unsigned int> next_task=0;
  std::vector<std::thread> pool;
  std::cout << "Process with " << n_threads << " threads." << std::endl;
  auto executor = [&tasks,&next_task]() {
                     for(;;) {
                        unsigned int my_task = next_task.fetch_add(1, std::memory_order_relaxed);
                        if (my_task>=tasks.size()) break;
                        for (unsigned int pass_i=0; pass_i<200; ++pass_i) {
                        tasks.at(my_task)();
                        }
                     }
                  };
  pool.reserve(n_threads);
  for (unsigned int i=0; i<n_threads; ++i) {
     pool.emplace_back(executor);
  }
  for (std::thread &a_thread :pool) {
     a_thread.join();
  }
  std::cout << "all done." << std::endl;

  return 0;
}
