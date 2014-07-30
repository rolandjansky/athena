/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IN4MLinkContainerFlagFillerTool.cxx 460048 2011-09-23 13:18:05Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "NavFourMom/INav4MomLinkContainer.h"

// Local include(s):
#include "IN4MLinkContainerFlagFillerTool.h"

namespace D3PD {

   IN4MLinkContainerFlagFillerTool::
   IN4MLinkContainerFlagFillerTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
      : BlockFillerTool< INavigable4Momentum >( type, name, parent ),
        m_getters( this ) {

      declareProperty( "FlagNames", m_flagNames,
                       "Names of the flags added to the D3PD" );
      declareProperty( "Getters", m_getters,
                       "Getter tools for the INav4MomLinkContainer objects" );
   }

   StatusCode IN4MLinkContainerFlagFillerTool::initialize() {

      // Retrieve the collection getter tool(s):
      CHECK( m_getters.retrieve() );

      // Check that the configuration makes sense:
      if( m_getters.size() != m_flagNames.size() ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Different number of getter tools and flags specified";
         return StatusCode::FAILURE;
      }

      // Check that all the getters will return the needed objects:
      for( ToolHandleArray< IObjGetterTool >::size_type i = 0;
           i < m_getters.size(); ++i ) {
         CHECK( m_getters[ i ]->configureD3PD< INav4MomLinkContainer >() );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode IN4MLinkContainerFlagFillerTool::book() {

      // Resize the flag vector to the right size:
      m_flags.resize( m_flagNames.size(), 0 );

      // Book the flag variable(s):
      for( size_t i = 0; i < m_flagNames.size(); ++i ) {
         CHECK( addVariable( m_flagNames[ i ], m_flags[ i ],
                             "Was this object selected according to X criteria?" ) );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode IN4MLinkContainerFlagFillerTool::fill( const INavigable4Momentum& p ) {

      // Check for each container if this object is in any of them:
      for( ToolHandleArray< IObjGetterTool >::size_type i = 0;
           i < m_getters.size(); ++i ) {

         // Reset the flag:
         *( m_flags[ i ] ) = false;

         // Try to access this container:
         const INav4MomLinkContainer* links =
            m_getters[ i ]->get< INav4MomLinkContainer >( true );
         if( ! links ) {
            ATH_MSG_WARNING( "Container missing for filling the '"
                             << m_flagNames[ i ] << "' flag" );
            continue;
         }

         // Loop over all elements in this link container:
         INav4MomLinkContainer::const_iterator itr = links->begin();
         INav4MomLinkContainer::const_iterator end = links->end();
         for( ; itr != end; ++itr ) {
            // Check that this is a valid element link:
            if( ! itr->isValid() ) {
               ATH_MSG_WARNING( "Container specified for the '"
                                << m_flagNames[ i ] << "' flag contains an "
                                << "invalid link!" );
               continue;
            }

            // Check if it points to the same object that we are investigating:
            const INavigable4Momentum* obj = *( itr->cptr() );
            if( obj == &p ) {
               *( m_flags[ i ] ) = true;
               break;
            }
         }
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
