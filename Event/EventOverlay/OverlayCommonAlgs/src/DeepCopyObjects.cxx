/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//// To deep copy various transient objects from one storegate to another
//// Piyali.Banerjee@cern.ch; September 2010
/////////////////////////////////////////////////////////////////

#include "DeepCopyObjects.h"

DeepCopyObjects::DeepCopyObjects(const std::string &name, ISvcLocator *pSvcLocator) :
  OverlayAlgBase(name, pSvcLocator), 
  m_storeGateTo("StoreGateSvc/BkgEvent_2_SG", name),
  m_storeGateFrom(m_storeGateData)
{
  declareProperty("ToStore", m_storeGateTo, "help");
  declareProperty("FromStore", m_storeGateFrom, "help");
  declareProperty("EvtInfoObjects", m_evtinfo=false);
  declareProperty("InDetObjects", m_indet=false);
  declareProperty("MuonObjects", m_muon=false);
  declareProperty("TruthObjects", m_truth=false);
  declareProperty("LArObjects",m_lar=false);
  declareProperty("TileObjects",m_tile=false);
  declareProperty("Level1Objects",m_level1=false);
}

template <class TypeToBeCopied> void 
DeepCopyObjects::printEveryObjectOfType(StoreGateSvc* stg)
{
   MsgStream log(msgSvc(), name());

   const std::string templateClassName = typeid(TypeToBeCopied).name();

   typedef std::vector<std::string> KeyList;
   KeyList keys = stg->keys<TypeToBeCopied>();

   if(keys.empty()) {
     log << MSG::WARNING << "printEveryObjectOfType<" << templateClassName
         << ">(): no keys found" << endmsg;
   }

   std::cout << "printEveryObjectOfType<" << templateClassName
             << ">(): Begin printing " << stg->name() << std::endl;

   for(KeyList::const_iterator k=keys.begin(); k!=keys.end(); ++k) {
     const TypeToBeCopied* object = 0;
     if (stg->retrieve(object, *k).isSuccess() ) { 
         std::cout << "Key == " << *k << std::endl;
         std::cout << *object << std::endl;
     } 
     else { 
         log << MSG::WARNING << "printEveryObjectOfType<"
             << templateClassName
             << ">(): problem retrieving object with pointer="
             << object << ", key=" << *k << endmsg;
     } 

   }

   std::cout << "printEveryObjectOfType<" << templateClassName
             << ">(): End printing " << stg->name() << std::endl;
}

