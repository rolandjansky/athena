/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:
#include "TestCaloGeom.h"

// Athena related 
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IToolSvc.h"
#include <algorithm>
#include <cmath>
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

// specific :
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloDetDescr/ICaloCoordinateTool.h"

#include "boost/io/ios_state.hpp"

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
TestCaloGeom::TestCaloGeom(const std::string& name, 
			   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_coord("TBCaloCoordinate")
{}

// DESTRUCTOR:
TestCaloGeom::~TestCaloGeom()
{  }

// INITIALIZE:
StatusCode TestCaloGeom::initialize()
{
  ATH_CHECK( m_coord.retrieve() );
  return StatusCode::SUCCESS;
}

// FINALIZE:
StatusCode TestCaloGeom::finalize()
{
  return StatusCode::SUCCESS;
}

// EXECUTE:
StatusCode TestCaloGeom::execute()
{  
  ATH_MSG_INFO ( "Executing TestCaloGeom" );
  
  // This little class handles the phi range cheching (-pi,pi)
  CaloPhiRange toto;  
  toto.print(); 

  print_beam();

  return StatusCode::SUCCESS;
}

void
TestCaloGeom::print_beam()
{
  ATH_MSG_INFO ( "Executing TestCaloGeom : print_beam " );

  double eta= m_coord->beam_local_eta();
  double phi= m_coord->beam_local_phi();

  std::cout  << " " << std::endl;
  std::cout  << " " << std::endl;
  std::cout  << " The H8 beam calo-local coodinates are : eta=" << eta 
	     << " phi=" << phi << std::endl;
  std::cout  << " " << std::endl;
  std::cout  << " " << std::endl;

}
