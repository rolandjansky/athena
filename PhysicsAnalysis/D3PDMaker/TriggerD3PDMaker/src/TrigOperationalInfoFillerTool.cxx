/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigOperationalInfoFillerTool.cxx 504000 2012-06-05 16:28:50Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigOperationalInfoFillerTool.h"

namespace D3PD {

   TrigOperationalInfoFillerTool::TrigOperationalInfoFillerTool( const std::string& type,
                                                                 const std::string& name,
                                                                 const IInterface* parent )
      : BlockFillerTool< TrigOperationalInfoCollection >( type, name, parent )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode TrigOperationalInfoFillerTool::book() {

      CHECK( addVariable( "chains", m_chains,
                          "Trigger chains responsible for putting the event in the "
                          "express stream" ) );

      return StatusCode::SUCCESS;
   }

   StatusCode TrigOperationalInfoFillerTool::fill( const TrigOperationalInfoCollection& info ) {

      // First of all, clear the vector:
      m_chains->clear();

      //
      // Loop over all TrigOperationalInfo objects:
      //
      TrigOperationalInfoCollection::const_iterator coll_itr = info.begin();
      TrigOperationalInfoCollection::const_iterator coll_end = info.end();
      for( ; coll_itr != coll_end; ++coll_itr ) {

         //
         // Loop over all chain names:
         //
         const std::pair< std::vector< std::string >, std::vector< float > > chain_infos =
            ( *coll_itr )->infos();
         std::vector< std::string >::const_iterator chain_itr = chain_infos.first.begin();
         std::vector< std::string >::const_iterator chain_end = chain_infos.first.end();
         for( ; chain_itr != chain_end; ++chain_itr ) {

            // Remember this chain name:
            m_chains->push_back( *chain_itr );

         }

      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
