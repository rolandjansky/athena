/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ChainEntryFillerTool.cxx 504000 2012-06-05 16:28:50Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "ChainEntryFillerTool.h"

namespace D3PD {

   ChainEntryFillerTool::ChainEntryFillerTool( const std::string& type, 
                                               const std::string& name, 
                                               const IInterface* parent )
      : BlockFillerTool< ChainEntry >( type, name, parent )
   {
      book().ignore(); // Avoid coverity warnings.
   }

   StatusCode ChainEntryFillerTool::book() {

      // Book the variables:
      CHECK( addVariable( "ChainId",  m_chainId ) );
      CHECK( addVariable( "RoIType",  m_roiType ) );
      CHECK( addVariable( "RoIIndex", m_roiIndex ) );

      return StatusCode::SUCCESS;
   }

   StatusCode ChainEntryFillerTool::fill( const ChainEntry& x ) {

      // Extract the information from the object:
      const std::vector< int >& types   = x.getRoITypes();
      const std::vector< int >& indices = x.getRoIIndices();
      size_t n = types.size();
      static const chainId_t ef_offset = 10000;

      // Set the chain_id of this chain. Notice that the ID of the EF
      // chains is incremented by 10k to be able to differentiate between
      // the LVL2 and EF chains in the ntuple.
      *m_chainId = static_cast< chainId_t >( x.getCounter() );
      if ( ( ( *m_chainId ) >= 0 ) && ( x.getName().find( "EF_" ) == 0 ) ) {
         *m_chainId += ef_offset;
      }

      // Set the RoI variables:
      m_roiType->clear(); m_roiIndex->clear();
      for( size_t i = 0; i < n; ++i ) {
         m_roiType->push_back( static_cast< index_t::value_type >( types[ i ] ) );
         m_roiIndex->push_back( static_cast< index_t::value_type >( indices[ i ] ) );
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
