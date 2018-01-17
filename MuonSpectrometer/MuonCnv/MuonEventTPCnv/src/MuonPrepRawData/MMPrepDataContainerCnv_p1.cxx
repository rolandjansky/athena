/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPrepRawData/MMPrepData.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MMPrepData_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p1.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonEventTPCnv/MuonPrepRawData/MMPrepDataCnv_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/MMPrepDataContainerCnv_p1.h"
#include "CxxUtils/make_unique.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
// #include "GaudiKernel/CnvFactory.h"
// #include "GaudiKernel/StatusCode.h"
// #include "GaudiKernel/Service.h"

// Athena
#include "StoreGate/StoreGateSvc.h"


StatusCode Muon::MMPrepDataContainerCnv_p1::initialize(MsgStream &log) {
   // Do not initialize again:
  m_isInitialized=true;

   // Get Storegate, ID helpers, and so on
  ISvcLocator* svcLocator = Gaudi::svcLocator();
   // get StoreGate service
  StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    log << MSG::FATAL << "StoreGate service not found !" << endmsg;
    return StatusCode::FAILURE;
  }

   // get DetectorStore service
  StoreGateSvc *detStore;
  sc = svcLocator->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  } else {
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endmsg;
  }

   // Get the helper from the detector store
  sc = detStore->retrieve(m_MMId);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not get MM IdHelper !" << endmsg;
    return StatusCode::FAILURE;
  } else {
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the MM IdHelper." << endmsg;
  }

  sc = detStore->retrieve(m_muonDetMgr);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not get DetectorDescription manager" << endmsg;
    return sc;
  }

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;
  return StatusCode::SUCCESS;
}

