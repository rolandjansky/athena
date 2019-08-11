/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_RdoToDigit.h"
#include "MuonIdHelpers/MmIdHelper.h"

MM_RdoToDigit::MM_RdoToDigit(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode MM_RdoToDigit::initialize()
{
  ATH_CHECK( detStore()->retrieve(m_mmHelper, "MMIDHELPER") );
  ATH_CHECK( m_mmRdoDecoderTool.retrieve() );
  ATH_CHECK(m_mmRdoKey.initialize());
  ATH_CHECK(m_mmDigitKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode MM_RdoToDigit::execute()
{
  ATH_MSG_DEBUG( "in execute()"  );
  SG::ReadHandle<Muon::MM_RawDataContainer> rdoRH(m_mmRdoKey);
  if (!rdoRH.isValid()) {
    ATH_MSG_ERROR( "No MM RDO container found!"  );
    return StatusCode::FAILURE;
  }
  const Muon::MM_RawDataContainer* rdoContainer = rdoRH.cptr();
  ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " MM RDOs." );

  SG::WriteHandle<MmDigitContainer> wh_mmDigit(m_mmDigitKey);
  ATH_CHECK(wh_mmDigit.record(std::make_unique<MmDigitContainer>(m_mmHelper->module_hash_max())));
  ATH_MSG_DEBUG( "Decoding MM RDO into MM Digit"  );

  // retrieve the collection of RDO
  Identifier oldId;
  MmDigitCollection * collection = nullptr;
  // now decode RDO into digits
  for (const Muon::MM_RawDataCollection * coll : *rdoContainer) { 
    ATH_CHECK(this->decodeMM(coll, wh_mmDigit.ptr(), collection, oldId));
  }

  return StatusCode::SUCCESS;
}

StatusCode MM_RdoToDigit::decodeMM( const Muon::MM_RawDataCollection * rdoColl, MmDigitContainer * mmContainer, MmDigitCollection*& collection, Identifier& oldId ) const
{
  const IdContext mmContext = m_mmHelper->module_context();

  if ( rdoColl->size() > 0 ) {
    ATH_MSG_DEBUG( " Number of RawData in this rdo "
                   << rdoColl->size()  );

    // for each RDO, loop over RawData, converter RawData to digit
    // retrieve/create digit collection, and insert digit into collection
    for (const Muon::MM_RawData * data : *rdoColl) { 
      MmDigit * newDigit = m_mmRdoDecoderTool->getDigit(data);
      if (!newDigit) {
        ATH_MSG_WARNING( "Error in MM RDO decoder"  );
        continue;
      }

      // find here the Proper Digit Collection identifier, using the rdo-hit id
      // (since RDO collections are not in a 1-to-1 relation with digit collections)
      const Identifier elementId = m_mmHelper->elementID(newDigit->identify());
      IdentifierHash coll_hash;
      if (m_mmHelper->get_hash(elementId, coll_hash, &mmContext)) {
        ATH_MSG_WARNING( "Unable to get MM digit collection hash id "
                         << "context begin_index = " << mmContext.begin_index()
                         << " context end_index  = " << mmContext.end_index()
                         << " the identifier is " );
        elementId.show();
      }


      if (oldId != elementId) {
        MmDigitContainer::const_iterator it_coll = mmContainer->indexFind(coll_hash);
        if (mmContainer->end() ==  it_coll) {
          MmDigitCollection * newCollection =
            new MmDigitCollection(elementId,coll_hash);
          newCollection->push_back(newDigit);
          collection = newCollection;
          if (mmContainer->addCollection(newCollection, coll_hash).isFailure())
            ATH_MSG_WARNING( "Couldn't record MmDigitCollection with key=" << coll_hash
                             << " in StoreGate!"  );
        }
        else {
          MmDigitCollection * oldCollection = (MmDigitCollection*) *it_coll;
          oldCollection->push_back(newDigit);
          collection = oldCollection;
        }
        oldId = elementId;
      }
      else {
        collection->push_back(newDigit);
      }
    }
  }
  return StatusCode::SUCCESS;
}
