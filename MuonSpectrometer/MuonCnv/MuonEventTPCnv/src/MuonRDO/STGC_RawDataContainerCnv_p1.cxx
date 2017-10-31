/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventTPCnv/MuonRDO/STGC_RawDataContainerCnv_p1.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataContainer_p1.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

void Muon::STGC_RawDataContainerCnv_p1::transToPers(const Muon::STGC_RawDataContainer* transCont,  Muon::STGC_RawDataContainer_p1* persCont, MsgStream &log) 
{
  // std::cout<<"STGC_RawDataContainerCnv_p1::transToPers"<<std::endl;

//
// //  std::cout<<"Starting transToPers"<<std::endl;
//    typedef Muon::STGC_RawDataContainer TRANS;
//     //typedef ITPConverterFor<Trk::PrepRawData> CONV;
//
    STGC_RawDataCnv_p1  chanCnv;
    auto it_Coll     = transCont->begin();
    auto it_CollEnd  = transCont->end();
//     unsigned int pcollIndex; // index to the persistent collection we're filling
//     unsigned int pcollBegin = 0; // index to start of persistent collection we're filling, in long list of persistent PRDs
//     unsigned int pcollEnd = 0; // index to end
//     unsigned int idHashLast = 0; // Used to calculate deltaHashId.
//     int numColl = transCont->numberOfCollections();
//     persCont->m_collections.resize(numColl);
//
//     if (log.level() <= MSG::DEBUG)
//         log << MSG::DEBUG<< " Preparing " << persCont->m_collections.size() << "Collections" <<endmsg;
//   //  std::cout<<"Preparing " << persCont->m_collections.size() << "Collections" << std::endl;
//     for (pcollIndex = 0; it_Coll != it_CollEnd; ++pcollIndex, it_Coll++)  {
//         // Add in new collection
//       if (log.level() <= MSG::DEBUG)
//           log << MSG::DEBUG<<"New collection"<<endmsg;
//         const Muon::STGC_RawDataCollection& collection = (**it_Coll);
//         Muon::MuonPRD_Collection_p2& pcollection = persCont->m_collections[pcollIndex]; //get ref to collection we're going to fill
//
//         pcollBegin  = pcollEnd; // Next collection starts at end of previous one.
//         pcollEnd   += collection.size();
//
//         pcollection.m_hashId = collection.identifyHash();
//         idHashLast += pcollection.m_hashId;
//         pcollection.m_id = collection.identify().get_identifier32().get_compact();
//         pcollection.m_size = collection.size();
// //        std::cout<<"Coll Index: "<<pcollIndex<<"\tCollId: "<<collection.identify().get_compact()<<"\tCollHash: "<<collection.identifyHash()<<"\tpCollId: "<<pcollection.m_id<<"\tpCollHash: "<<std::endl;
//
//         // Add in channels
//         persCont->m_prds.resize(pcollEnd); // FIXME! isn't this potentially a bit slow? Do a resize and a copy for each loop? EJWM.
//         persCont->m_prdDeltaId.resize(pcollEnd);
//
//         unsigned int lastPRDIdHash = 0;
//         for (unsigned int i = 0; i < collection.size(); ++i) {
//             unsigned int pchanIndex=i+pcollBegin;
//             const STGC_RawData* chan = collection[i]; // channel being converted
//             STGC_RawData_p1*   pchan = &(persCont->m_prds[pchanIndex]); // persistent version to fill
//             chanCnv.transToPers(chan, pchan, log); // convert from STGC_RawData to STGC_RawData_p1
//
//             unsigned int clusIdCompact = chan->identify().get_identifier32().get_compact();
//             unsigned int collIdCompact = collection.identify().get_identifier32().get_compact();
//
//             persCont->m_prdDeltaId[pchanIndex]=clusIdCompact - collIdCompact; //store delta identifiers, rather than full identifiers
//             if(log.level() <= MSG::DEBUG){
//               log << MSG::DEBUG<<i<<":\t clusId: "<<clusIdCompact<<", \t collectionId="<<collIdCompact<<"\t delta="<<persCont->m_prdDeltaId[pchanIndex]<<endmsg;
//               Identifier temp(pcollection.m_id + persCont->m_prdDeltaId[pchanIndex]);
//               if (temp!=chan->identify() )
//                 log << MSG::WARNING << "PRD ids differ! Transient:"<<chan->identify()<<", From persistent:"<<temp<<" diff = "<<chan->identify().get_compact()-temp.get_compact()<<endmsg;
//               else
//                 log << MSG::DEBUG <<" PRD ids match."<<endmsg;
//               if (lastPRDIdHash && lastPRDIdHash != chan->collectionHash() )  log << MSG::WARNING << "Collection Identifier hashes differ!"<<endmsg;
//               lastPRDIdHash = chan->collectionHash();
//               log << MSG::DEBUG<<"Collection hash = "<<lastPRDIdHash<<endmsg;
//
//               if (chan->collectionHash()!= collection.identifyHash() ) log << MSG::WARNING << "Collection's idHash does not match PRD collection hash!"<<endmsg;
//               if (chan->detectorElement() !=m_muonDetMgr->getsTgcReadoutElement(chan->identify()))
//                 log << MSG::WARNING << "Getting de from identity didn't work!"<<endmsg;
//               else
//                 log << MSG::DEBUG<<"Getting de from identity did work "<<endmsg;
//               if (chan->detectorElement() !=m_muonDetMgr->getsTgcReadoutElement(temp)) log << MSG::WARNING << "Getting de from reconstructed identity didn't work!"<<endmsg;
//               log << MSG::DEBUG<<"Finished loop"<<endmsg;
//             }
//         }
//     }
//     if (log.level() <= MSG::DEBUG)
//         log << MSG::DEBUG<< " ***  Writing STGC_RawDataContainer ***" <<endmsg;
}

