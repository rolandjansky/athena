// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

// Local include(s):
#include "AsgAnalysisAlgorithms/TreeFillerAlg.h"

// EDM include(s):
#include "AthContainersInterfaces/IAuxTypeVectorFactory.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/normalizedTypeinfoName.h"

// Framework include(s):
#include "SystematicsHandles/Helpers.h"

// ROOT include(s):
#include <TClass.h>
#include <TTree.h>
#include <TBranch.h>
#include <TVirtualCollectionProxy.h>

// System include(s):
#include <regex>
#include <algorithm>
#include <functional>
#include <sstream>

namespace CP {

   TreeFillerAlg::TreeFillerAlg( const std::string& name,
                                                 ISvcLocator* svcLoc )
   : EL::AnaAlgorithm( name, svcLoc ) {

      // Declare the algorithm's properties.
      declareProperty( "TreeName", m_treeName = "physics",
                       "Name of the tree to write" );
   }

   StatusCode TreeFillerAlg::execute() {
      // get the output tree for the first time
      if( ! m_tree ) {
         m_tree = tree( m_treeName );
      }

      if( ! m_tree ) {
         ATH_MSG_ERROR( "Could not find output tree \"" << m_treeName
                        << "\"" );
         return StatusCode::FAILURE;
      }

      // Fill the tree.
      if( m_tree->Fill() < 0 ) {
         ATH_MSG_ERROR( "Error while filling TTree" );
         return StatusCode::FAILURE;
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace CP
