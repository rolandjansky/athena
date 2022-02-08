/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPrepRawData/sTgcPrepData.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/sTgcPrepData_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonEventTPCnv/MuonPrepRawData/sTgcPrepDataCnv_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/sTgcPrepDataContainerCnv_p1.h"
#include "TrkEventCnvTools/ITrkEventCnvTool.h"
// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"


// Athena
#include "StoreGate/StoreGateSvc.h"


StatusCode Muon::sTgcPrepDataContainerCnv_p1::initialize(MsgStream &log) {
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
        log << MSG::FATAL << "Could not get ID helper !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the  ID helper." << endmsg;
    }

    if (m_eventCnvTool.retrieve().isFailure()) {
        log << MSG::FATAL << "Could not get DetectorDescription manager" << endmsg;
        return StatusCode::FAILURE;
    }

    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized. " << endmsg;
    return StatusCode::SUCCESS;
}
const MuonGM::sTgcReadoutElement* Muon::sTgcPrepDataContainerCnv_p1::getReadOutElement(const Identifier& id ) const {
    const Trk::ITrkEventCnvTool* cnv_tool = m_eventCnvTool->getCnvTool(id);
    if (!cnv_tool) return nullptr; 
    return dynamic_cast<const MuonGM::sTgcReadoutElement*>(cnv_tool->getDetectorElement(id));
}

