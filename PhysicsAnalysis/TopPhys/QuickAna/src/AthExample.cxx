/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// QuickAna includes
#include "AthExample.h"

#include <QuickAna/IQuickAna.h>
#include <xAODEgamma/ElectronContainer.h>

namespace ana
{

AthExample::AthExample( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ){

  //declareProperty( "Property", m_nProperty ); //example property declaration

  declareProperty ("quickAna", quickAna);
}


AthExample::~AthExample() {}


StatusCode AthExample::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_CHECK( quickAna.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode AthExample::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode AthExample::execute() {
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  ATH_CHECK (quickAna->process ());

  ATH_MSG_INFO ("electron size: " << quickAna->electrons()->size());

  return StatusCode::SUCCESS;
}


}
