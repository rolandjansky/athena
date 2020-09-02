/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenVertexGetterTool.cxx 452268 2011-08-04 20:27:55Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AtlasHepMC/GenVertex.h"

// Local include(s):
#include "GenVertexGetterTool.h"

namespace D3PD {

   GenVertexGetterTool::GenVertexGetterTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
      : Base( type, name, parent ),
        m_mcColl(0),
        m_selector( "GenObjectsFilterTool" )
   {

      declareProperty( "Selector", m_selector,
                       "Handle for the selector tool to be used" );
   }

   StatusCode GenVertexGetterTool::initialize() {

      // Initialize the base class:
      CHECK( Base::initialize() );
      // Retrieve the selector tool:
      CHECK( m_selector.retrieve() );

      return StatusCode::SUCCESS;
   }

   size_t GenVertexGetterTool::sizeHint( bool allowMissing ) {

      const McEventCollection* mc = get( allowMissing );
      if( ! mc ) {
         return 0;
      }

      McEventCollection::const_iterator iter = mc->begin();
      if( iter == mc->end() ) return 0;

      return ( *iter )->vertices_size();
   }

   StatusCode GenVertexGetterTool::reset( bool allowMissing ) {

      m_mcColl = get( allowMissing );

      if( ! m_mcColl ) {
         m_evtItr = m_evtEnd;
         m_vtxItr = m_vtxEnd;
         if( allowMissing ) return StatusCode::SUCCESS;
         return StatusCode::FAILURE;
      }

      m_evtItr = m_mcColl->begin();
      m_evtEnd = m_mcColl->end();

      if( m_evtItr == m_evtEnd ){
         m_vtxItr = m_vtxEnd;
         return StatusCode::SUCCESS;
      }

      m_vtxItr = ( *m_evtItr )->vertices_begin();
      m_vtxEnd = ( *m_evtItr )->vertices_end();

      return StatusCode::SUCCESS;
   }

   const void* GenVertexGetterTool::nextUntyped() {

      if( m_evtItr == m_evtEnd ) return 0;

      // Check if this GenEvent passes our selection cuts:
      if( ! m_selector->pass( *m_evtItr, m_mcColl ) ) {
         ++m_evtItr;
	 if( m_evtItr == m_evtEnd ) return 0;
         m_vtxItr = ( *m_evtItr )->vertices_begin();
         m_vtxEnd = ( *m_evtItr )->vertices_end();
         return nextUntyped();
      }

      // Check if there are no more vertices in this GenEvent:
      if( m_vtxEnd ==  m_vtxItr ){
         ++m_evtItr;
         if( m_evtItr == m_evtEnd ) return 0;

         m_vtxItr = ( *m_evtItr )->vertices_begin();
         m_vtxEnd = ( *m_evtItr )->vertices_end();
         return nextUntyped();
      }

      if( ! m_selector->pass( *m_vtxItr, m_mcColl ) ) {
         ++m_vtxItr;
         return nextUntyped();
      }

      // I just like to write this part our verbosely...
      HepMC::GenVertex* vtx = *m_vtxItr;
      ++m_vtxItr;

      return vtx;
   }

   const std::type_info& GenVertexGetterTool::elementTypeinfo() const {

      return typeid( HepMC::GenVertex );
   }

} // namespace D3PD
