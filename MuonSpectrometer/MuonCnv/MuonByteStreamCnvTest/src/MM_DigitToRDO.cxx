/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"
#include "MM_DigitToRDO.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

MM_DigitToRDO::MM_DigitToRDO(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_idHelper(0)
{
}

StatusCode MM_DigitToRDO::initialize()
{
  declareProperty("RDOContainerName", m_rdoContainer = "MMRDO");
  declareProperty("DigitContainerName", m_digitContainer = "MM_DIGITS");
  
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_rdoContainer.initialize() );
  ATH_CHECK( m_digitContainer.initialize() );
  ATH_CHECK( detStore()->retrieve(m_idHelper, "MMIDHELPER") );  
  return StatusCode::SUCCESS;
}

StatusCode MM_DigitToRDO::execute()
{  
  using namespace Muon;
  ATH_MSG_DEBUG( "in execute()"  );
  SG::WriteHandle<MM_RawDataContainer> rdos (m_rdoContainer);
  SG::ReadHandle<MmDigitContainer> digits (m_digitContainer);
  ATH_CHECK( rdos.record(std::unique_ptr<MM_RawDataContainer>(new MM_RawDataContainer(m_idHelper->module_hash_max())) ) );

  if (digits.isValid()){
    for (const MmDigitCollection* digitColl : *digits ){
    
      // Making some assumptions here that digit hash == RDO hash. 
      IdentifierHash hash = digitColl->identifierHash();
      MM_RawDataCollection* coll = new MM_RawDataCollection(hash);
      if (rdos->addCollection(coll,hash).isFailure() ){
        ATH_MSG_WARNING("Failed to add collection with hash " << (int)hash );
        delete coll;
        continue;
      }
    
      for (const MmDigit* digit : *digitColl ){
        Identifier id = digit->identify();
        MM_RawData* rdo = new MM_RawData(id);
        coll->push_back(rdo);
      }
    }
  } else {
    ATH_MSG_WARNING("Unable to find MM digits");
  }
    
  ATH_MSG_DEBUG( "done execute()"  );
  return StatusCode::SUCCESS;
}

StatusCode MM_DigitToRDO::finalize() {
  return StatusCode::SUCCESS;
}
