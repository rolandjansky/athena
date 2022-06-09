/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "STGC_DigitToRDO.h"

class NswCalibDbTimeChargeData;


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
  ATH_CHECK(m_calibTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode STGC_DigitToRDO::execute(const EventContext& ctx) const
{
  using namespace Muon;
  ATH_MSG_DEBUG( "in execute()"  );
  SG::ReadHandle<sTgcDigitContainer> digits (m_digitContainer, ctx);
  std::unique_ptr<STGC_RawDataContainer> rdos = std::make_unique<STGC_RawDataContainer>(m_idHelperSvc->stgcIdHelper().module_hash_max());
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

      constexpr double lowerTimeBound = Muon::STGC_RawData::s_lowerTimeBound;
      constexpr int BCWindow = Muon::STGC_RawData::s_BCWindow;
      for (const sTgcDigit* digit : *digitColl ) {
        // Set proper data time window in simulated sTGC RDOs
        // BC0 has t = [-12.5, +12.5]
        // and data will use BC = [-3,+4]
        // totaling digits within t = [-87.5, 112.5]
        float digitTime = digit->time();
        if (digitTime < lowerTimeBound || digitTime >= lowerTimeBound+BCWindow*25) continue;
        Identifier id = digit->identify();
        bool isDead = digit->isDead();
        int tdo     = 0;
        int pdo     = 0;
        int relBCID = 0;  
        m_calibTool->timeToTdo  (ctx, digitTime      , id, tdo, relBCID); 
        m_calibTool->chargeToPdo(ctx, digit->charge(), id, pdo         ); 
        STGC_RawData* rdo = new STGC_RawData(id, 
                                             static_cast<unsigned int>(relBCID), 
                                             static_cast<float>(tdo), 
                                             static_cast<unsigned int>(tdo), 
                                             static_cast<unsigned int>(pdo), 
                                             isDead, 
                                             true);
        coll->push_back(rdo);
      }
    }
  }
  else {
    ATH_MSG_WARNING("Unable to find STGC digits");
  }
  SG::WriteHandle<STGC_RawDataContainer> writeHandle (m_rdoContainer, ctx);
  ATH_CHECK(writeHandle.record(std::move(rdos)));

  ATH_MSG_DEBUG( "done execute()"  );
  return StatusCode::SUCCESS;
}