template<class TypeToBeCopied> void 
DeepCopyObjects::copyMuonObjectRDO(StoreGateSvc* to, StoreGateSvc *from) 
{

  MsgStream log(msgSvc(), name());

  const int classNum = ClassID_traits<TypeToBeCopied>::ID();
  const std::string templateClassName = typeid(TypeToBeCopied).name();

  typedef std::vector<std::string> KeyList; 
  KeyList keys = from->keys<TypeToBeCopied>(); 

  if(keys.empty()) { 
     log << MSG::WARNING << "copyMuonobjectRDO<" << templateClassName
         << ">(): no keys found" << endmsg; 
  } 

  for (KeyList::const_iterator k=keys.begin(); k!=keys.end(); ++k) {  
    if (classNum == 4183) { // CscRawDataContainer
       const CscRawDataContainer *oldObject = 0;
       if (from->retrieve(oldObject, *k).isFailure()){  
          log << MSG::WARNING << "copyMuonObjectRDO<" << templateClassName 
              <<">(): problem retrieving container with key = "
              << *k << endmsg;  
          continue;  
       }  
      
       CscRawDataContainer *newObject = new CscRawDataContainer (oldObject->size()); 

       CscRawDataContainer::const_iterator iFirst = oldObject->begin();
       CscRawDataContainer::const_iterator iLast  = oldObject->end();

      // since CscRawDataCollection in OriginalEvent_SG has absurd identifierHash values
      // we will use collectionNo variable as an index into the 
      // DataLinkVector inside CscRawDataContainer
       int collectionNo = 0; 
       for (; iFirst != iLast; ++iFirst ) { 
          CscRawDataCollection *element = new CscRawDataCollection ((*iFirst)->identify()); 
          element->setIdentifyHash((*iFirst)->identifyHash());
          element->setRodId((*iFirst)->rodId());
          element->setSubDetectorId((*iFirst)->subDetectorId());

          if ((*iFirst)->samplingPhase()){
             element->set_samplingPhase();
          }

          if ((*iFirst)->triggerType()){
             element->set_triggerType();
          }

          element->set_firstBitSummary((*iFirst)->firstBitSummary());
          element->set_eventType((*iFirst)->eventType());

          uint16_t numRPU = ((*iFirst)->rpuID()).size();
          for (uint16_t numRPU_i = 0; numRPU_i != numRPU; numRPU++){
             element->addRPU(((*iFirst)->rpuID())[numRPU_i]);
          }

          uint16_t numDataType = ((*iFirst)->dataType()).size();
          for (uint16_t numDataType_i = 0; numDataType_i != numDataType; numDataType_i++){
             element->addDataType(((*iFirst)->dataType())[numDataType_i]);
          }

         // Default SpuCount size is 10
          uint16_t numSpuCount = 10;
          for (uint16_t numSpuCount_i = 0; numSpuCount_i != numSpuCount; numSpuCount_i++){
             element->set_spuCount(numSpuCount_i, (*iFirst)->spuCount(numSpuCount_i));
          }

          element->set_scaAddress((*iFirst)->scaAddress());

          CscRawDataCollection::const_iterator firstData = (*iFirst)->begin();
	  CscRawDataCollection::const_iterator lastData = (*iFirst)->end();
          for (; firstData != lastData; ++firstData) { 
	        CscRawData *newData = new CscRawData (**firstData); 
	        element->push_back(newData); 
	  } 
	  if (newObject->addCollection(element, collectionNo).isFailure()) { 
	     log << MSG::WARNING << "copyMuonObjectRDO<" << templateClassName
                 << ">(): problem adding collection with vector index = "
                 << collectionNo << endmsg; 
	  }  
          collectionNo++;
       }

        if (!to->record(newObject, *k).isSuccess()) {
           log << MSG::WARNING << "copyMuonObjectRDO<" << templateClassName
               << ">(): problem recording new container with key = " 
               << *k << endmsg;
        }
    } 
    else if (classNum == 4187){ // MdtCsmContainer
      const MdtCsmContainer *oldObject = 0;
      if (from->retrieve(oldObject, *k).isFailure()){  
	log << MSG::WARNING << "copyMuonObjectRDO<" << templateClassName
            << ">(): problem retrieving container with key = "
            << *k << endmsg;  
	continue;  
      }  
      
      MdtCsmContainer *newObject = new MdtCsmContainer (oldObject->size()); 
      
      MdtCsmContainer::const_iterator iFirst = oldObject->begin();
      MdtCsmContainer::const_iterator iLast  = oldObject->end();
      
      for (; iFirst != iLast; ++iFirst ) { 
	MdtCsm *element = new MdtCsm ((*iFirst)->identify(), (*iFirst)->identifyHash(),
				      (*iFirst)->SubDetId(), (*iFirst)->MrodId(),
				      (*iFirst)->CsmId()); 
	
	MdtCsm::const_iterator firstData = (*iFirst)->begin();
	MdtCsm::const_iterator lastData = (*iFirst)->end();
	for (; firstData != lastData; ++firstData) { 
	    MdtAmtHit *newData = 
            new MdtAmtHit ((*firstData)->tdcId(), (*firstData)->channelId(), 
                           (*firstData)->isMasked()); 
            newData->setValues((*firstData)->coarse(), (*firstData)->fine(),
                               (*firstData)->width());
            newData->setLeading((*firstData)->leading());

            for (unsigned int i = 0; i < ((*firstData)->dataWords())->size(); i++) {
                newData->addData((*((*firstData)->dataWords()))[i]);
            }
	    element->push_back(newData); 
	} 
	if (newObject->addCollection(element, (*iFirst)->identifyHash()).isFailure()) {
	  log << MSG::WARNING << "copyMuonObjectRDO<" << templateClassName
              << ">(): problem adding collection with hash = "
              << (*iFirst)->identifyHash() << endmsg;
	} 
      }
      
      if (!to->record(newObject, *k).isSuccess()) {
	log << MSG::WARNING << "copyMuonObjectRDO<" << templateClassName
            << ">(): problem recording new container with key = "
            <<*k << endmsg;
      }
    }  
    else if (classNum == 4190){  // RpcPadContainer
      const RpcPadContainer *oldObject = 0; 
      if (from->retrieve(oldObject, *k).isFailure()){   
	log << MSG::WARNING << "copyMuonObjectRDO<" << templateClassName
            <<">(): problem retrieving container with key = "<< *k 
            << endmsg;   
	continue;   
      }   

      RpcPadContainer *newObject = new RpcPadContainer (oldObject->size());  
      
      RpcPadContainer::const_iterator iFirst = oldObject->begin(); 
      RpcPadContainer::const_iterator iLast  = oldObject->end(); 
      
      for (; iFirst != iLast; ++iFirst ) {  
	RpcPad *element = new RpcPad (
          (*iFirst)->identify(), (*iFirst)->identifyHash(), 
          (*iFirst)->onlineId(), (*iFirst)->lvl1Id(), (*iFirst)->bcId(), 
          (*iFirst)->status(), (*iFirst)->errorCode(), (*iFirst)->sector()
        );  
      
	RpcPad::const_iterator firstData = (*iFirst)->begin(); 
	RpcPad::const_iterator lastData = (*iFirst)->end(); 
	for (; firstData != lastData; ++firstData) {  
	    RpcCoinMatrix *newData = new RpcCoinMatrix (
               (*firstData)->identify(), (*firstData)->onlineId(),
               (*firstData)->crc(), (*firstData)->fel1Id(),
               (*firstData)->febcId());  

	    RpcCoinMatrix::const_iterator iter = (*firstData)->begin(); 
	    RpcCoinMatrix::const_iterator lastIter = (*firstData)->end(); 
	    for (; iter != lastIter; ++iter) {  
                RpcFiredChannel *channel = new RpcFiredChannel (**iter);
                newData->push_back(channel);
            }

	    element->push_back(newData);  
	}  
	if (newObject->addCollection(element, 
               (*iFirst)->identifyHash()).isFailure()) { 
	  log << MSG::WARNING << "copyMuonObjectRDO<"
              << templateClassName 
              << ">(): problem adding collection with hash = "
              << (*iFirst)->identifyHash() << endmsg; 
	}  
      } 
   
      if (to->record(newObject, *k).isFailure()) { 
	log << MSG::WARNING << "copyMuonObjectRDO<"
            << templateClassName
            << ">(): problem recording new container with "
            << "key=" << *k << endmsg; 
      } 
    } 
    else if (classNum == 1298668563){ //RpcSectorLogicContainer
      const RpcSectorLogicContainer *oldObject = 0; 
      if (from->retrieve(oldObject, *k).isFailure()){   
	log << MSG::WARNING << "copyMuonObjectRDO<" << templateClassName
            << ">(): problem retrieving container with key = "
            << *k << endmsg;   
	continue;   
      }   
    
      RpcSectorLogicContainer *newObject = new RpcSectorLogicContainer ();  
      
      RpcSectorLogicContainer::const_iterator iFirst = oldObject->begin(); 
      RpcSectorLogicContainer::const_iterator iLast  = oldObject->end(); 
    
      for (; iFirst != iLast; ++iFirst ) {  
	RpcSectorLogic *element = new RpcSectorLogic ((*iFirst)->sectorId(), (*iFirst)->fel1Id(), 
						      (*iFirst)->bcid(), (*iFirst)->errorCode(), 
						      (*iFirst)->crc());  
        element->setHasMoreThan2TriggerCand((*iFirst)->hasMoreThan2TriggerCand());

        uint16_t triggerRates_size = ((*iFirst)->triggerRates()).size();
        for (uint16_t triggerRates_size_i = 0; triggerRates_size_i != triggerRates_size; triggerRates_size_i++){
           element->addTriggerRate(((*iFirst)->triggerRates())[triggerRates_size_i]);
        }
	
        uint16_t counters_size = ((*iFirst)->counters()).size();
        for (uint16_t counters_size_i = 0; counters_size_i != counters_size; counters_size_i++){
           element->addCounter(((*iFirst)->counters())[counters_size_i]);
        }
	
	RpcSectorLogic::const_iterator firstData = (*iFirst)->begin(); 
	RpcSectorLogic::const_iterator lastData = (*iFirst)->end(); 
	for (; firstData != lastData; ++firstData) {  
	    RpcSLTriggerHit *newData = new RpcSLTriggerHit (**firstData);  
	    element->push_back(newData);  
	}  
	newObject->push_back(element);
      } 
    
      if (!to->record(newObject, *k).isSuccess()) { 
	log << MSG::WARNING << "copyMuonObjectRDO<" << templateClassName
            << ">(): problem recording new container with key = "
            << *k << endmsg; 
      } 
    }
    else if (classNum == 4186){ // TgcRdoContainer
      const TgcRdoContainer *oldObject = 0; 
      if (from->retrieve(oldObject, *k).isFailure()){   
	log << MSG::WARNING << "copyMuonObjectRDO<" << templateClassName
            << ">(): problem retrieving container with key = "
            << *k << endmsg;   
	continue;   
      }   
      
      TgcRdoContainer *newObject = new TgcRdoContainer (oldObject->size());  
    
      TgcRdoContainer::const_iterator iFirst = oldObject->begin(); 
      TgcRdoContainer::const_iterator iLast  = oldObject->end(); 
    
      // since TgcRdo in OriginalEvent_SG has absurd identifierHash values
      // we will use collectionNo variable as an index into the 
      // DataLinkVector inside TgcRdoContainer
      int collectionNo = 0; 
      for (; iFirst != iLast; ++iFirst ) {  
	TgcRdo *element = new TgcRdo((*iFirst)->identify(), (*iFirst)->identifyHash());
        new (element) TgcRdo((*iFirst)->subDetectorId(), (*iFirst)->rodId(), 
                             (*iFirst)->bcId(), (*iFirst)->l1Id());  
	element->setVersion((*iFirst)->version());
	element->setTriggerType((*iFirst)->triggerType());
	element->setErrors(*(uint16_t *)((void *)(&(*iFirst)->errors())));
	element->setRodStatus(*(uint32_t *)((void *)(&(*iFirst)->rodStatus())));
	element->setLocalStatus(*(uint32_t *)((void *)(&(*iFirst)->localStatus())));
	element->setOrbit((*iFirst)->orbit());
	
	TgcRdo::const_iterator firstData = (*iFirst)->begin(); 
	TgcRdo::const_iterator lastData = (*iFirst)->end(); 
	for (; firstData != lastData; ++firstData) {  
	    TgcRawData *newData = new TgcRawData (**firstData);  
	    element->push_back(newData);  
	}  
	if (newObject->addCollection(element, collectionNo).isFailure()) { 
	  log << MSG::WARNING << "copyMuonObjectRDO<" << templateClassName
              << ">(): problem adding collection with vector index = "
              << collectionNo << endmsg; 
	}  
        collectionNo++;
      } 
    
      if (!to->record(newObject, *k).isSuccess()) {
	log << MSG::WARNING << "copyMuonObjectRDO<" << templateClassName
            << ">(): problem recording new container with key = "
            << *k << endmsg;
      }
    }
  }
}

