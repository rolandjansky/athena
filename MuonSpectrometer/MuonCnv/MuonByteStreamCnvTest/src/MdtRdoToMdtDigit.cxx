/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtRdoToMdtDigit.h"
#include "MuonIdHelpers/MdtIdHelper.h"

MdtRdoToMdtDigit::MdtRdoToMdtDigit(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode MdtRdoToMdtDigit::initialize()
{
  ATH_CHECK( detStore()->retrieve(m_mdtHelper, "MDTIDHELPER") );
  ATH_CHECK( m_mdtRdoDecoderTool.retrieve() );
  ATH_CHECK(m_mdtRdoKey.initialize());
  ATH_CHECK(m_mdtDigitKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode MdtRdoToMdtDigit::execute()
{
  ATH_MSG_DEBUG( "in execute()"  );
  SG::ReadHandle<MdtCsmContainer> rdoRH(m_mdtRdoKey);
  if (!rdoRH.isValid()) {
    ATH_MSG_WARNING( "No MDT RDO container found!"  );
    return StatusCode::SUCCESS;
  }
  const MdtCsmContainer* rdoContainer = rdoRH.cptr();
  ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " MDT RDOs." );

  SG::WriteHandle<MdtDigitContainer> wh_mdtDigit(m_mdtDigitKey);
  ATH_CHECK(wh_mdtDigit.record(std::make_unique<MdtDigitContainer>(m_mdtHelper->module_hash_max())));
  ATH_MSG_DEBUG( "Decoding MDT RDO into MDT Digit"  );

  // retrieve the collection of RDO
  Identifier oldId;
  MdtDigitCollection * collection = nullptr;
  // now decode RDO into digits
  MdtCsmContainer::const_iterator mdtCSM = rdoContainer->begin();
  for (; mdtCSM!=rdoContainer->end();++mdtCSM) {
    ATH_CHECK(this->decodeMdt(*mdtCSM, wh_mdtDigit.ptr(), collection, oldId));
  }

  return StatusCode::SUCCESS;
}

StatusCode MdtRdoToMdtDigit::decodeMdt( const MdtCsm * rdoColl, MdtDigitContainer * mdtContainer, MdtDigitCollection*& collection, Identifier& oldId ) const
{
  const IdContext mdtContext = m_mdtHelper->module_context();

  if ( rdoColl->size() > 0 ) {
    ATH_MSG_DEBUG( " Number of AmtHit in this Csm "
                   << rdoColl->size()  );

    uint16_t subdetId = rdoColl->SubDetId();
    uint16_t mrodId = rdoColl->MrodId();
    uint16_t csmId = rdoColl->CsmId();

    // for each Csm, loop over AmtHit, converter AmtHit to digit
    // retrieve/create digit collection, and insert digit into collection
    for (const MdtAmtHit * amtHit : *rdoColl) {
      MdtDigit * newDigit = m_mdtRdoDecoderTool->getDigit(amtHit,subdetId,
                                                          mrodId,csmId);

      if (!newDigit) {
        ATH_MSG_WARNING( "Error in MDT RDO decoder"  );
        continue;
      }

      // find here the Proper Digit Collection identifier, using the rdo-hit id
      // (since RDO collections are not in a 1-to-1 relation with digit collections)
      const Identifier elementId = m_mdtHelper->elementID(newDigit->identify());
      IdentifierHash coll_hash;
      if (m_mdtHelper->get_hash(elementId, coll_hash, &mdtContext)) {
        ATH_MSG_WARNING( "Unable to get MDT digit collection hash id "
                         << "context begin_index = " << mdtContext.begin_index()
                         << " context end_index  = " << mdtContext.end_index()
                         << " the identifier is " );
        elementId.show();
      }


      if (oldId != elementId) {
        MdtDigitContainer::const_iterator it_coll = mdtContainer->indexFind(coll_hash);
        if (mdtContainer->end() ==  it_coll) {
          MdtDigitCollection * newCollection =
            new MdtDigitCollection(elementId,coll_hash);
          newCollection->push_back(newDigit);
          collection = newCollection;
          if (mdtContainer->addCollection(newCollection, coll_hash).isFailure())
            ATH_MSG_WARNING( "Couldn't record MdtDigitCollection with key=" << coll_hash
                             << " in StoreGate!"  );
        }
        else {
          MdtDigitCollection * oldCollection = const_cast<MdtDigitCollection*>( *it_coll );
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
