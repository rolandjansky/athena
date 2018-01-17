/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Takashi Kubota - June 30, 2008 */
#include "MuonTrigCoinData/RpcCoinData.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"
#include "MuonEventTPCnv/MuonTrigCoinData/RpcCoinData_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/MuonCoinDataContainer_p1.h"


#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonEventTPCnv/MuonTrigCoinData/RpcCoinDataCnv_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/RpcCoinDataContainerCnv_p1.h"
#include "CxxUtils/make_unique.h"

// // Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
// #include "GaudiKernel/CnvFactory.h"
// #include "GaudiKernel/StatusCode.h"
// #include "GaudiKernel/Service.h"
// #include "GaudiKernel/MsgStream.h"
// #include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"
#include "AthAllocators/DataPool.h"

StatusCode Muon::RpcCoinDataContainerCnv_p1::initialize(MsgStream &log) {
   // Do not initialize again:
    m_isInitialized=true;

   // Get Storegate, ID helpers, and so on
    ISvcLocator* svcLocator = Gaudi::svcLocator();
   // // get StoreGate service
   //  StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
   //  if (sc.isFailure()) {
   //      log << MSG::FATAL << "StoreGate service not found !" << endmsg;
   //      return StatusCode::FAILURE;
   //  }

   // get DetectorStore service
    StoreGateSvc *detStore;
    StatusCode sc = svcLocator->service("DetectorStore", detStore);
    if (sc.isFailure()) {
        log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        log << MSG::DEBUG << "Found DetectorStore." << endmsg;
    }

   // Get the pixel helper from the detector store
    sc = detStore->retrieve(m_RpcId);
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get Rpc ID helper !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        log << MSG::DEBUG << "Found the Rpc ID helper." << endmsg;
    }

    sc = detStore->retrieve(m_muonDetMgr);
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get RpcDetectorDescription" << endmsg;
        return sc;
    }

    log << MSG::DEBUG << "Converter initialized." << endmsg;
    return StatusCode::SUCCESS;
}

void Muon::RpcCoinDataContainerCnv_p1::transToPers(const RpcCoinDataContainerCnv_p1::TRANS* transCont,  RpcCoinDataContainerCnv_p1::PERS* persCont, MsgStream &log) 
{
        // The transient model has a container holding collections and the
        // collections hold channels.
        //
        // The persistent model flattens this so that the persistent
        // container has two vectors:
        //   1) all collections, and
        //   2) all PRD
        //
        // The persistent collections, then only maintain indexes into the
        // container's vector of all channels. 
        //
        // So here we loop over all collection and add their channels
        // to the container's vector, saving the indexes in the
        // collection. 

        typedef Muon::RpcCoinDataContainer TRANS;

        RpcCoinDataCnv_p1  chanCnv;
        TRANS::const_iterator it_Coll     = transCont->begin();
        TRANS::const_iterator it_CollEnd  = transCont->end();
        unsigned int pcollIndex; // index to the persistent collection we're filling
        unsigned int pcollBegin = 0; // index to start of persistent collection we're filling, in long list of persistent PRDs
        unsigned int pcollEnd = 0; // index to end 
        unsigned int idHashLast = 0; // Used to calculate deltaHashId.
        int numColl = transCont->numberOfCollections();
        persCont->m_collections.resize(numColl);
        log << MSG::DEBUG  << " Preparing " << persCont->m_collections.size() << "Collections" << endmsg;
        for (pcollIndex = 0; it_Coll != it_CollEnd; ++pcollIndex, it_Coll++)  {
            // Add in new collection
            const Muon::MuonCoinDataCollection<RpcCoinData>& collection = (**it_Coll);
            Muon::MuonPRD_Collection_p2& pcollection = persCont->m_collections[pcollIndex];
            
            pcollBegin  = pcollEnd; // Next collection starts at end of previous one.
            pcollEnd   += collection.size();
            
            pcollection.m_hashId = collection.identifyHash(); 
            idHashLast += pcollection.m_hashId;
            pcollection.m_id = collection.identify().get_identifier32().get_compact();
            pcollection.m_size = collection.size();
            
            
            // Add in channels
            persCont->m_prds.resize(pcollEnd);
            persCont->m_prdDeltaId.resize(pcollEnd);
            log << MSG::VERBOSE << "Reading collections with " <<  collection.size() << "PRDs " << endmsg;
            for (unsigned int i = 0; i < collection.size(); ++i) {
                unsigned int pchanIndex=i+pcollBegin;
                const Muon::RpcCoinData* chan = collection[i];
                Muon::RpcCoinData_p1* pchan = &(persCont->m_prds[pchanIndex]);
                chanCnv.transToPers(chan, pchan, log);
                persCont->m_prdDeltaId[pchanIndex]=chan->identify().get_identifier32().get_compact() - collection.identify().get_identifier32().get_compact(); //store delta identifiers, rather than full identifiers
            }
        }
      log << MSG::DEBUG  << " ***  Writing Muon::RpcCoinDataContainer" << endmsg;
}

