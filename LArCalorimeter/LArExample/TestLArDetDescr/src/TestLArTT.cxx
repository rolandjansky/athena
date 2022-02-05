/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:
#include "TestLArTT.h"

// Athena related 
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include <algorithm>
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

// specific :
#include "CaloTTDetDescr/CaloTTDescrManager.h"
#include "CaloTTDetDescr/CaloTTDescriptor.h"

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
TestLArTT::TestLArTT(const std::string& name, 
		     ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

// INITIALIZE:
StatusCode TestLArTT::initialize()
{
  ATH_CHECK(m_caloMgrKey.initialize());
  return StatusCode::SUCCESS;
}

// EXECUTE:
StatusCode TestLArTT::execute()
{
  if(!m_tt_man.isValid()) {
    const CaloTTDescrManager* ttMan{nullptr};
    ATH_CHECK(detStore()->retrieve(ttMan));
    m_tt_man.set(ttMan);
  }

  print_reg();
  
  return StatusCode::SUCCESS;
}

void
TestLArTT::print_reg()
{
  ATH_MSG_INFO ( "Executing TestLArTT" );

  const CaloTTDescrManager* ttMan = *m_tt_man.ptr();
  std::vector<CaloTTDescriptor*>::const_iterator first = ttMan->calo_descriptors_begin();
  std::vector<CaloTTDescriptor*>::const_iterator last  = ttMan->calo_descriptors_end();

  int i=0;
  for (; first != last; ++first) {
    std::cout << std::endl;
    std::cout << " ------ CaloTTDescriptor number " << i << std::endl;
    (*first)->print();
    i++;
  }

  
}

