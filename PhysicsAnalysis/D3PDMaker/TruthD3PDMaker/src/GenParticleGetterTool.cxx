/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenParticleGetterTool.cxx 452268 2011-08-04 20:27:55Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AtlasHepMC/GenParticle.h"

// Local include(s):
#include "GenParticleGetterTool.h"

namespace D3PD {

   GenParticleGetterTool::GenParticleGetterTool( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
      : Base( type, name, parent ),
        m_mcColl(0),
        m_selector( "GenObjectsFilterTool" )
   {

      declareProperty( "Selector", m_selector,
                       "Handle for the selector tool to be used" );
   }

   StatusCode GenParticleGetterTool::initialize() {

      // Initialize the base class:
      CHECK( Base::initialize() );
      // Retrieve the selector tool:
      CHECK( m_selector.retrieve() );

      return StatusCode::SUCCESS;
   }

   size_t GenParticleGetterTool::sizeHint( bool allowMissing ) {

      const McEventCollection* mc = get( allowMissing );
      if( ! mc ) {
         return 0;
      }

      McEventCollection::const_iterator iter = mc->begin();
      if( iter == mc->end() ) return 0;

      return ( *iter )->particles_size();
   }

   StatusCode GenParticleGetterTool::reset( bool allowMissing ) {

      m_mcColl = get( allowMissing );

      if( ! m_mcColl ) {
         m_evtItr = m_evtEnd;
         m_partItr = m_partEnd;
         if( allowMissing ) return StatusCode::SUCCESS;
         return StatusCode::FAILURE;
      }

      m_evtItr = m_mcColl->begin();
      m_evtEnd = m_mcColl->end();

      if( m_evtItr == m_evtEnd ){
         m_partItr = m_partEnd;
         return StatusCode::SUCCESS;
      }

      m_partItr = ( *m_evtItr )->particles_begin();
      m_partEnd = ( *m_evtItr )->particles_end();

      return StatusCode::SUCCESS;
   }

   const void* GenParticleGetterTool::nextUntyped() {
  
      if( m_evtItr == m_evtEnd ) return 0;

      // Check if this GenEvent passes our selection cuts:
      if( ! m_selector->pass( *m_evtItr, m_mcColl ) ) {
         ++m_evtItr;
	 if( m_evtItr == m_evtEnd ) return 0;
         m_partItr = ( *m_evtItr )->particles_begin();
         m_partEnd = ( *m_evtItr )->particles_end();
         return nextUntyped();
      }

      // Check if there are no more particles in this GenEvent:
      if( m_partItr == m_partEnd ) {
         ++m_evtItr;
         if( m_evtItr == m_evtEnd ) return 0;

         m_partItr = ( *m_evtItr )->particles_begin();
         m_partEnd = ( *m_evtItr )->particles_end();
         return nextUntyped();
      }

      // Check if this GenParticle passes our selection:
      if( ! m_selector->pass( *m_partItr, m_mcColl ) ) {
         ++m_partItr;
         return nextUntyped();
      }

      // I just like to write this part our verbosely...
      HepMC::GenParticle* part = *m_partItr;
      ++m_partItr;

      return part;
   }

   const std::type_info& GenParticleGetterTool::elementTypeinfo() const {

      return typeid( HepMC::GenParticle );
   }

} // namespace D3PD