void Muon::MMPrepDataContainerCnv_p1::transToPers(const Muon::MMPrepDataContainer* transCont,  Muon::MMPrepDataContainer_p1* persCont, MsgStream &log) 
{
  if(/*log.level() <= MSG::DEBUG &&*/ !m_isInitialized) {
      if (this->initialize(log) != StatusCode::SUCCESS) {
          log << MSG::FATAL << "Could not initialize MMPrepDataContainerCnv_p1 " << endmsg;
      } 
  }
    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all PRDs
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and add their channels
    // to the container's vector, saving the indexes in the
    // collection. 

  // std::cout<<"Starting transToPers"<<std::endl;
  typedef Muon::MMPrepDataContainer TRANS;
    //typedef ITPConverterFor<Trk::PrepRawData> CONV;

  MMPrepDataCnv_p1  chanCnv;
  TRANS::const_iterator it_Coll     = transCont->begin();
  TRANS::const_iterator it_CollEnd  = transCont->end();
  unsigned int pcollIndex; // index to the persistent collection we're filling
  unsigned int pcollBegin = 0; // index to start of persistent collection we're filling, in long list of persistent PRDs
  unsigned int pcollEnd = 0; // index to end 
  unsigned int idHashLast = 0; // Used to calculate deltaHashId.
  int numColl = transCont->numberOfCollections();
  persCont->m_collections.resize(numColl);

  if (log.level() <= MSG::DEBUG) 
    log << MSG::DEBUG<< " Preparing " << persCont->m_collections.size() << "collections" <<endmsg;
  // std::cout<<"Preparing " << persCont->m_collections.size() << " collections" << std::endl;
  for (pcollIndex = 0; it_Coll != it_CollEnd; ++pcollIndex, it_Coll++)  {  
    const Muon::MMPrepDataCollection& collection = (**it_Coll);
    if (log.level() <= MSG::DEBUG) 
      log << MSG::DEBUG<<"Coll hash for "<<pcollIndex<<": "<<collection.identifyHash()<<endmsg;
    // Add in new collection
    Muon::MuonPRD_Collection_p2& pcollection = persCont->m_collections[pcollIndex]; //get ref to collection we're going to fill

    pcollBegin  = pcollEnd; // Next collection starts at end of previous one.
    pcollEnd   += collection.size();

    pcollection.m_hashId = collection.identifyHash(); 
    idHashLast += pcollection.m_hashId;
    pcollection.m_id = collection.identify().get_identifier32().get_compact();
    pcollection.m_size = collection.size();
//        std::cout<<"Coll Index: "<<pcollIndex<<"\tCollId: "<<collection.identify().get_compact()<<"\tCollHash: "<<collection.identifyHash()<<"\tpCollId: "<<pcollection.m_id<<"\tpCollHash: "<<std::endl;

        // Add in channels
    persCont->m_prds.resize(pcollEnd); // FIXME! isn't this potentially a bit slow? Do a resize and a copy for each loop? EJWM.
    persCont->m_prdDeltaId.resize(pcollEnd);

    //unsigned int lastPRDIdHash = 0;
    for (unsigned int i = 0; i < collection.size(); ++i) {
      unsigned int pchanIndex=i+pcollBegin;
      const MMPrepData* chan = collection[i]; // channel being converted
      MMPrepData_p1*   pchan = &(persCont->m_prds[pchanIndex]); // persistent version to fill
      chanCnv.transToPers(chan, pchan, log); // convert from MMPrepData to MMPrepData_p1
      unsigned int clusIdCompact = chan->identify().get_identifier32().get_compact();
      unsigned int collIdCompact = collection.identify().get_identifier32().get_compact();
      unsigned int diff = clusIdCompact - collIdCompact;
      if (diff>sizeof(unsigned short)) log << MSG::WARNING<<"Diff is greater than max size of diff permitted!!!"<<endmsg;
      persCont->m_prdDeltaId[pchanIndex]=diff; //store delta identifiers, rather than full identifiers
      // FIXME - above doesn't work ATM 
      log << MSG::DEBUG<<"Trans id:"<<std::hex<<clusIdCompact<<"\t pers Id:"<<pchan->m_id<<std::dec<<endmsg;
      // std::cout <<"Trans id:"<<chan->m_clusId<<"\t pers Id:"<<pchan->m_id<<std::endl;
      
      //check! 
      log << MSG::DEBUG<<"chan identify(): "<<chan->identify()<<endmsg;
      
      if (chan->detectorElement() != m_muonDetMgr->getMMReadoutElement(chan->identify()))
        log << MSG::WARNING<<"DE from det manager ("<<m_muonDetMgr->getMMReadoutElement(chan->identify())
            <<") does not match that from PRD ("<<chan->detectorElement()<<")"<<endmsg; 
      
      // sanity checks - to be removed at some point
      // if(log.level() <= MSG::DEBUG){
      //        log << MSG::DEBUG<<i<<":\t clusId: "<<clusIdCompact<<", \t collectionId="<<collIdCompact<<"\t delta="<<persCont->m_prdDeltaId[pchanIndex]<<"\t diff="<<diff<<endmsg;
      //        Identifier temp(pcollection.m_id + persCont->m_prdDeltaId[pchanIndex]);
      //        if (temp!=chan->m_clusId ) 
      //          log << MSG::WARNING << "PRD ids differ! Transient:"<<chan->m_clusId<<", From persistent:"<<temp<<" diff = "<<chan->m_clusId.get_compact()-temp.get_compact()<<endmsg;
      //        else 
      //          log << MSG::DEBUG <<" PRD ids match."<<endmsg;
      //        if (lastPRDIdHash && lastPRDIdHash != chan->collectionHash() )  log << MSG::WARNING << "Collection Identifier hashes differ!"<<endmsg;
      //        lastPRDIdHash = chan->collectionHash();
      //        log << MSG::DEBUG<<"Collection hash = "<<lastPRDIdHash<<endmsg;
      //        if (chan->collectionHash()!= collection.identifyHash() ) log << MSG::WARNING << "Collection's idHash does not match PRD collection hash!"<<endmsg;
      //        if (chan->m_detEl !=m_muonDetMgr->getMMReadoutElement(chan->identify())) 
      //          log << MSG::WARNING << "Getting de from identity didn't work!"<<endmsg;
      //        else 
      //          log << MSG::DEBUG<<"Getting de from identity did work "<<endmsg;
      //        if (chan->m_detEl !=m_muonDetMgr->getMMReadoutElement(temp)) log << MSG::WARNING << "Getting de from reconstructed identity didn't work!"<<endmsg;
      //        log << MSG::DEBUG<<"Finished loop"<<endmsg;
      //      }
    }
  }
  if (log.level() <= MSG::DEBUG) 
    log << MSG::DEBUG<< " ***  Writing MMPrepDataContainer ***" <<endmsg;
}

