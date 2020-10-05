/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:
#include "TestLArDetDescr/TestLArTT.h"

// Athena related 
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include <algorithm>
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

// specific :
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloTTDetDescr/CaloTTDescrManager.h"
#include "CaloTTDetDescr/CaloTTDescriptor.h"

// looking for Volumes :
//#include "LArDetDescr/ILArVolumeTool.h"
//#include "GaudiKernel/IToolSvc.h"

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
TestLArTT::TestLArTT(const std::string& name, 
				   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_tt_man(0)
{}

// DESTRUCTOR:
TestLArTT::~TestLArTT()
{  }

// INITIALIZE:
StatusCode TestLArTT::initialize()
{
  ATH_CHECK( detStore()->retrieve(m_tt_man, "CaloTTMgr") );
  ATH_MSG_DEBUG ( " Found the CaloTTDescrManager" );
  return StatusCode::SUCCESS;
}

// FINALIZE:
StatusCode TestLArTT::finalize()
{
  return StatusCode::SUCCESS;
}

// EXECUTE:
StatusCode TestLArTT::execute()
{  
  print_reg( false, true, true, false );
  
  return StatusCode::SUCCESS;
}

void
TestLArTT::print_reg(bool /*em*/, bool /*hec*/, bool /*fcal*/, bool /*tiles*/)
{
  ATH_MSG_INFO ( "Executing TestLArTT" );

  std::vector<CaloTTDescriptor*>::const_iterator
    first = m_tt_man->calo_descriptors_begin();

  std::vector<CaloTTDescriptor*>::const_iterator
    last = m_tt_man->calo_descriptors_end();

  int i=0;
  for (; first != last; ++first) {
    std::cout << std::endl;
    std::cout << " ------ CaloTTDescriptor number " << i << std::endl;
    (*first)->print();
    i++;
  }

  
}

