/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonSelectionTool.cxx 299883 2014-03-28 17:34:16Z krasznaa $

// Local include(s):
#include "CPAnalysisExamples/MuonSelectionToolExample.h"

namespace CP {

   MuonSelectionToolExample::MuonSelectionToolExample( const std::string& name )
      : asg::AsgTool( name ),
        m_accept( "MuonSelection" ) {

      declareProperty( "MaxEta", m_maxEta = 2.5 );
      declareProperty( "Author", m_author = xAOD::Muon::MuidCo );
   }

   StatusCode MuonSelectionToolExample::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising..." );
      ATH_MSG_DEBUG( "Maximum eta: " << m_maxEta );
      ATH_MSG_DEBUG( "Selected author: " << m_author );

      // Set up the TAccept object:
      m_accept.addCut( "Eta",
                       "Selection of muons according to their pseudorapidity" );
      m_accept.addCut( "Author",
                       "Selection of muons according to their author" );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   const Root::TAccept& MuonSelectionToolExample::getTAccept() const {

      return m_accept;
   }

   const Root::TAccept&
   MuonSelectionToolExample::accept( const xAOD::IParticle* p ) const {

      // Reset the result:
      m_accept.clear();

      // Check if this is a muon:
      if( p->type() != xAOD::Type::Muon ) {
         ATH_MSG_ERROR( "accept(...) Function received a non-muon" );
         return m_accept;
      }

      // Cast it to a muon:
      const xAOD::Muon* mu = dynamic_cast< const xAOD::Muon* >( p );
      if( ! mu ) {
         ATH_MSG_FATAL( "accept(...) Failed to cast particle to muon" );
         return m_accept;
      }

      // Let the specific function do the work:
      return accept( *mu );
   }

   const Root::TAccept& MuonSelectionToolExample::accept( const xAOD::Muon& mu ) const {

      // Reset the result:
      m_accept.clear();

      // Do the eta cut:
      ATH_MSG_VERBOSE( "Muon eta: " << mu.eta() );
      if( std::abs( mu.eta() ) > m_maxEta ) {
         return m_accept;
      }
      m_accept.setCutResult( "Eta", true );

      // Do the author cut:
      ATH_MSG_VERBOSE( "Muon author: " << mu.author() );
      if( mu.author() != m_author ) {
         return m_accept;
      }
      m_accept.setCutResult( "Author", true );

      // Return the result:
      return m_accept;
   }

} // namespace CP
