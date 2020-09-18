/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircle_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Container_p1.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircleCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircleContainerCnv_p1.h"
#include "AthenaKernel/errorcheck.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"



#include "AthAllocators/DataPool.h"

void InDet::TRT_DriftCircleContainerCnv_p1::transToPers(const InDet::TRT_DriftCircleContainer* transCont,  InDet::InDetPRD_Container_p1* persCont, MsgStream &log) 
{

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all RDO
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and add their channels
    // to the container's vector, saving the indexes in the
    // collection. 

    typedef InDet::TRT_DriftCircleContainer TRANS;
    typedef ITPConverterFor<Trk::PrepRawData> CONV;

    TRT_DriftCircleCnv_p1  chanCnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;
    persCont->m_collections.resize(transCont->numberOfCollections());
//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " Preparing " << persCont->m_collections.size() << "Collections" << endmsg;
  
    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, ++it_Coll)  {
        // Add in new collection
        const InDet::TRT_DriftCircleCollection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        InDet::InDetPRD_Collection_p1& pcollection = persCont->m_collections[collIndex];
        pcollection.m_id    = collection.identify().get_compact();
        pcollection.m_hashId = (unsigned int) collection.identifyHash();
        pcollection.m_begin = chanBegin;
        pcollection.m_end   = chanEnd;
        // Add in channels
        persCont->m_PRD.resize(chanEnd);
        for (unsigned int i = 0; i < collection.size(); ++i) {
            const InDet::TRT_DriftCircle* chan = collection[i];
            persCont->m_PRD[i + chanBegin] = toPersistent((CONV**)0, chan, log );
        }
    }
//   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Writing TRT_DriftCircleContainer ***" << endmsg;
}

void  InDet::TRT_DriftCircleContainerCnv_p1::persToTrans(const InDet::InDetPRD_Container_p1* persCont, InDet::TRT_DriftCircleContainer* transCont, MsgStream &log) 
{

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all channels
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and extract their channels
    // from the vector.


    InDet::TRT_DriftCircleCollection* coll = 0;

    TRT_DriftCircleCnv_p1  chanCnv;
    typedef ITPConverterFor<Trk::PrepRawData> CONV;

//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " Reading " << persCont->m_collections.size() << "Collections" << endmsg;
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

        // Create trans collection - is NOT owner of TRT_DriftCircle (SG::VIEW_ELEMENTS)
	// IDet collection don't have the Ownership policy c'tor
        const InDet::InDetPRD_Collection_p1& pcoll = persCont->m_collections[icoll];        
        //Identifier collID(Identifier(pcoll.m_id));
        IdentifierHash collIDHash(IdentifierHash(pcoll.m_hashId));
        coll = new InDet::TRT_DriftCircleCollection(collIDHash);
        coll->setIdentifier(Identifier(pcoll.m_id));
        unsigned int nchans           = pcoll.m_end - pcoll.m_begin;
        coll->resize(nchans);
        const InDetDD::TRT_BaseElement * de = m_trtMgr->getElement(collIDHash);
        // Fill with channels
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
            const TPObjRef pchan = persCont->m_PRD[ichan + pcoll.m_begin];
            InDet::TRT_DriftCircle* chan = dynamic_cast<InDet::TRT_DriftCircle*>(createTransFromPStore((CONV**)0, pchan, log ) );
            if (chan) {
               chan->m_detEl = de;
               (*coll)[ichan] = chan;
            }
        }
        
        // register the rdo collection in IDC with hash - faster addCollection
        StatusCode sc = transCont->addCollection(coll, collIDHash);
        if (sc.isFailure()) {
            throw std::runtime_error("Failed to add collection to ID Container");
        }
//         if (log.level() <= MSG::DEBUG) {
//             log << MSG::DEBUG << "AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << (int) collIDHash << " / " << 
// collID.get_compact() << ", added to Identifiable container." << endmsg;
//         }
    }

//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Reading TRT_DriftCircleContainer" << endmsg;
}



//================================================================
InDet::TRT_DriftCircleContainer* InDet::TRT_DriftCircleContainerCnv_p1::createTransient(const InDet::InDetPRD_Container_p1* persObj, MsgStream& log) {
    if(!m_isInitialized) {
     if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize TRT_DriftCircleContainerCnv_p1 " << endmsg;
     } 
    }
    std::unique_ptr<InDet::TRT_DriftCircleContainer> trans(std::make_unique<InDet::TRT_DriftCircleContainer>(m_trtId->module_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}

StatusCode InDet::TRT_DriftCircleContainerCnv_p1::initialize(MsgStream &/*log*/) {
   // Do not initialize again:
   m_isInitialized=true;
   
   // Get Storegate, ID helpers, and so on
   ISvcLocator* svcLocator = Gaudi::svcLocator();

   StoreGateSvc *detStore = nullptr;
   CHECK( svcLocator->service("DetectorStore", detStore) );
   CHECK( detStore->retrieve(m_trtId, "TRT_ID") );
   CHECK( detStore->retrieve(m_trtMgr) );
   return StatusCode::SUCCESS;
}