/*
template<class TypeToBeCopied, class Datum>
void DeepCopyObjects::copyMuonObjectDigit(StoreGateSvc* from, StoreGateSvc *to) {
     
  const std::string templateClassName = typeid(TypeToBeCopied).name();

  typedef typename TypeToBeCopied::base_value_type Collection;

  typedef std::vector<std::string> KeyList; 

  KeyList keys = from->keys<TypeToBeCopied>(); 
  if(keys.empty()) { 
     ATH_MSG_WARNING("copyMuonobject<"<<templateClassName<<">(): no keys found"); 
  } 

  for (KeyList::const_iterator k=keys.begin(); k!=keys.end(); ++k) {  
      const TypeToBeCopied *oldObject = 0;
      if (from->retrieve(oldObject, *k).isFailure()) {
         ATH_MSG_WARNING("copyMuonobject<"<<templateClassName<<">(): problem retrieving container with " <<"key="<<*k);
         continue;
      }

      TypeToBeCopied *newObject = new TypeToBeCopied (oldObject->size());

      typename TypeToBeCopied::const_iterator iFirst = oldObject->begin();
      typename TypeToBeCopied::const_iterator iLast = oldObject->end();

      for (; iFirst != iLast; ++iFirst) {
         Collection *element = new Collection ((*iFirst)->identify(), (*iFirst)->identifierHash());
	
	 typename Collection::const_iterator firstData = (*iFirst)->begin();
	 typename Collection::const_iterator lastData = (*iFirst)->end();
	 for (; firstData != lastData; ++firstData) {
	    Datum *newData = new Datum(*(dynamic_cast<const Datum*>(*firstData)));
	    element->push_back(newData);
	 }
	 if (newObject->addCollection (element, (*iFirst)->identifierHash()).isFailure()) {
	   ATH_MSG_WARNING("copyMuonobject<"<<templateClassName<<">(): problem adding collection with hash "<<"hash="<<(*iFirst)->identifierHash());
	 }
      }
    
      if (!to->record(newObject, *k).isSuccess()) { 
           ATH_MSG_WARNING("copyMuonobject<"<<templateClassName<<">(): problem recording new container with "<<"key="<<*k); 
      } 
  }
}


template<class TypeToBeCopied> void 
DeepCopyObjects::copyInDetObject(StoreGateSvc* to, StoreGateSvc *from) 
{ }

template<class TypeToBeCopied> void 
DeepCopyObjects::copyTruthObject(StoreGateSvc* to, StoreGateSvc *from) 
{ }

template<class TypeToBeCopied> void 
DeepCopyObjects::copyLArObject(StoreGateSvc* to, StoreGateSvc *from) 
{ }

template<class TypeToBeCopied> void 
DeepCopyObjects::copyTileObject(StoreGateSvc* to, StoreGateSvc *from) 
{ }

template<class TypeToBeCopied> void 
DeepCopyObjects::copyLevel1Object(StoreGateSvc* to, StoreGateSvc *from) 
{ }

*/

