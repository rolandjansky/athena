/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "STGC_DigitToRDO.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

STGC_DigitToRDO::STGC_DigitToRDO(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode STGC_DigitToRDO::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK( m_rdoContainer.initialize() );
  ATH_CHECK( m_digitContainer.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode STGC_DigitToRDO::execute(const EventContext& ctx) const
{
  using namespace Muon;
  ATH_MSG_DEBUG( "in execute()"  );
  SG::WriteHandle<STGC_RawDataContainer> rdos (m_rdoContainer, ctx);
  SG::ReadHandle<sTgcDigitContainer> digits (m_digitContainer, ctx);
  ATH_CHECK( rdos.record(std::make_unique<STGC_RawDataContainer>(m_idHelperSvc->stgcIdHelper().module_hash_max())) );

  if (digits.isValid() ) {
    for (const sTgcDigitCollection* digitColl : *digits ) {

      // Transform the hash id ( digit collection use detector element ID hash, RDO's use
      // module Id hash
      Identifier digitId = digitColl->identify();
      IdentifierHash hash;
      int getRdoCollHash = m_idHelperSvc->stgcIdHelper().get_module_hash(digitId,hash);
      if ( getRdoCollHash !=0 ) {
        ATH_MSG_ERROR("Could not get the module hash Id");
        return StatusCode::FAILURE;
      }

      STGC_RawDataCollection* coll = new STGC_RawDataCollection(hash);
      if (rdos->addCollection(coll,hash).isFailure() ) {
        ATH_MSG_WARNING("Failed to add collection with hash " << (int)hash );
        delete coll;
        continue;
      }

      for (const sTgcDigit* digit : *digitColl ) {
        Identifier id = digit->identify();
        uint16_t bcTag = digit->bcTag();
        float time   = digit->time();
        unsigned int tdo = static_cast<unsigned int>(25.0+digit->time()); //place holder for time->tdo from calibration
        // 10bit charge conversion to PDO is done in sTGC_digitization as a quick fix for now, it will be corrected once the calibration is in place
	      unsigned int charge = static_cast<unsigned int>(digit->charge()); //place holder for charge->pdo from calibration
        bool isDead = digit->isDead();
        STGC_RawData* rdo = new STGC_RawData(id, bcTag, time, tdo, charge, isDead);
        coll->push_back(rdo);
      }
    }
  }
  else {
    ATH_MSG_WARNING("Unable to find STGC digits");
  }

  ATH_MSG_DEBUG( "done execute()"  );
  return StatusCode::SUCCESS;
}
