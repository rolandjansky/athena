/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonEventTPCnv/MuonDigitContainer/STGC_Digit_p1.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonDigitContainer_p1.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonEventTPCnv/MuonDigitContainer/STGC_DigitContainerCnv_p1.h"
#include "MuonEventTPCnv/MuonDigitContainer/STGC_DigitCnv_p1.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
// #include "GaudiKernel/CnvFactory.h"
// #include "GaudiKernel/StatusCode.h"
// #include "GaudiKernel/Service.h"

// Athena
#include "StoreGate/StoreGateSvc.h"
#include "MuonEventTPCnv/CreateTransientTemplates.h"


//#include "DataModel/DataPool.h"
/*******************************************************************************/
StatusCode Muon::STGC_DigitContainerCnv_p1::initialize(MsgStream &log) {
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
  sc = detStore->retrieve(m_sTgcId);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not get sTgc IdHelper !" << endmsg;
    return StatusCode::FAILURE;
  } else {
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the sTgc IdHelper." << endmsg;
  }

  sc = detStore->retrieve(m_muonDetMgr);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not get DetectorDescription manager" << endmsg;
    return sc;
  }

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;
  return StatusCode::SUCCESS;
}
/*******************************************************************************/
void Muon::STGC_DigitContainerCnv_p1::transToPers(const sTgcDigitContainer* transCont,  Muon::STGC_DigitContainer_p1* persCont, MsgStream &log) 
{
  if(log.level() <= MSG::DEBUG && !m_isInitialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize STGC_DigitContainerCnv_p1 " << endmsg;
    } 
  }
  // The transient model has a container holding collections and the
  // collections hold channels.
  //
  // The persistent model flattens this so that the persistent
  // container has two sets of vectors:
  //   1) data for all collections, and
  //   2) a vector containing all DIGITs
  //
  // The persistent collections, then only maintain indexes into the
  // container's vector of all channels. 
  //
  // So here we loop over all collection and add their channels
  // to the container's vector, saving the indexes in the
  // collection. 

  // std::cout<<"Starting transToPers"<<std::endl;
  typedef sTgcDigitContainer TRANS;
  //typedef ITPConverterFor<Trk::PrepRawData> CONV;

  STGC_DigitCnv_p1  chanCnv;
  TRANS::const_iterator it_Coll     = transCont->begin();
  TRANS::const_iterator it_CollEnd  = transCont->end();
  unsigned int pcollIndex = 0; // index to the persistent collection we're filling
  unsigned int pcollBegin = 0; // index to start of persistent collection we're filling, in long list of persistent DIGITs
  unsigned int pcollEnd = 0; // index to end 
  unsigned int idHashLast = 0; // Used to calculate deltaHashId.
  int numColl = transCont->numberOfCollections();
  // persCont->m_collections.resize(numColl);
  persCont->m_collectionId.resize(numColl);
  persCont->m_collectionHashId.resize(numColl);
  persCont->m_size.resize(numColl);
  persCont->m_digits.resize(numColl);

  // First loop to get size of digits
  unsigned int numOfDigits=0;
  for (; it_Coll != it_CollEnd; it_Coll++)  {
    numOfDigits+=(*it_Coll)->size();
  }
  persCont->m_digits.resize(numOfDigits);
  persCont->m_digitDeltaId.resize(numOfDigits);
  
  if (log.level() <= MSG::DEBUG) 
    log << MSG::DEBUG<< " Preparing " << persCont->m_collectionId.size() << "collections and "<<numOfDigits<<" digits in total." <<endmsg;
  // std::cout<<"Preparing " << persCont->m_collections.size() << " collections" << std::endl;
  for (it_Coll = transCont->begin(); it_Coll != it_CollEnd; ++pcollIndex, it_Coll++)  {  
    const sTgcDigitCollection& collection = (**it_Coll);
    if (log.level() <= MSG::DEBUG) 
      log << MSG::DEBUG<<"Coll hash for "<<pcollIndex<<": "<<collection.identifierHash()<<endmsg;
    // Add in new collection
    // Muon::MuonDIGIT_Collection_p2& pcollection = persCont->m_collections[pcollIndex]; //get ref to collection we're going to fill

    pcollBegin  = pcollEnd; // Next collection starts at end of previous one.
    pcollEnd   += collection.size();

    persCont->m_collectionHashId[pcollIndex] = collection.identifierHash(); 
    idHashLast += persCont->m_collectionHashId[pcollIndex];
    persCont->m_collectionId[pcollIndex] = collection.identify().get_identifier32().get_compact();
    persCont->m_size[pcollIndex] = collection.size();
    //        std::cout<<"Coll Index: "<<pcollIndex<<"\tCollId: "<<collection.identify().get_compact()<<"\tCollHash: "<<collection.identifierHash()<<"\tpCollId: "<<pcollection.m_id<<"\tpCollHash: "<<std::endl;

    //unsigned int lastPRDIdHash = 0;
    for (unsigned int i = 0; i < collection.size(); ++i) {
      unsigned int pchanIndex=i+pcollBegin;
      const sTgcDigit* chan = collection[i]; // channel being converted
      STGC_Digit_p1*   pchan = &(persCont->m_digits[pchanIndex]); // persistent version to fill
      chanCnv.transToPers(chan, pchan, log); // convert from sTgcDigit to STGC_Digit_p1
      unsigned int clusIdCompact = chan->identify().get_identifier32().get_compact();
      unsigned int collIdCompact = collection.identify().get_identifier32().get_compact();
      unsigned int diff = clusIdCompact - collIdCompact;
      if (diff>std::numeric_limits<uint16_t>::max()) log << MSG::ERROR<<"Diff of "<<diff<<" is greater than max size of diff permitted!!! ("<<std::numeric_limits<uint16_t>::max()<<")"<<endmsg;
      persCont->m_digitDeltaId[pchanIndex]=diff; //store delta identifiers, rather than full identifiers
    }
  }
  if (log.level() <= MSG::DEBUG) 
    log << MSG::DEBUG<< " ***  Writing STGC_DigitContainer ***" <<endmsg;
}
/*******************************************************************************/
void  Muon::STGC_DigitContainerCnv_p1::persToTrans(const Muon::STGC_DigitContainer_p1* persCont, sTgcDigitContainer* transCont, MsgStream &log) 
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

  sTgcDigitCollection* coll = 0;

  STGC_DigitCnv_p1  chanCnv;
  unsigned int pchanIndex(0); // position within persCont->m_digits. Incremented inside innermost loop 
  unsigned int pCollEnd = persCont->m_size.size();
  if (log.level() <= MSG::DEBUG) 
    log << MSG::DEBUG<< " Reading " << pCollEnd << "Collections" <<endmsg;
  for (unsigned int pcollIndex = 0; pcollIndex < pCollEnd; ++pcollIndex) {
    // const Muon::MuonPRD_Collection_p2& pcoll = persCont->m_collections[pcollIndex];        
    IdentifierHash collIDHash(persCont->m_collectionHashId[pcollIndex] );
    log << MSG::DEBUG<<"Coll hash for "<<pcollIndex<<": "<<collIDHash<<endmsg;
    
    coll = new sTgcDigitCollection(Identifier(persCont->m_collectionId[pcollIndex]),collIDHash);

    unsigned int pchanEnd = pchanIndex+persCont->m_size[pcollIndex];
    unsigned int chanIndex = 0; // transient index

    coll->reserve(persCont->m_size[pcollIndex]);
    // Fill with channels
    for (; pchanIndex < pchanEnd; ++ pchanIndex, ++chanIndex) {
      const STGC_Digit_p1* pchan = &(persCont->m_digits[pchanIndex]);
      sTgcDigit* chan = new sTgcDigit;

      chanCnv.persToTrans(pchan, chan, log);// FIXME! remove.
      log << MSG::DEBUG<<"Trans id:"<<std::hex<<chan->identify().get_identifier32().get_compact()<<"\t pers Id:"<<pchan->m_muonId<<std::dec<<endmsg;
      // std::cout <<"Trans id:"<<chan->identify()<<"\t pers Id:"<<pchan->m_id<<std::endl;
      
      if ( m_sTgcId->valid(chan->identify())!=true ) {
	// have invalid PRD
        log << MSG::WARNING  << "sTgc PRD has invalid Identifier of "<< m_sTgcId->show_to_string(chan->identify())
	    <<" - are you sure you have the correct geometry loaded, and NSW enabled?" << endmsg;
      } 
      log << MSG::DEBUG<<"chan identify(): "<<chan->identify()<<endmsg;      
      coll->push_back(chan);    
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
    log << MSG::DEBUG<< " ***  Reading STGC_DigitContainer ***" << endmsg;
}
/*******************************************************************************/
sTgcDigitContainer* Muon::STGC_DigitContainerCnv_p1::createTransient(const Muon::STGC_DigitContainer_p1* persObj, MsgStream& log) 
{
  if(!m_isInitialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize STGC_DigitContainerCnv_p1 " << endmsg;
      return 0;
    } 
  }
  std::auto_ptr<sTgcDigitContainer> trans(new sTgcDigitContainer(m_sTgcId->detectorElement_hash_max()));
  persToTrans(persObj, trans.get(), log);
  return trans.release();
}
/*******************************************************************************/

