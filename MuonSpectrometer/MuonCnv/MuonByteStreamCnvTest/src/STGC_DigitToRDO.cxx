/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"
#include "STGC_DigitToRDO.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

STGC_DigitToRDO::STGC_DigitToRDO(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_idHelper(0)
{
}

StatusCode STGC_DigitToRDO::initialize()
{
  declareProperty("RDOContainerName", m_rdoContainer = "sTGCRDO");
  declareProperty("DigitContainerName", m_digitContainer = "sTGC_DIGITS");
  
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_rdoContainer.initialize() );
  ATH_CHECK( m_digitContainer.initialize() );
  
  if ( detStore()->retrieve(m_idHelper, "STGCIDHELPER").isFailure()) {
    ATH_MSG_FATAL ( "Could not get sTGC IdHelper !" );
    return StatusCode::FAILURE;
  } 
  
  return StatusCode::SUCCESS;
}

StatusCode STGC_DigitToRDO::execute()
{  
  using namespace Muon;
  ATH_MSG_DEBUG( "in execute()"  );
  SG::WriteHandle<STGC_RawDataContainer> rdos (m_rdoContainer);
  SG::ReadHandle<sTgcDigitContainer> digits (m_digitContainer);
  ATH_CHECK( rdos.record(std::unique_ptr<STGC_RawDataContainer>(new STGC_RawDataContainer(m_idHelper->module_hash_max())) ) );

  for (auto digitColl : *digits ){
    // Making some assumptions here that digit hash == RDO hash. 
    IdentifierHash hash = digitColl->identifierHash();
    STGC_RawDataCollection* coll = new STGC_RawDataCollection(hash);
    if (rdos->addCollection(coll,hash).isFailure() ){
      ATH_MSG_WARNING("Failed to add collection with hash " << (int)hash );
      delete coll;
      continue;
    }
    
    for (auto digit : *digitColl ){
      Identifier id = digit->identify();
      STGC_RawData* rdo = new STGC_RawData(id);
      coll->push_back(rdo);
    }
  }
    
  ATH_MSG_DEBUG( "done execute()"  );
  return StatusCode::SUCCESS;
}

StatusCode STGC_DigitToRDO::finalize() {
  return StatusCode::SUCCESS;
}
