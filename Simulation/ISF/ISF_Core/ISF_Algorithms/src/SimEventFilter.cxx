/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMULATIONBASE
// ISF_Algs includes
#include "SimEventFilter.h"
// FrameWork includes
#include "Gaudi/Property.h"
// McEventCollection
#include "GeneratorObjects/McEventCollection.h"
//
#include "EventBookkeeperTools/FilterReporter.h"


///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ISF::SimEventFilter::SimEventFilter( const std::string& name, ISvcLocator* pSvcLocator ) :
  ::AthReentrantAlgorithm( name, pSvcLocator )
{
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ISF::SimEventFilter::initialize()
{
  ATH_CHECK(m_filterParams.initialize(false));
  ATH_MSG_VERBOSE ( "--------------------------------------------------------" );
  ATH_MSG_VERBOSE ( "Initializing the ISF Sim Filter " );

  if (!m_genParticleCommonFilters.empty()) ATH_CHECK(m_genParticleCommonFilters.retrieve());
  if (!m_genParticleOldFilters.empty()   ) ATH_CHECK(m_genParticleOldFilters.retrieve());
  if (!m_genParticleNewFilters.empty()   ) ATH_CHECK(m_genParticleNewFilters.retrieve());

  ATH_CHECK( m_inputHardScatterEvgenKey.initialize() );

  // intialziation successful
  return StatusCode::SUCCESS;
}

StatusCode ISF::SimEventFilter::finalize()
{
  ATH_MSG_VERBOSE ( "Finalizing ..." );

  ATH_MSG_INFO(m_filterParams.summary());
  ATH_MSG_VERBOSE(" =====================================================================");

  return StatusCode::SUCCESS;
}

/** check if the given particle passes all filters */
bool ISF::SimEventFilter::passesFilters(HepMC::ConstGenParticlePtr part, const ToolHandleArray<IGenParticleFilter>& filters) const
{
  // TODO: implement this as a std::find_if with a lambda function
  for ( const auto& filter : filters ) {
    // determine if the particle passes current filter
#ifdef HEPMC3
    bool passFilter = filter->pass(part);
#else
    bool passFilter = filter->pass(*part);
#endif
    ATH_MSG_VERBOSE("Filter '" << filter.typeAndName() << "' returned: "
                    << (passFilter ? "true, will keep particle."
                        : "false, will remove particle."));

    if (!passFilter) return false;
  }

  return true;
}

StatusCode ISF::SimEventFilter::execute(const EventContext &ctx) const
{
  ATH_MSG_DEBUG ("Executing ...");

  FilterReporter filter(m_filterParams, false, ctx);
  SG::ReadHandle<McEventCollection> inputHardScatterEvgen(m_inputHardScatterEvgenKey, ctx);
  if (!inputHardScatterEvgen.isValid()) {
    ATH_MSG_FATAL("Unable to read input GenEvent collection '" << inputHardScatterEvgen.key() << "'");
    return StatusCode::FAILURE;
  }

  bool pass = false;

  for ( const HepMC::GenEvent* eventPtr : *inputHardScatterEvgen ) {
    // skip empty events
    if (eventPtr == nullptr) { continue; }

    ATH_MSG_DEBUG("Starting check of GenEvent with"
                  " signal_process_id=" << HepMC::signal_process_id(eventPtr) <<
                  " and event_number=" << eventPtr->event_number() );

    for (auto p : *eventPtr) {
      ATH_MSG_VERBOSE("Checking filters for particle: "<< p);
      ATH_MSG_VERBOSE("Common filters:");
      bool b_common = passesFilters(p,m_genParticleCommonFilters);
      ATH_MSG_VERBOSE("Old filters:");
      bool b_old   = passesFilters(p,m_genParticleOldFilters);
      ATH_MSG_VERBOSE("New filters:");
      bool b_new   = passesFilters(p,m_genParticleNewFilters);

      if ( b_common && (b_old!=b_new) ) {
        pass=true;
      }
      if ( b_common && m_genParticleOldFilters.empty() && m_genParticleNewFilters.empty()) {
        pass=true;
      }
      if ( pass ) {
        ATH_MSG_DEBUG("Different result for particle "<<p<<" common="<<b_common<<" old="<<b_old<<" new="<<b_new);
        if (p->production_vertex ()) {
          ATH_MSG_VERBOSE("  prod :"<<p->production_vertex());
        }
        if (p->end_vertex ()) {
          ATH_MSG_VERBOSE("  decay:"<<p->end_vertex());
        }

        for ( const auto& filter : m_genParticleCommonFilters ) {
          // determine if the particle passes current filter
#ifdef HEPMC3
          bool passFilter = filter->pass(p);
#else
          bool passFilter = filter->pass(*p);
#endif
          ATH_MSG_DEBUG("  GenParticleCommonFilter '" << filter.typeAndName() << "' returned: "
                          << (passFilter ? "true, will keep particle."
                              : "false, will remove particle."));
        }
        for ( const auto& filter : m_genParticleOldFilters ) {
          // determine if the particle passes current filter
#ifdef HEPMC3
          bool passFilter = filter->pass(p);
#else
          bool passFilter = filter->pass(*p);
#endif
          ATH_MSG_DEBUG("  GenParticleOldFilter '" << filter.typeAndName() << "' returned: "
                          << (passFilter ? "true, will keep particle."
                              : "false, will remove particle."));
        }
        for ( const auto& filter : m_genParticleNewFilters ) {
          // determine if the particle passes current filter
#ifdef HEPMC3
          bool passFilter = filter->pass(p);
#else
          bool passFilter = filter->pass(*p);
#endif
          ATH_MSG_DEBUG("  GenParticleNewFilter '" << filter.typeAndName() << "' returned: "
                          << (passFilter ? "true, will keep particle."
                              : "false, will remove particle."));
        }

        break;
      }
    }
  }

  ATH_MSG_DEBUG ("End SimEventFilter, difference in filters: "<<(pass ? "found" : "not found")<<"="<<pass<<", invert="<<m_invertfilter);

  if (m_invertfilter) {
    pass =! pass;
  }

  filter.setPassed(pass);

  return StatusCode::SUCCESS;
}
#endif // SimEventFilter currently will not compile in the AthSimulation Project