void  Muon::RpcCoinDataContainerCnv_p1::persToTrans(const RpcCoinDataContainerCnv_p1::PERS* persCont, RpcCoinDataContainerCnv_p1::TRANS* transCont, MsgStream &log) 
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


      Muon::MuonCoinDataCollection<RpcCoinData>* coll = 0;

      RpcCoinDataCnv_p1  chanCnv;
      unsigned int pchanIndex(0); // position within persCont->m_prds. Incremented inside innermost loop 
      unsigned int pCollEnd = persCont->m_collections.size();
      log << MSG::DEBUG  << " Reading " << persCont->m_collections.size() << "Collections" << endmsg;
      for (unsigned int pcollIndex = 0; pcollIndex < pCollEnd; ++pcollIndex) {
          const Muon::MuonPRD_Collection_p2& pcoll = persCont->m_collections[pcollIndex];       
          // Identifier collID= Identifier(idLast);
          IdentifierHash collIDHash=IdentifierHash(pcoll.m_hashId);
          coll = new Muon::RpcCoinDataCollection(collIDHash);
          coll->setIdentifier(Identifier(pcoll.m_id)); 
          
          unsigned int pchanEnd = pchanIndex+pcoll.m_size;
          unsigned int chanIndex = 0; // transient index
          
          coll->resize(pcoll.m_size);
          // Fill with channels:
          for (; pchanIndex < pchanEnd; ++ pchanIndex, ++chanIndex) {
              const Muon::RpcCoinData_p1* pchan = &(persCont->m_prds[pchanIndex]);
              Identifier clusId(pcoll.m_id + persCont->m_prdDeltaId[pchanIndex]);

              // The reason I need to do the following is that one collection can have several detector elements in, the collection hashes!=detector element hashes
              IdentifierHash deIDHash;
              int result = m_RpcId->get_detectorElement_hash(clusId, deIDHash);
              if (result) log << MSG::WARNING  << " Muon::MdtPrepDataContainerCnv_p2::persToTrans: problem converting Identifier to DE hash "<<endmsg;
              const MuonGM::RpcReadoutElement* detEl =
                m_muonDetMgr->getRpcReadoutElement(deIDHash);;

              auto chan = CxxUtils::make_unique<Muon::RpcCoinData>
                (chanCnv.createRpcCoinData (pchan,
                                            clusId,
                                            detEl,
                                            log));

              chan->setHashAndIndex(collIDHash, chanIndex);
              (*coll)[chanIndex] = std::move(chan);
          }

          // register the PRD collection in IDC with hash - faster addCollection
          StatusCode sc = transCont->addCollection(coll, collIDHash);
          if (sc.isFailure()) {
              throw std::runtime_error("Failed to add collection to ID Container");
          }
          if (log.level() <= MSG::DEBUG) {
              log << MSG::DEBUG << "AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << (int) collIDHash << " / " 
                  << coll->identify().get_compact() << ", added to Identifiable container." << endmsg;
          }
      }

      log << MSG::DEBUG  << " ***  Reading InDet::SCT_ClusterContainer" << endmsg;

}

//================================================================
Muon::RpcCoinDataContainerCnv_p1::TRANS* Muon::RpcCoinDataContainerCnv_p1::createTransient(const Muon::RpcCoinDataContainerCnv_p1::PERS* persObj, MsgStream& log) 
{
    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize RpcCoinDataContainerCnv_p1 " << endmsg;
            return 0;
        } 
    }
    std::auto_ptr<Muon::RpcCoinDataContainer> trans(new Muon::RpcCoinDataContainer(m_RpcId->module_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


