/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"
#include "sTGC_DigitToRDO.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

sTGC_DigitToRDO::sTGC_DigitToRDO(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_idHelper(0)
{
}

StatusCode sTGC_DigitToRDO::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_rdoContainer.initialize() );
  ATH_CHECK( m_digitContainer.initialize() );
  
  if ( detStore()->retrieve(m_idHelper, "STGCIDHELPER").isFailure()) {
    ATH_MSG_FATAL ( "Could not get sTGC IdHelper !" );
    return StatusCode::FAILURE;
  } 
  
  return StatusCode::SUCCESS;
}

StatusCode sTGC_DigitToRDO::execute()
{  
  ATH_MSG_DEBUG( "in execute()"  );
  SG::WriteHandle<sTGC_RawDataContainer> rdos (m_rdoContainer);
  SG::ReadHandle<sTgcDigitContainer> digits (m_digitContainer);
  ATH_CHECK( rdos.record(std::unique_ptr<sTGC_RawDataContainer>(new sTGC_RawDataContainer(m_idHelper->module_hash_max())) ) );

  for (auto digitColl : *digits ){
    // Making some assumptions here that digit hash == RDO hash. 
    IdentifierHash hash = digitColl->identifierHash();
    sTGC_RawDataCollection* coll = new sTGC_RawDataCollection(hash);
    if (rdos->addCollection(coll,hash).isFailure() ){
      ATH_MSG_WARNING("Failed to add collection with hash " << (int)hash );
      delete coll;
      continue;
    }
    
    for (auto digit : *digitColl ){
      Identifier id = digit->identify();
      sTGC_RawData* rdo = new sTGC_RawData(id);
      coll->push_back(rdo);
    }
  }
    
  ATH_MSG_DEBUG( "done execute()"  );
  return StatusCode::SUCCESS;
}

StatusCode sTGC_DigitToRDO::finalize() {
  return StatusCode::SUCCESS;
}