void  Muon::STGC_RawDataContainerCnv_p1::persToTrans(const Muon::STGC_RawDataContainer_p1* persCont, Muon::STGC_RawDataContainer* transCont, MsgStream &log) 
{
  std::cout<<"STGC_RawDataContainerCnv_p1::persToTrans"<<std::endl;
  
//
//     // The transient model has a container holding collections and the
//     // collections hold channels.
//     //
//     // The persistent model flattens this so that the persistent
//     // container has two vectors:
//     //   1) all collections, and
//     //   2) all channels
//     //
//     // The persistent collections, then only maintain indexes into the
//     // container's vector of all channels.
//     //
//     // So here we loop over all collection and extract their channels
//     // from the vector.
//
//     Muon::STGC_RawDataCollection* coll = 0;
//
    STGC_RawDataCnv_p1  chanCnv;
//     unsigned int pchanIndex(0); // position within persCont->m_prds. Incremented inside innermost loop
//     unsigned int pCollEnd = persCont->m_collections.size();
//     if (log.level() <= MSG::DEBUG)
//         log << MSG::DEBUG<< " Reading " << pCollEnd << "Collections" <<endmsg;
//     for (unsigned int pcollIndex = 0; pcollIndex < pCollEnd; ++pcollIndex) {
//         const Muon::MuonPRD_Collection_p2& pcoll = persCont->m_collections[pcollIndex];
//         IdentifierHash collIDHash(pcoll.m_hashId);
//         coll = new Muon::STGC_RawDataCollection(collIDHash);
//         // Identifier firstChanId = persCont->m_prds[collBegin].m_clusId;
//         // Identifier collId = m_TgcId->parentID(firstChanId);
//         coll->setIdentifier(Identifier(pcoll.m_id));
//
// //        std::cout<<"Coll Index: "<<pcollIndex<<"\tCollId: "<<collection.identify().get_compact()<<"\tCollHash: "<<collection.identifyHash()<<"\tpCollId: "<<pcollection.m_id<<"\tpCollHash: "<<std::endl;
//
//         // FIXME - really would like to remove Identifier from collection, but cannot as there is :
//         // a) no way (apparently - find it hard to believe) to go from collection IdHash to collection Identifer.
//
//         unsigned int pchanEnd = pchanIndex+pcoll.m_size;
//         unsigned int chanIndex = 0; // transient index
//
//         coll->reserve(pcoll.m_size);
//         // Fill with channels
//         for (; pchanIndex < pchanEnd; ++ pchanIndex, ++chanIndex) {
//             const STGC_RawData_p1* pchan = &(persCont->m_prds[pchanIndex]);
//
//             Identifier clusId(pcoll.m_id + persCont->m_prdDeltaId[pchanIndex]);
//
//             if ( m_sTgcId->valid(clusId)!=true ) {
//               // have invalid PRD
//               log << MSG::WARNING  << "Tgc PRD has invalid Identifier of "<< m_sTgcId->show_to_string(clusId)<< " - are you sure you have the correct geometry loaded, and NSW enabled?"<<endmsg;
//             }
//
//             // The reason I need to do the following is that one collection can have several detector elements in, the collection hashes!=detector element hashes
//             IdentifierHash deIDHash;
//             int result = m_sTgcId->get_detectorElement_hash(clusId, deIDHash);
//             if (result&&log.level() <= MSG::WARNING)
//               log << MSG::WARNING<< " Muon::STGC_RawDataContainerCnv_p1::persToTrans: problem converting Identifier to DE hash "<<endmsg;
//             const MuonGM::sTgcReadoutElement* detEl =
//               m_muonDetMgr->getsTgcReadoutElement(clusId);
//             if (!detEl) {
//               log << MSG::WARNING<< "Muon::STGC_RawDataContainerCnv_p1::persToTrans: could not get valid det element for PRD with id="<<clusId<<". Skipping."<<endmsg;
//               continue;
//             }
//
//             auto chan = CxxUtils::make_unique<STGC_RawData>
//               (chanCnv.createSTGC_RawData (pchan,
//                                            clusId,
//                                            detEl,
//                                            log));
//
//             chan->setHashAndIndex(collIDHash, chanIndex);
//             coll->push_back(std::move(chan));
//         }
//
//         // register the rdo collection in IDC with hash - faster addCollection
//         StatusCode sc = transCont->addCollection(coll, collIDHash);
//         if (sc.isFailure()) {
//             throw std::runtime_error("Failed to add collection to Identifiable Container");
//         }
//         if (log.level() <= MSG::DEBUG) {
//             log << MSG::DEBUG << "AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << (int) collIDHash << " / " <<
//                 coll->identify().get_compact() << ", added to Identifiable container." << endmsg;
//         }
//     }
//
//     if (log.level() <= MSG::DEBUG)
//         log << MSG::DEBUG<< " ***  Reading STGC_RawDataContainer ***" << endmsg;
}



//================================================================
Muon::STGC_RawDataContainer* Muon::STGC_RawDataContainerCnv_p1::createTransient(const Muon::STGC_RawDataContainer_p1* persObj, MsgStream& log) 
{
  // std::cout<<"STGC_RawDataContainerCnv_p1::createTransient"<<std::endl;
    std::unique_ptr<STGC_RawDataContainer> trans(new Muon::STGC_RawDataContainer(m_sTgcId->detectorElement_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


