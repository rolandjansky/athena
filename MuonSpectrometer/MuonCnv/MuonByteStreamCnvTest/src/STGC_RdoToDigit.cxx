/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "STGC_RdoToDigit.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

STGC_RdoToDigit::STGC_RdoToDigit(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode STGC_RdoToDigit::initialize()
{
  ATH_CHECK( detStore()->retrieve(m_stgcHelper, "STGCIDHELPER") );
  ATH_CHECK( m_stgcRdoDecoderTool.retrieve() );
  ATH_CHECK(m_stgcRdoKey.initialize());
  ATH_CHECK(m_stgcDigitKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode STGC_RdoToDigit::execute()
{
  ATH_MSG_DEBUG( "in execute()"  );
  SG::ReadHandle<Muon::STGC_RawDataContainer> rdoRH(m_stgcRdoKey);
  if (!rdoRH.isValid()) {
    ATH_MSG_ERROR( "No STGC RDO container found!"  );
    return StatusCode::FAILURE;
  }
  const Muon::STGC_RawDataContainer* rdoContainer = rdoRH.cptr();
  ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " sTGC RDOs." );

  SG::WriteHandle<sTgcDigitContainer> wh_stgcDigit(m_stgcDigitKey);
  ATH_CHECK(wh_stgcDigit.record(std::make_unique<sTgcDigitContainer>(m_stgcHelper->detectorElement_hash_max())));
  ATH_MSG_DEBUG( "Decoding sTGC RDO into sTGC Digit"  );

  // retrieve the collection of RDO
  Identifier oldId;
  sTgcDigitCollection * collection = nullptr;
  // now decode RDO into digits
  for (const Muon::STGC_RawDataCollection * coll : *rdoContainer) {
    ATH_CHECK(this->decodeSTGC(coll, wh_stgcDigit.ptr(), collection, oldId));
  }

  return StatusCode::SUCCESS;
}

StatusCode STGC_RdoToDigit::decodeSTGC( const Muon::STGC_RawDataCollection * rdoColl, sTgcDigitContainer * stgcContainer, sTgcDigitCollection*& collection, Identifier& oldId ) const
{
  const IdContext stgcContext = m_stgcHelper->module_context();

  if ( rdoColl->size() > 0 ) {
    ATH_MSG_DEBUG( " Number of RawData in this rdo "
                   << rdoColl->size()  );

    // for each RDO, loop over RawData, converter RawData to digit
    // retrieve/create digit collection, and insert digit into collection
    for (const Muon::STGC_RawData * data : *rdoColl) {
      sTgcDigit * newDigit = m_stgcRdoDecoderTool->getDigit(data);
      if (!newDigit) {
        ATH_MSG_ERROR( "Error in sTGC RDO decoder"  );
        continue;
      }

      // find here the Proper Digit Collection identifier, using the rdo-hit id
      // (since RDO collections are not in a 1-to-1 relation with digit collections)
      const Identifier elementId = m_stgcHelper->elementID(newDigit->identify());
      IdentifierHash coll_hash;
      if (m_stgcHelper->get_hash(elementId, coll_hash, &stgcContext)) {
        ATH_MSG_WARNING( "Unable to get STGC digit collection hash id "
                         << "context begin_index = " << stgcContext.begin_index()
                         << " context end_index  = " << stgcContext.end_index()
                         << " the identifier is " );
        elementId.show();
      }


      if (oldId != elementId) {
        sTgcDigitContainer::const_iterator it_coll = stgcContainer->indexFind(coll_hash);
        if (stgcContainer->end() ==  it_coll) {
          sTgcDigitCollection * newCollection =
            new sTgcDigitCollection(elementId,coll_hash);
          newCollection->push_back(newDigit);
          collection = newCollection;
          if (stgcContainer->addCollection(newCollection, coll_hash).isFailure())
            ATH_MSG_WARNING( "Couldn't record sTgcDigitCollection with key=" << coll_hash
                             << " in StoreGate!"  );
        }
        else {
          sTgcDigitCollection * oldCollection = (sTgcDigitCollection*) *it_coll;
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