void  Muon::MMPrepDataContainerCnv_p1::persToTrans(const Muon::MMPrepDataContainer_p1* persCont, Muon::MMPrepDataContainer* transCont, MsgStream &log) 
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

  Muon::MMPrepDataCollection* coll = 0;

  MMPrepDataCnv_p1  chanCnv;
  unsigned int pchanIndex(0); // position within persCont->m_prds. Incremented inside innermost loop 
  unsigned int pCollEnd = persCont->m_collections.size();
  if (log.level() <= MSG::DEBUG) 
    log << MSG::DEBUG<< " Reading " << pCollEnd << "Collections" <<endmsg;
  for (unsigned int pcollIndex = 0; pcollIndex < pCollEnd; ++pcollIndex) {
    const Muon::MuonPRD_Collection_p2& pcoll = persCont->m_collections[pcollIndex];        
    IdentifierHash collIDHash(pcoll.m_hashId);
    log << MSG::DEBUG<<"Coll hash for "<<pcollIndex<<": "<<collIDHash<<endmsg;
    
    coll = new Muon::MMPrepDataCollection(collIDHash);
        // Identifier firstChanId = persCont->m_prds[collBegin].m_clusId;
        // Identifier collId = m_MMId->parentID(firstChanId);
    coll->setIdentifier(Identifier(pcoll.m_id)); 

//        std::cout<<"Coll Index: "<<pcollIndex<<"\tCollId: "<<collection.identify().get_compact()<<"\tCollHash: "<<collection.identifyHash()<<"\tpCollId: "<<pcollection.m_id<<"\tpCollHash: "<<std::endl;

        // FIXME - really would like to remove Identifier from collection, but cannot as there is :
        // a) no way (apparently - find it hard to believe) to go from collection IdHash to collection Identifer.

    unsigned int pchanEnd = pchanIndex+pcoll.m_size;
    unsigned int chanIndex = 0; // transient index

    coll->reserve(pcoll.m_size);
        // Fill with channels
    for (; pchanIndex < pchanEnd; ++ pchanIndex, ++chanIndex) {
      const MMPrepData_p1* pchan = &(persCont->m_prds[pchanIndex]);
      
      const MuonGM::MMReadoutElement* detEl =
        m_muonDetMgr->getMMReadoutElement(Identifier(pchan->m_id));
      if (!detEl) {
        if (log.level() <= MSG::WARNING) 
          log << MSG::WARNING<< "Muon::MMPrepDataContainerCnv_p1::persToTrans: could not get valid det element for PRD with id="<<pchan->m_id<<". Skipping."<<endmsg;
        continue;
      }

      // chan->m_clusId=Identifier(pcoll.m_id + persCont->m_prdDeltaId[pchanIndex]); FIXME! Put this back once diff is sane.
      auto chan = CxxUtils::make_unique<MMPrepData>
        (chanCnv.createMMPrepData (pchan, detEl, log));

      log << MSG::DEBUG<<"Trans id:"<<std::hex<<chan->identify().get_identifier32().get_compact()<<"\t pers Id:"<<pchan->m_id<<std::dec<<endmsg;

      // std::cout <<"Trans id:"<<chan->m_clusId<<"\t pers Id:"<<pchan->m_id<<std::endl;
      
      if ( m_MMId->valid(chan->identify())!=true ) {
                // have invalid PRD
        log << MSG::WARNING  << "MM PRD has invalid Identifier of "<< m_MMId->show_to_string(chan->identify())
          <<" - are you sure you have the correct geometry loaded, and NSW enabled?" << endmsg;
      } 
      // chanCnv.persToTrans(pchan, chan, log); // Fill chan with data from pchan FIXME! Put this back once diff is sane.

      // The reason I need to do the following is that one collection can have several detector elements in, the collection hashes!=detector element hashes
      // IdentifierHash deIDHash;
      // int result = m_MMId->get_detectorElement_hash(chan->identify(), deIDHash);
      // if (result&&log.level() <= MSG::WARNING) 
      //   log << MSG::WARNING<< " Muon::MMPrepDataContainerCnv_p1::persToTrans: problem converting Identifier to DE hash "<<endmsg;
          // chan->m_detEl = m_muonDetMgr->getMMReadoutElement(deIDHash);;
      log << MSG::DEBUG<<"chan identify(): "<<chan->identify()<<endmsg;

      chan->setHashAndIndex(collIDHash, chanIndex); 
      coll->push_back(std::move(chan));
      
    }

      // register the rdo collection in IDC with hash - faster addCollection
    StatusCode sc = transCont->addCollection(coll, collIDHash);
    if (sc.isFailure()) {
      throw std::runtime_error("Failed to add collection to Identifiable Container");
    }
    if (log.level() <= MSG::DEBUG) {
      log << MSG::DEBUG << "AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << (int) collIDHash << " / " << 
        coll->identify().get_compact() << ", added to Identifiable container." << endmsg;
    }
  }

    if (log.level() <= MSG::DEBUG) 
      log << MSG::DEBUG<< " ***  Reading MMPrepDataContainer ***" << endmsg;
}

Muon::MMPrepDataContainer* Muon::MMPrepDataContainerCnv_p1::createTransient(const Muon::MMPrepDataContainer_p1* persObj, MsgStream& log) 
{
  if(!m_isInitialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize MMPrepDataContainerCnv_p1 " << endmsg;
      return 0;
    } 
  }
  std::auto_ptr<Muon::MMPrepDataContainer> trans(new Muon::MMPrepDataContainer(m_MMId->detectorElement_hash_max()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


