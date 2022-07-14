/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   ElectronMergerAlgorithm
//   Author: Jonathan Long, jonathan.long@cern.ch
//   Copied from MuonMergerAlgorithm: Author: Sagar Addepalli, addepalli.sagar@cern.ch
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Electron merger algorirthm is a wrapper algorithm around the ElectronMergerTool
// to be used downstream for combining LRT electrons and standard electrons 
///////////////////////////////////////////////////////////////////
#include "DerivationFrameworkEGamma/ElectronMergerAlgorithm.h"


ElectronMergerAlgorithm::ElectronMergerAlgorithm( const std::string& name, ISvcLocator* svcLoc )
   : AthAlgorithm( name, svcLoc ){}

StatusCode ElectronMergerAlgorithm::initialize() {

   // Greet the user:
   ATH_MSG_INFO( "Initialising" );
   // Retrieve the tools:
   ATH_CHECK( m_electronMergerTool.retrieve() );

   // Return gracefully:
   return StatusCode::SUCCESS;
}

StatusCode ElectronMergerAlgorithm::execute() {

  //Retrieve the merged electrons:
  ATH_CHECK( m_electronMergerTool->addBranches() );
  return StatusCode::SUCCESS;

}