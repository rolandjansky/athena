/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenEventGetterTool.cxx 452268 2011-08-04 20:27:55Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AtlasHepMC/GenEvent.h"

// Local include(s):
#include "GenEventGetterTool.h"

namespace D3PD {

   GenEventGetterTool::GenEventGetterTool( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
      : Base( type, name, parent ),
        m_mcColl(0),
        m_selector( "GenObjectsFilterTool" )
   {

      declareProperty( "Selector", m_selector,
                       "Handle for the selector tool to be used" );
   }

   StatusCode GenEventGetterTool::initialize() {

      // Initialize the base class:
      CHECK( Base::initialize() );
      // Retrieve the selector tool:
      CHECK( m_selector.retrieve() );

      return StatusCode::SUCCESS;
   }

   size_t GenEventGetterTool::sizeHint( bool allowMissing ) {

      const McEventCollection* mc = get( allowMissing );
      if( ! mc ) {
         return 0;
      }

      return mc->size();
   }

   StatusCode GenEventGetterTool::reset( bool allowMissing ) {

      // Retrieve the collection from SG:
      m_mcColl = get( allowMissing );
 
      // Handle the case when there is no collection in SG:
      if( ! m_mcColl ) {
         m_evtItr = m_evtEnd;
         if( allowMissing )return StatusCode::SUCCESS;
         return StatusCode::FAILURE;
      }

      // Initialize the iterators:
      m_evtItr = m_mcColl->begin();
      m_evtEnd = m_mcColl->end();

      return StatusCode::SUCCESS;
   }

   const void* GenEventGetterTool::nextUntyped() {

      // Check if we've reached the end of the container:
      if( m_evtItr == m_evtEnd ) return 0;

      // Remember the current GenEvent, and advance the iterator:
      const HepMC::GenEvent* evt = *m_evtItr;
      ++m_evtItr;

      // If this is not a "good" GenEvent, then try the next one:
      if( ! m_selector->pass( evt, m_mcColl ) ) return nextUntyped();

      // This is a "good" GenEvent:
      return evt;
   }

} // namespace D3PD
