// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

// Local include(s):
#include "AsgAnalysisAlgorithms/TreeMakerAlg.h"

// ROOT include(s):
#include <TTree.h>

namespace CP {

   TreeMakerAlg::TreeMakerAlg( const std::string& name,
                                                 ISvcLocator* svcLoc )
   : EL::AnaAlgorithm( name, svcLoc ) {

      // Declare the algorithm's properties.
      declareProperty( "TreeName", m_treeName = "physics",
                       "Name of the tree to write" );
      declareProperty( "TreeAutoFlush", m_treeAutoFlush = 200,
                       "AutoFlush value for the output tree" );
   }

   StatusCode TreeMakerAlg::execute() {
      if( m_treeConfigured ) {
         return StatusCode::SUCCESS;
      }

      // Create the output tree.
      ATH_CHECK( book( TTree( m_treeName.c_str(), "xAOD->NTuple tree" ) ) );
      TTree *mytree { tree( m_treeName ) };
      if( ! mytree ) {
         ATH_MSG_ERROR( "Could not create output tree \"" << m_treeName
                        << "\"" );
         return StatusCode::FAILURE;
      }
      mytree->SetAutoFlush( m_treeAutoFlush );
      ATH_MSG_INFO( "Created xAOD->NTuple tree: " << m_treeName );

      m_treeConfigured = true;

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace CP