//================================================================
StatusCode DeepCopyObjects::overlayInitialize()
{

  if (m_storeGateTo.retrieve().isFailure()) {
     ATH_MSG_FATAL("DeepCopyObjects::initialize): ToStore service not found !");
     return StatusCode::FAILURE;
  } 

  if (m_storeGateFrom.retrieve().isFailure()) {
    ATH_MSG_FATAL("DeepCopyObjects::initialize): FromStoreGate service not found !");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode DeepCopyObjects::overlayFinalize() 
{
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode DeepCopyObjects::overlayExecute() {
  MsgStream log(msgSvc(), name());

  //std::cout << "DeepCopyObjects::execute() begin"<< std::endl;
  //std::cout << m_storeGateMC->dump() << std::endl;
  //std::cout << m_storeGateData->dump() << std::endl;
  //std::cout << m_storeGateOutput->dump() << std::endl;
  //std::cout << m_storeGateFrom->dump() << std::endl;
  //std::cout << m_storeGateTo->dump() << std::endl;

  if(m_evtinfo == true){
    typedef std::vector<std::string> KeyList; 
    KeyList keys = m_storeGateFrom->keys<EventInfo>(); 
    if(keys.empty()) { 
      log << MSG::WARNING << "no object of type EventInfo in source store " 
          << m_storeGateFrom->name() << endmsg; 
    } 

    for (KeyList::const_iterator k=keys.begin(); k!=keys.end(); ++k) {  
       const EventInfo *oldEvtInfo = 0; 
       if (m_storeGateFrom->retrieve(oldEvtInfo, *k).isFailure()) {
          log << MSG::WARNING << "could not retrieve object with " 
              << "key = " << *k << " from store " 
              << m_storeGateFrom->name() << endmsg; 
          continue;
        }

        EventInfo * newEvtInfo = new EventInfo (*oldEvtInfo);     
        if (m_storeGateTo->record(newEvtInfo, *k).isFailure()) {  
          log << MSG::WARNING 
              << "could not record EventInfo to target store "
              << m_storeGateTo->name() << " with key= " 
              << *k << endmsg; 
        }
    } 
  }


/*

  if(m_indet){
    copyInDetObject<TRT_RDO_Container>(&*m_storeGateTo, &*m_storeGateFrom);
    copyInDetObject<SCT_RDO_Container>(&*m_storeGateTo, &*m_storeGateFrom);
    copyInDetObject<PixelRDO_Container>(&*m_storeGateTo, &*m_storeGateFrom);
  }

  if(m_truth){
    copyTruthObject<McEventCollection>(&*m_storeGateTo, &*m_storeGateFrom);
    copyTruthObject<TrackRecordCollection>(&*m_storeGateTo, &*m_storeGateFrom);
    copyTruthObject<CaloCalibrationHitContainer>(&*m_storeGateTo, &*m_storeGateFrom);
    copyTruthObject<InDetSimDataCollection>(&*m_storeGateTo, &*m_storeGateFrom);
    copyTruthObject<MuonSimDataCollection>(&*m_storeGateTo, &*m_storeGateFrom);
    copyTruthObject<CscSimDataCollection>(&*m_storeGateTo, &*m_storeGateFrom);
  }

  if(m_lar){
    copyLArObject<LArDigitContainer>(&*m_storeGateTo, &*m_storeGateFrom);
    copyLArObject<LArRawChannelContainer>(&*m_storeGateTo, &*m_storeGateFrom);
  }

  if(m_tile){
    copyTileObject<TileDigitsContainer>(&*m_storeGateTo, &*m_storeGateFrom);
    copyTileObject<TileRawChannelContainer>(&*m_storeGateTo, &*m_storeGateFrom);
    copyTileObject<TileL2Container>(&*m_storeGateTo, &*m_storeGateFrom);
  }

  if(m_muon){

    copyMuonObjectRDO<MdtCsmContainer>(&*m_storeGateTo, &*m_storeGateFrom);
//    printEveryObjectOfType<MdtCsmContainer>(&*m_storeGateFrom);
//    printEveryObjectOfType<MdtCsmContainer>(&*m_storeGateTo);

    copyMuonObjectRDO<TgcRdoContainer>(&*m_storeGateTo,&*m_storeGateFrom);
//    printEveryObjectOfType<TgcRdoContainer>(&*m_storeGateFrom);
//    printEveryObjectOfType<TgcRdoContainer>(&*m_storeGateTo);

    copyMuonObjectRDO<RpcPadContainer>(&*m_storeGateTo,&*m_storeGateFrom);
//    printEveryObjectOfType<RpcPadContainer>(&*m_storeGateFrom);
//    printEveryObjectOfType<RpcPadContainer>(&*m_storeGateTo);

    copyMuonObjectRDO<CscRawDataContainer>(&*m_storeGateTo, &*m_storeGateFrom);
//    printEveryObjectOfType<CscRawDataContainer>(&*m_storeGateFrom);
//    printEveryObjectOfType<CscRawDataContainer>(&*m_storeGateTo);

    //copyMuonObjectRDO<RpcSectorLogicContainer>(&*m_storeGateTo, &*m_storeGateFrom);

  }

// Copy level 1 objects 
  
  if(m_level1){
    copyLevel1Object<LArTTL1Container>(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object<TileTTL1Container>(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object<ROIB::RoIBResult>(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object<MuCTPI_RDO>(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object<CTP_RDO>(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object< DataVector<LVL1::TriggerTower> >(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object< DataVector<LVL1::CPMTower> >(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object< DataVector<LVL1::JetElement> >(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object< DataVector<LVL1::CPMHits> >(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object< DataVector<LVL1::CPMRoI> >(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object< DataVector<LVL1::CMMCPHits> >(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object< DataVector<LVL1::CMMJetHits> >(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object< DataVector<LVL1::CMMEtSums> >(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object< DataVector<LVL1::JEMHits> >(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object< DataVector<LVL1::JEMRoI> >(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object< DataVector<LVL1::JEMEtSums> >(&*m_storeGateTo, &*m_storeGateFrom);
    copyLevel1Object<LVL1::CMMRoI>(&*m_storeGateTo, &*m_storeGateFrom);
  }

*/

  if(m_muon){
    copyMuonObjectRDO<CscRawDataContainer>(&*m_storeGateTo, &*m_storeGateFrom);
  }

  //std::cout << "DeepCopyObjects::execute() end"<< std::endl;
  //std::cout << m_storeGateMC->dump() << std::endl;
  //std::cout << m_storeGateData->dump() << std::endl;
  //std::cout << m_storeGateOutput->dump() << std::endl;
  //std::cout << m_storeGateFrom->dump() << std::endl;
  //std::cout << m_storeGateTo->dump() << std::endl;

  return StatusCode::SUCCESS;
}

//================================================================
//EOF
