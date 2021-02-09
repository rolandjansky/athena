/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ISF_Algs includes
#include "SimEventFilter.h"
// FrameWork includes
#include "GaudiKernel/Property.h"
// McEventCollection
#include "GeneratorObjects/McEventCollection.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ISF::SimEventFilter::SimEventFilter( const std::string& name, ISvcLocator* pSvcLocator ) :
  ::AthFilterAlgorithm( name, pSvcLocator ),
  m_inputHardScatterEvgen(),
  m_genParticleCommonFilters(this),
  m_genParticleOldFilters(this),
  m_genParticleNewFilters(this)
{
  setFilterDescription("Filter to select events where two particle filter chains gave different selection results for at least one particle");

  declareProperty("InputHardScatterCollection",
                  m_inputHardScatterEvgen,
                  "Input Hard Scatter EVGEN collection.");

  // particle filters
  declareProperty("GenParticleCommonFilters",
                  m_genParticleCommonFilters,
                  "Tools for filtering out GenParticles for both selections.");
  declareProperty("GenParticleOldFilters",
                  m_genParticleOldFilters,
                  "Tools for filtering out GenParticles with the old selection.");
  declareProperty("GenParticleNewFilters",
                  m_genParticleNewFilters,
                  "Tools for filtering out GenParticles with the new selection.");

  declareProperty("InvertFilter",
                  m_invertfilter,
                  "Invert filter decision.");
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ISF::SimEventFilter::initialize()
{

  ATH_MSG_VERBOSE ( "--------------------------------------------------------" );
  ATH_MSG_INFO( "Initializing the ISF Sim Filter " );

  if (!m_genParticleCommonFilters.empty()) ATH_CHECK(m_genParticleCommonFilters.retrieve());
  if (!m_genParticleOldFilters.empty()   ) ATH_CHECK(m_genParticleOldFilters.retrieve());
  if (!m_genParticleNewFilters.empty()   ) ATH_CHECK(m_genParticleNewFilters.retrieve());

  // intialziation successful
  return StatusCode::SUCCESS;
}

StatusCode ISF::SimEventFilter::finalize()
{
  ATH_MSG_INFO ( "Finalizing ..." );

  //TODO: thread safe output of filter decisions

  ATH_MSG_INFO(" pass = "<<m_pass<<" / "<<m_total<<" = "<<(m_total>0 ? (100.0*m_pass)/m_total : 0)<<"%");
  ATH_MSG_INFO(" =====================================================================");

  return StatusCode::SUCCESS;
}

/** check if the given particle passes all filters */
bool ISF::SimEventFilter::passesFilters(const HepMC::GenParticle& part, ToolHandleArray<IGenParticleFilter>& filters) const
{
  // TODO: implement this as a std::find_if with a lambda function
  for ( const auto& filter : filters ) {
    // determine if the particle passes current filter
    bool passFilter = filter->pass(part);
    ATH_MSG_VERBOSE("Filter '" << filter.typeAndName() << "' returned: "
                    << (passFilter ? "true, will keep particle."
                        : "false, will remove particle."));

    if (!passFilter) return false;
  }

  return true;
}

StatusCode ISF::SimEventFilter::execute()
{
  ATH_MSG_DEBUG ("Executing ...");

  if (!m_inputHardScatterEvgen.isValid()) {
    ATH_MSG_FATAL("Unable to read input GenEvent collection '" << m_inputHardScatterEvgen.key() << "'");
    return StatusCode::FAILURE;
  }

  bool pass=false;

  for ( const auto& eventPtr : *m_inputHardScatterEvgen ) {
    // skip empty events
    if (eventPtr == nullptr) { continue; }

    ATH_MSG_DEBUG("Starting check of GenEvent with"
                  " signal_process_id=" << eventPtr->signal_process_id() <<
                  " and event_number=" << eventPtr->event_number() );

    auto allGenPartBegin = eventPtr->particles_begin();
    auto allGenPartEnd = eventPtr->particles_end();

    for(auto p = allGenPartBegin; p!= allGenPartEnd; ++p) {
      ATH_MSG_VERBOSE("Checking filters for particle: "<<**p);
      ATH_MSG_VERBOSE("Common filters:");
      bool b_common=passesFilters(**p,m_genParticleCommonFilters);
      ATH_MSG_VERBOSE("Old filters:");
      bool b_old   =passesFilters(**p,m_genParticleOldFilters);
      ATH_MSG_VERBOSE("New filters:");
      bool b_new   =passesFilters(**p,m_genParticleNewFilters);

      if( b_common && (b_old!=b_new) ) pass=true;
      if( b_common && m_genParticleOldFilters.empty() && m_genParticleNewFilters.empty()) pass=true;
      if( pass ) {
        ATH_MSG_INFO("Different result for particle "<<**p<<" common="<<b_common<<" old="<<b_old<<" new="<<b_new);
        if((*p)->production_vertex ()) ATH_MSG_INFO("  prod :"<<*((*p)->production_vertex ()));
        if((*p)->end_vertex ()) ATH_MSG_INFO("  decay:"<<*((*p)->end_vertex ()));

        for ( const auto& filter : m_genParticleCommonFilters ) {
          // determine if the particle passes current filter
          bool passFilter = filter->pass(**p);
          ATH_MSG_DEBUG("  GenParticleCommonFilter '" << filter.typeAndName() << "' returned: "
                          << (passFilter ? "true, will keep particle."
                              : "false, will remove particle."));
        }
        for ( const auto& filter : m_genParticleOldFilters ) {
          // determine if the particle passes current filter
          bool passFilter = filter->pass(**p);
          ATH_MSG_DEBUG("  GenParticleOldFilter '" << filter.typeAndName() << "' returned: "
                          << (passFilter ? "true, will keep particle."
                              : "false, will remove particle."));
        }
        for ( const auto& filter : m_genParticleNewFilters ) {
          // determine if the particle passes current filter
          bool passFilter = filter->pass(**p);
          ATH_MSG_DEBUG("  GenParticleNewFilter '" << filter.typeAndName() << "' returned: "
                          << (passFilter ? "true, will keep particle."
                              : "false, will remove particle."));
        }

        break;
      }
    }
  }

  if(pass) ++m_pass;
  ++m_total;

  ATH_MSG_INFO ("End SimEventFilter, difference in filters: "<<(pass ? "found" : "not found")<<"="<<pass<<", invert="<<m_invertfilter);
  if(m_invertfilter) pass=!pass;

  setFilterPassed(pass);

  return StatusCode::SUCCESS;
}