void Muon::sTgcPrepDataContainerCnv_p1::transToPers(const Muon::sTgcPrepDataContainer* transCont,  Muon::sTgcPrepDataContainer_p1* persCont, MsgStream &log) 
{
  
  if(!m_isInitialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize sTgcPrepDataContainerCnv_p1 " << endmsg;
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
  
  typedef Muon::sTgcPrepDataContainer TRANS;
  
  sTgcPrepDataCnv_p1  chanCnv;
  TRANS::const_iterator it_Coll     = transCont->begin();
  TRANS::const_iterator it_CollEnd  = transCont->end();
  unsigned int pcollIndex = 0; // index to the persistent collection we're filling
  unsigned int pcollBegin = 0; // index to start of persistent collection we're filling, in long list of persistent PRDs
  unsigned int pcollEnd = 0; // index to end 
  int numColl = transCont->numberOfCollections();
  persCont->m_collections.resize(numColl);
  
  if (log.level() <= MSG::DEBUG) 
    log << MSG::DEBUG<< " Preparing " << persCont->m_collections.size() << "Collections" <<endmsg;
  
  for (pcollIndex = 0; it_Coll != it_CollEnd; ++pcollIndex, ++it_Coll)  {
    // Add in new collection
    if (log.level() <= MSG::DEBUG) 
      log << MSG::DEBUG<<"New collection"<<endmsg;
    const Muon::sTgcPrepDataCollection& collection = (**it_Coll);
    Muon::MuonPRD_Collection_p2& pcollection = persCont->m_collections[pcollIndex]; //get ref to collection we're going to fill
    
    pcollBegin  = pcollEnd; // Next collection starts at end of previous one.
    pcollEnd   += collection.size();
    
    pcollection.m_hashId = collection.identifyHash(); 
    pcollection.m_id = collection.identify().get_identifier32().get_compact();
    pcollection.m_size = collection.size();
    
    // Add in channels
    persCont->m_prds.resize(pcollEnd); 
    persCont->m_prdDeltaId.resize(pcollEnd);
    
    unsigned int lastPRDIdHash = 0;
    for (unsigned int i = 0; i < collection.size(); ++i) {
      unsigned int pchanIndex=i+pcollBegin;

      const sTgcPrepData* chan = collection[i]; // channel being converted
      sTgcPrepData_p1*   pchan = &(persCont->m_prds[pchanIndex]); // persistent version to fill

      chanCnv.transToPers(chan, pchan, log); // convert from sTgcPrepData to sTgcPrepData_p1

      // persCont->m_prdDeltaId is of data type unsigned short, thus we need to encode the channel (starting from the
      // collection (module) is in contained) into 16 bits, we do it by storing multilayer, gasGap and channel  
      int multilayer  = (m_sTgcId->multilayer(chan->identify())-1); // ranges between 1-2 (1bit)
      int gasGap      = (m_sTgcId->gasGap(chan->identify())-1); // ranges between 1-4 (2bits)
      int channelType = (m_sTgcId->channelType(chan->identify())); // ranges between 0-2 (2bits)
      int channel     = (m_sTgcId->channel(chan->identify())-m_sTgcId->channelMin()); 

      // created an unsigned short and store multilayer, gasGap and channel by bit-shifts   
      unsigned short diff = ( channel << 5 | channelType << 3 | gasGap << 1 | multilayer) ; 
      log << MSG::DEBUG  << "Translated id=" << chan->identify().get_compact() << " (multilayer=" << multilayer 
	  << ", gasGap << " << gasGap << ", channel=" << channel << ") into diff=" << diff << endmsg;


      persCont->m_prdDeltaId[pchanIndex]=diff; //store delta identifiers, rather than full identifiers      
      
      if(log.level() <= MSG::DEBUG){
	Identifier temp(pcollection.m_id + persCont->m_prdDeltaId[pchanIndex]);
	if (temp!=chan->identify() ) 
	  log << MSG::WARNING << "PRD ids differ! Transient:"<<chan->identify()<<", From persistent:"<<temp<<" diff = "<<chan->identify().get_compact()-temp.get_compact()<<endmsg;
	else 
	  log << MSG::DEBUG <<" PRD ids match."<<endmsg;
	if (lastPRDIdHash && lastPRDIdHash != chan->collectionHash() )  log << MSG::WARNING << "Collection Identifier hashes differ!"<<endmsg;
	lastPRDIdHash = chan->collectionHash();
	log << MSG::DEBUG<<"Collection hash = "<<lastPRDIdHash<<endmsg;
        
	if (chan->collectionHash()!= collection.identifyHash() ) log << MSG::WARNING << "Collection's idHash does not match PRD collection hash!"<<endmsg;
	if (chan->detectorElement() !=getReadOutElement(chan->identify())) 
	  log << MSG::WARNING << "Getting de from identity didn't work!"<<endmsg;
	else 
	  log << MSG::DEBUG<<"Getting de from identity did work "<<endmsg;
	if (chan->detectorElement() !=getReadOutElement(temp)) log << MSG::WARNING << "Getting de from reconstructed identity didn't work!"<<endmsg;
	log << MSG::DEBUG<<"Finished loop"<<endmsg;
      }
    }
  }
  if (log.level() <= MSG::DEBUG) 
    log << MSG::DEBUG<< " ***  Writing sTgcPrepDataContainer ***" <<endmsg;
}

void  Muon::sTgcPrepDataContainerCnv_p1::persToTrans(const Muon::sTgcPrepDataContainer_p1* persCont, Muon::sTgcPrepDataContainer* transCont, MsgStream &log) 
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
  
  Muon::sTgcPrepDataCollection* coll = nullptr;
  
  sTgcPrepDataCnv_p1  chanCnv;
  unsigned int pchanIndex(0); // position within persCont->m_prds. Incremented inside innermost loop 
  unsigned int pCollEnd = persCont->m_collections.size();
  if (log.level() <= MSG::DEBUG) 
    log << MSG::DEBUG<< " Reading " << pCollEnd << "Collections" <<endmsg;
  for (unsigned int pcollIndex = 0; pcollIndex < pCollEnd; ++pcollIndex) {
    const Muon::MuonPRD_Collection_p2& pcoll = persCont->m_collections[pcollIndex];        
    IdentifierHash collIDHash(pcoll.m_hashId);
    coll = new Muon::sTgcPrepDataCollection(collIDHash);
    coll->setIdentifier(Identifier(pcoll.m_id)); 
    
    unsigned int pchanEnd = pchanIndex+pcoll.m_size;
    unsigned int chanIndex = 0; // transient index
    
    coll->reserve(pcoll.m_size);
    // Fill with channels
    for (; pchanIndex < pchanEnd; ++ pchanIndex, ++chanIndex) {
      const sTgcPrepData_p1* pchan = &(persCont->m_prds[pchanIndex]);
      
      /// get the cluster Identifier from the collection Identifier and the persCont->m_prdDeltaId (unsigned short)
      unsigned short diff = persCont->m_prdDeltaId[pchanIndex];
      // we need to redo the bit-shift to retrieve channel, gasGap and multilayer
      int channel = (diff>>5);
      int channelType = ( 3 & (diff>>3) );
      int gasGap = ( 3 & (diff>>1));
      int multilayer = ( 1 & diff );
      Identifier clusId = m_sTgcId->channelID(Identifier(pcoll.m_id), multilayer+1, gasGap+1, 
					      channelType, channel+m_sTgcId->channelMin());
      log << MSG::DEBUG  << "Diff of " << diff << " translated into multilayer=" 
	  << multilayer << ", gasGap=" << gasGap << ", channelType=" << channelType 
	  << ", channel=" << channel << " -> id=" << clusId.get_compact() << endmsg;

	if ( m_sTgcId->valid(clusId)!=true ) {
	  // have invalid PRD
	  log << MSG::WARNING  << "Tgc PRD has invalid Identifier of "<< m_sTgcId->show_to_string(clusId)<< " - are you sure you have the correct geometry loaded, and NSW enabled?"<<endmsg;
	} 
	
	// The reason I need to do the following is that one collection can have several detector elements in, the collection hashes!=detector element hashes
	IdentifierHash deIDHash;
	int result = m_sTgcId->get_detectorElement_hash(clusId, deIDHash);
	if (result&&log.level() <= MSG::WARNING) 
	  log << MSG::WARNING<< " Muon::sTgcPrepDataContainerCnv_p1::persToTrans: problem converting Identifier to DE hash "<<endmsg;
	const MuonGM::sTgcReadoutElement* detEl =
	  getReadOutElement(clusId); 
	if (!detEl) {
	  log << MSG::WARNING<< "Muon::sTgcPrepDataContainerCnv_p1::persToTrans: could not get valid det element for PRD with id="<<clusId<<". Skipping."<<endmsg;
	  continue;
	}
        
	auto chan = std::make_unique<sTgcPrepData>
	  (chanCnv.createsTgcPrepData (pchan,
				       clusId,
				       detEl,
				       log));
	
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
      log << MSG::DEBUG<< " ***  Reading sTgcPrepDataContainer ***" << endmsg;
}



//================================================================
Muon::sTgcPrepDataContainer* Muon::sTgcPrepDataContainerCnv_p1::createTransient(const Muon::sTgcPrepDataContainer_p1* persObj, MsgStream& log) 
{
    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize sTgcPrepDataContainerCnv_p1 " << endmsg;
            return nullptr;
        } 
    }
    std::unique_ptr<Muon::sTgcPrepDataContainer> trans(new Muon::sTgcPrepDataContainer(m_sTgcId->module_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


