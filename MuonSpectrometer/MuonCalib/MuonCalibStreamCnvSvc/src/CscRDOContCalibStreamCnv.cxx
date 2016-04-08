/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStreamCnvSvc/CscRDOContCalibStreamCnv.h"
#include "MuonCalibStreamCnvSvc/MuonCalibStreamAddress.h" 
#include "MuonCalibStreamCnvSvc/MuonCalibStreamDataProviderSvc.h" 
#include "MuCalDecode/CalibEvent.h" 
#include "MuCalDecode/CalibData.h" 
#include "MuCalDecode/CalibUti.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "EventInfo/TagInfo.h"

#include "StoreGate/StoreGateSvc.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "MuonCSC_CnvTools/ICSC_ROD_Decoder.h"

#include <iostream>
#include <map> 
#include <list> 

using namespace  LVL2_MUON_CALIBRATION;

CscRDOContCalibStreamCnv::CscRDOContCalibStreamCnv(ISvcLocator* svcloc) :
  Converter(MuonCalibStream_StorageType, classID(),svcloc),
  m_cabling("CSCcablingSvc", "CscRDOContCalibStreamCnv"),
  m_rdoContainer(NULL), m_cscRodDecoderTool("Muon::CscROD_Decoder"),
  m_isClusterWordsUnrealistic(false),
  m_numMissingRPUs(0) {
  MsgStream log ( messageService(),"CscRDOContCalibStreamCnv") ;
  log << MSG::INFO<< " construct " <<endreq; 
}

CscRDOContCalibStreamCnv::~CscRDOContCalibStreamCnv() {
  std::cout << "Number of misssing CSC RPUs is " << m_numMissingRPUs;
}

const CLID& CscRDOContCalibStreamCnv::classID(){
  return ClassID_traits<CscRawDataContainer>::ID() ;
}
/*
int CscRDOContCalibStreamCnv::getRodIdFromSectorId(int tmp_sectorId){
  return (((tmp_sectorId-1)%12) + 1);
}

uint16_t CscRDOContCalibStreamCnv::bcTagCnv(uint16_t bcBitMap){
  return (bcBitMap == 4 ? 1:
      (bcBitMap == 2 ? 2:
       (bcBitMap == 1 ? 3:
        0)));
}
*/

StatusCode CscRDOContCalibStreamCnv::initialize() {

  StatusCode sc = Converter::initialize(); 
  if (StatusCode::SUCCESS!=sc) { 
    return sc; 
  } 

  MsgStream log(messageService(), "CscRDOContCalibStreamCnv");
  log << MSG::INFO << " initializing CscRDOContCalibStreamCnv " <<endreq; 

  // Retrieve the CscIdHelper
  // and get the detector descriptor manager
  sc = serviceLocator()->service("DetectorStore", m_detStore);
  if (sc.isSuccess()) {      
    sc = m_detStore->retrieve(m_cscIdHelper, "CSCIDHELPER" );
    if (sc.isFailure()) {
      log << MSG::FATAL << " Cannot retrieve CscIdHelper " << endreq;
      return sc;
    }
    sc = m_detStore->retrieve( m_muonMgr );
    if (sc.isFailure()) {
      log << MSG::FATAL << " Cannot retrieve MuonGeoModel " << endreq;
      return sc;
    }
  } else {
    log << MSG::ERROR << " Can't locate DetectorStore " << endreq;  
    return sc;
  }

  // get CSC cablingSvc
  if(m_cabling.retrieve().isFailure()) {
    log << MSG::ERROR << "Can't retrieve cabling service" << endreq;
    return StatusCode::FAILURE;
  }
  /*const ICSCcablingServerSvc* pCscCabGet = NULL;
    if (service("CSCcablingServerSvc", pCscCabGet).isFailure())
    {
    log << MSG::FATAL << "Unable to retrieve the CSC cabling Server" << endreq;
    return StatusCode::FAILURE;
    }
    if (pCscCabGet->giveCabling(m_cabling).isFailure())
    {
    log << MSG::FATAL << "Unable to retrieve the CSC cabling Service from the Server" << endreq;
    m_cabling = 0;
    return StatusCode::FAILURE;
    }
    else {
    log << MSG::INFO << " Found the CSCcablingSvc. " << endreq;
    }*/

  // retreive also the dataProviderSvc
  IService* svc ;
  sc = serviceLocator()->getService("MuonCalibStreamDataProviderSvc", svc);
  if(sc != StatusCode::SUCCESS ) {
    log<<MSG::ERROR << " Cant get MuonCalibStreamDataProviderSvc " <<endreq;
    return sc ;
  }

  m_dataProvider = dynamic_cast<MuonCalibStreamDataProviderSvc*> (svc);
  if(m_dataProvider == 0 ) {
    log<<MSG::ERROR<< " Cant cast to MuonCalibStreamDataProviderSvc " <<endreq;                                                                                
    return StatusCode::FAILURE ;
  }

  if(m_cscRodDecoderTool.retrieve().isFailure()) {
    log << MSG::ERROR << "Can't retrieve cscRdodecoderTool" << endreq;
    return StatusCode::FAILURE;
  }

  // create an empty rdo container and record it
  m_rdoContainer = new CscRawDataContainer(m_muonMgr->cscIdHelper()->module_hash_max()); 
  m_rdoContainer->addRef();


  return service("StoreGateSvc", m_storeGate); 
}

  StatusCode
CscRDOContCalibStreamCnv::createObj(IOpaqueAddress* /*pAddr*/, DataObject*& pObj) {
  MsgStream log(messageService(), "CscRDOContCalibStreamCnv");

  StatusCode sc;

  log << MSG::DEBUG << "CSC RDO create Obj" << endreq;

  if(m_rdoContainer) {
    log << MSG::DEBUG << "Cleaning up rdo container" << endreq;
    m_rdoContainer->cleanup();
  } else {    
    // should a RodContainer be created??
    log << MSG::ERROR << "Not making container"  << endreq;
    return StatusCode::FAILURE;
  }
  // register it in StoreGate.
  std::string key = "CSCRDO";
  sc = m_storeGate->record(m_rdoContainer,key);
  if (sc.isFailure()) log << MSG::ERROR << "Fail to record CSC RDO container in TDS" << endreq;

  sc=fillCollections(pObj); 
  if(sc==StatusCode::FAILURE) return sc; 
  pObj = StoreGateSvc::asStorable( m_rdoContainer );
  return sc; 
}

StatusCode 
CscRDOContCalibStreamCnv::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/) {
  MsgStream log(messageService(), "CscRDOContCalibStreamCnv");
  log << MSG::ERROR << " No conversion CscRDO to stream " << endreq ; 
  return   StatusCode::FAILURE ; 
}

//fillCollections() highly based on CscROD_Decoder.
StatusCode CscRDOContCalibStreamCnv::fillCollections(DataObject*& pObj) {
  MsgStream log(messageService(), "CscRDOContCalibStreamCnv");

  log << MSG::DEBUG << "filling CSC collections"  << endreq;

  /** find the det descr tag version from the event type */
  const DataHandle<TagInfo> tagInfo;
  StatusCode status = m_detStore->retrieve(tagInfo);
  if (status.isFailure()) {
    log << MSG::ERROR <<"Could not retrieve tag info  from TDS. - abort ..."  << endreq; 
    return status;
  }

  tagInfo->findTag("GeoAtlas", m_detdescr);
  log << MSG::DEBUG <<  "DetDescr tag = " << m_detdescr << endreq;

  //Retrieve event itself  
  const CalibEvent* event= m_dataProvider->getEvent(); 

  //int l1Id=event->lvl1_id();
  //int bcId=0; 

  //This part should start printing words
  if(event->csc()!=NULL){

    //Loop over CSCCalibData. Each CscCalibDataObject corresponds to a single ROD
    std::list<CscCalibData> listOfCSCCalibData = (event->csc())->data();
    log << MSG::DEBUG << "Csc CalibStream data in this event. Number of CSCCalibData objects is " << listOfCSCCalibData.size() << endreq;
    std::list<CscCalibData>::iterator CSCCalibDataItr = listOfCSCCalibData.begin();
    std::list<CscCalibData>::iterator CSCCalibDataEnd = listOfCSCCalibData.end();
    //std::list<CscRawData> m_listOfRDO;
    for(; CSCCalibDataItr != CSCCalibDataEnd; CSCCalibDataItr++){

      const  std::list<uint32_t>& words = CSCCalibDataItr->rod_words();

      log << MSG::DEBUG << "Number of CalibDatas in fragment " << listOfCSCCalibData.size() << endreq;

      /***ROD content information taken from: http://positron.ps.uci.edu/~schernau/ROD/2rt/CSCDataFormat.html*/  
      std::list<uint32_t>::const_iterator word = words.begin();
      std::list<uint32_t>::const_iterator wordEnd = words.end();
      unsigned int numWord = words.size();
      log << MSG::DEBUG << "Number of words is " << numWord << endreq;
      
      /** ATLAS header marker */ 
      const uint32_t & headerMarker = *word;          //0xee1234ee;   // start of header marker
      const uint32_t & headerSize = *(++word);        // 9;            // size of ATLAS header (without CSC header)
      const uint32_t & formatVersion = *(++word);     //0x03010200;   // format major version 3.1, minor version 2.0

      const uint16_t minorVersion= (formatVersion & 0xffff);
      if(minorVersion == 0x0){
        log << MSG::DEBUG << "Minor version 0x0. ROB is empty. Doing nothing" << endreq;
        continue;
      } else if(formatVersion != 0x03010200) {
        log << MSG::WARNING << "Unknown rod version " << std::hex << formatVersion << std::dec 
          << ". Treating as 0x03010200, but there may be problems. " << endreq;
      }

      const uint32_t & sourceID = *(++word);          //0x006900ss;   // source ID, 69 for A side, or 6a for C side, ss=slot#
      const uint32_t & runNumber = *(++word);         //runNumber;     
      const uint32_t & eventNumber = *(++word);       //eventNumber;  // level 1 ID
      const uint32_t & bunchCrossing = *(++word);     //bcid;         // bunch crossing
      const uint32_t & triggerType = *(++word);       //0;            // trigger type, to be read from the TIM
      const uint32_t & eventType =*(++word);          //0xrrrrrr04;   // detector event type, 04= # of samples, rrrrrr = see below.
      /*The detector event type summarizes ROD parameters of the current event, the most important one being the number of samples. 
        It is defined in the following way: 

        eventType = (runParameter.numSamples & 0xff)         |
        ((runParameter.latency & 0xff) << 8)     |
        ((runParameter.calLayer & 0x3f) <<16)    | 
        ((runParameter.sparsified & 1) << 22)    |
        ((runParameter.neutron & 1) << 23)       |
        ((runParameter.calAmplitude & 0x3f)<<24) |
        ((runParameter.enableCal & 1) << 30)     |
        ((runParameter.sampling==20?0:1)<<31);
       */    
      log << MSG::DEBUG << "ATLAS HEADER"  
        << "\nheader marker " << MSG::hex << headerMarker 
        << "\nheader size " << MSG::dec <<  headerSize
        << "\nformat version " << MSG::hex << formatVersion 
        << "\nsource ID " << MSG::hex << sourceID
        << "\nrun number " << MSG::dec << runNumber
        << "\nevent number " << MSG::dec << eventNumber
        << "\nbunch crossing " << MSG::hex << bunchCrossing
        << "\ntrigger types " << MSG::hex << triggerType
        << "\neventType " << MSG::hex << eventType 
        << MSG::dec <<endreq;

      uint16_t subDetectorId = (sourceID>>16)&0xff; //For our purposes = A or C side.
      uint16_t onlineRodId = sourceID&0xff;  //??
      log << MSG::DEBUG << "The Online ROD id is " << MSG::hex << onlineRodId << endreq;
      uint16_t rodId = 0xffff;
      bool check = m_cabling->offlineId(onlineRodId, rodId);
      if(!check) {
        log << MSG::ERROR << "Can't determine offline ROD Id from online ROD ID - "
          << " check the cabling service." << endreq;
      }
      uint16_t idColl = m_cabling->collectionId(subDetectorId, rodId);

      log << MSG::DEBUG << "The Collection ID is " << idColl << endreq;
      log << MSG::DEBUG << "The ROD Id is " << rodId << endreq;
      log << MSG::DEBUG << "The sub-detector Id is " << subDetectorId << endreq; 

      if(subDetectorId != 0x6A && subDetectorId != 0x69) {
        log << MSG::ERROR << "sub-detector id is " << MSG::hex << subDetectorId << MSG::dec << ". Should be 0x6A or 0x69." << endreq;
        return StatusCode::RECOVERABLE;
      }
      if (idColl > 15 || rodId > 7) {
        log << MSG::ERROR << " Collection Id must be <= 15 and rodId must be <=7. They are " << idColl << " and " << rodId << " respectively. Won't decode this ROD."<< endreq;
        return StatusCode::RECOVERABLE;
      }

      //Create new collection if one doesn't already exist for this ROD. Otherwise quit.
      CscRawDataCollection * rawCollection = 0;
      CscRawDataContainer::const_iterator collItr = m_rdoContainer->indexFind(idColl);
      if(m_rdoContainer->end() !=  collItr) {
        log << MSG::WARNING << "CSC RDO collection already exists with collection hash = " << idColl << endreq;
        return StatusCode::SUCCESS;
      } else {
        log << MSG::DEBUG << "CSC RDO collection does not exist - creating a new one with hash = " << idColl << endreq;
        rawCollection = new CscRawDataCollection(idColl);
        if( (m_rdoContainer->addCollection(rawCollection, idColl)).isFailure()) {
          log << MSG::ERROR << "Failed to add RDO collection to container" << endreq;
          return StatusCode::RECOVERABLE;
        }
      }

      //Configure collection
      rawCollection->setSubDetectorId(subDetectorId);
      rawCollection->setRodId(onlineRodId);
      rawCollection->setOnlineId(idColl);
      rawCollection->set_eventType(eventType);

      /** the detector event type */
      rawCollection->set_eventType( eventType );

      /** is sparsified data */
      bool isSparsified = rawCollection->sparsified();

      /** is neutron rejection ON */
      bool ne = rawCollection->neutron();

      /** calibration enabled */
      bool cal = rawCollection->calEnabled();

      /** lentency */
      uint16_t latency = rawCollection->latency();

      /** calibration layer */
      uint16_t calLayer = rawCollection->calLayer();

      /** calibration amplitude */
      //uint16_t calAmplitude = rawCollection->calAmplitude();

      /** the samplint time */
      uint16_t samplingTime = rawCollection->rate();

      /** num Samples */
      uint16_t numSamples = eventType & 0xFF;

      log << MSG::DEBUG << "Event Type " << MSG::hex << eventType << MSG::dec << endreq;  
      log << MSG::DEBUG << "Sampling Time: " << samplingTime << endreq;
      log << MSG::DEBUG << "Number of Samples: " << numSamples << endreq;
      log << MSG::DEBUG << "Is Calibration Enabled?: " << cal << endreq;
      log << MSG::DEBUG << "Calibration Layer: " << calLayer << endreq;
      log << MSG::DEBUG << "Latency: " << latency << endreq;
      log << MSG::DEBUG << "Is neutron rejection ON?: " << ne << endreq;
      log << MSG::DEBUG << "Is sparsified data? " << isSparsified << endreq;

      //Check for missing words
/*    int wordCnt = 1;
      for(;word != wordEnd; word++){
        std::cout <<  wordCnt++ << ": Extra word is " << std::hex<< *word << std::dec << std::endl;
      }
      if(word == wordEnd) {
        log << MSG::DEBUG << "Skipping second RPU, at end of word list" << endreq;
        continue;
      }
*/

      //Extract the two RPU's and store into rawCollection
      log << MSG::DEBUG << "Number of words is " << numWord << endreq;
      m_isClusterWordsUnrealistic = false;
      word++;//last time we incremented, then read. Thus we need to increment once more
      log << MSG::DEBUG << "Processing first RPU" << endreq;
      StatusCode sc1 = extractRpu(word,rawCollection,numSamples,idColl);
      if(!sc1.isSuccess()) {
        //Something went wrong, whatever it was, we'll discard the rawColleciton.
          rawCollection->erase(rawCollection->begin(),rawCollection->end());
        if(sc1 == StatusCode::RECOVERABLE){
          log << MSG::WARNING << "Something unusual occured while reading first RPU. Discarding rawDataCollection." << endreq;
          continue;
        } else {
          log << MSG::ERROR 
            << "Failed reading first RPU! Skipping this event." 
            << endreq;
          return sc1;
        }
      }

      word++;
      
      if(word == wordEnd) {
        log << MSG::DEBUG << "Skipping second RPU, at end of word list" << endreq;
        continue;
      }
      log << MSG::DEBUG << "Processing second RPU" << endreq;
      StatusCode sc2 = extractRpu(word,rawCollection,numSamples,idColl);
      if(!sc2.isSuccess()) {
        //Something went wrong, whatever it was, we'll discard the rawColleciton.
          rawCollection->erase(rawCollection->begin(),rawCollection->end());
        if(sc2 == StatusCode::RECOVERABLE) {
          log << MSG::WARNING 
            << "Something unusual occured while reading second RPU. Discarding this rawDataCollection." 
            << endreq;
          continue;
        } else {
          log << MSG::ERROR 
            << "Failed reading second RPU! Skipping this event." 
            << endreq;
          return sc2;
        }
      }
      
      word++;
      if(word == wordEnd) log << MSG::DEBUG << "At end of word list" << endreq;

      if(m_isClusterWordsUnrealistic) {
        log << MSG::WARNING << std::hex << "ROB Fragment with ID " << std::dec 
          << sourceID << " has too many cluster words. Discarded!!" << endreq;
          rawCollection->erase(rawCollection->begin(),rawCollection->end());
      }

    }//end CalibData Loop
    pObj= m_rdoContainer;

    log << MSG::DEBUG << "End fill Collection" << endreq;
  } //end if(csc->event() != NULL)
  else {
    log << MSG::DEBUG << "Csc CalibStream data not in this event." << endreq;
  }
  
  return StatusCode::SUCCESS; 
}


//Extract RPU function is heavily based on code at:
//MuonSpectrometer/MuonCnv/MuonCSC_CnvTools/src/CscROD_Decoder.cxx
StatusCode CscRDOContCalibStreamCnv::extractRpu(std::list<uint32_t>::const_iterator & word, 
    CscRawDataCollection * rawCollection,const uint16_t & numSamples, const uint16_t & idColl
    ) {
  MsgStream log(messageService(), "CscRDOContCalibStreamCnv");

  uint32_t counter = 0;

  //RPU Block header
  //  0000 MMMM 0000 TTTT SSSS SSSS SSSS SSSS
  //
  //M: Module ID (RPU number on the ROD)
  //T: type, 0=normal, 1=discard, 2=Error
  //S: Size of this block in words, including this header and any 'ghost' 
  //words at the end.
  uint32_t header = *word;
  log << MSG::DEBUG << "RPU Header word " << MSG::hex << header << MSG::dec << endreq;
  if(!header) {
    log << MSG::WARNING << "RPU Header word is NULL! Skipping this RPU!" << endreq;
    m_numMissingRPUs++;
    return StatusCode::RECOVERABLE;
  }
  counter++;

  //RPU ID
  // patch from KJ
  //uint16_t rpuID = (header >> 24) & 0xFF;
  uint16_t rpuID_raw = (header >> 24) & 0xFF;
  // ROD version 3 (SLAC made) return 13 instead of 11 as of March 2010
  uint16_t rpuID;
  if (rpuID_raw == 13) {
      rpuID = uint16_t(rpuID_raw-2); // make 13 to 11...
  } else {
      rpuID = rpuID_raw;
  }
  log << MSG::DEBUG << "RPU ID " << rpuID << endreq;
  rawCollection->addRPU(rpuID);

  //Data Type
  uint16_t dataType = (header >> 16) & 0xFF;
  rawCollection->addDataType(dataType);
  log << MSG::DEBUG << "Data type " << dataType << endreq;;

  //Size in words of this RPU
  uint32_t rpuSize = header & 0xFFFF;
  log << MSG:: DEBUG << "RPU Size " << rpuSize << endreq; 


  /**Check to see if the data is good before deciding how to proceed */
  // The meaning for the Type TTTT is:
  //0: no error, but can be empty.
  //1: discarded: there is just a header, no data and no ghostwords.
  //2: Error: no data, only ghost words
  //3: Partial error: some data and ghost words. This is a new feature that is
  //planned, but not yet implemented. */
  if ( dataType == 1 ) {        // in this case we have a discard -> continue to the next RPU
    return StatusCode::SUCCESS;
  } else if ( dataType == 2) {  // this is an error condition -> for now skip the error words and go to next RPU
    int numSkip = rpuSize - counter;
    for(int i = 0; i < numSkip; i++){
      word++;
      counter++;
    }
    log << MSG::DEBUG << "Error condition encountered on RPU ID = " << rpuID  << ". Continuing..." << endreq; 
    return StatusCode::SUCCESS;
  } else { //we have good data -> please decode! but in this case there could be partial error 
    //in which case there are some errors and some could data */

    if ( rpuSize == 1 ) {  /** probably there is no error but still can be no data */
      log << MSG::DEBUG << "  TTTT Type = " << dataType << " with no data and no errors. " 
        << " Continue to next RPU ..." << endreq;
      //word++;
      return StatusCode::SUCCESS;  
    }

    //**********Data Block ************************************************/
    //The data begins with one word of SCA addresses and 2 words of cluster counts 
    //and misc. data. This is followed by the clusters.
    //
    //AAAA AAAA AAAA AAAA AAAA AAAA AAAA AAAA
    //CCCC CCCC CCCC CCCC CCCC CCCC CCCC CCCC
    //CCCC CCCC rrTP FFFF DDDD DDDD DDDD DDDD
    //
    //A: SCA address, 1 byte per time sample, limited to 4 samples
    //if more than 4 were taken.
    //C: Cluster counts, 1st precision SPU in most significan byte.
    //T: Trigger type (priority)
    //P: Sampling phase: 0: 2nd sample is near peak, 1: 3rd sample near peak
    //F: first bit summary
    //D: Number of data words in all clusters that follow
    //********************************************************************/

    //SCA Address
    uint32_t scaAddress = *(++word);
    counter++; 
    log << MSG::DEBUG << "SCA Address : " << std::hex << scaAddress << std::dec << endreq;
    rawCollection->set_scaAddress( scaAddress);

    //Read the cluster counts for precision layers
    uint32_t precisionClusters = *(++word);
    counter++;
    log << MSG::DEBUG << "Number of Precision Cluster words " << MSG::hex << precisionClusters << MSG::dec << endreq;
    uint32_t precisionClusterCounts = 0;
    for(uint16_t k = 0; k<4; k++) {
      uint16_t counts = (precisionClusters >> (24-k*8)) & 0xFF;
      precisionClusterCounts += counts;
      log << MSG::DEBUG << "Number of precision Cluster Counts - " << "Layer Index = " 
        <<k << " Cluster Counts = " << counts << endreq;
      unsigned int index = 0;
      if( rpuID == 5) index = k;
      else if (rpuID ==11 ) index = k+5;
      else log << MSG::ERROR << "RPU ID out of range " << rpuID << endreq;
      rawCollection->set_spuCount(index, counts);
    }

    //read the second cluster word */
    uint32_t secondClusterWord = *(++word);
    counter++;
    log << MSG::DEBUG << "  Second cluster word " << MSG::hex << secondClusterWord << MSG::dec << endreq;

    //non precision cluster counts 
    // all the counts are in position 5/10 for non-precision strips
    uint16_t nonPrecisionClusterCounts = (secondClusterWord >> 24) & 0xFF;
    log << MSG::DEBUG 
      << "  Summed Number of Clusters for non-precision layers: " 
      << nonPrecisionClusterCounts << endreq; 
    if (rpuID == 5)  rawCollection->set_spuCount(4, nonPrecisionClusterCounts);
    if (rpuID == 11) rawCollection->set_spuCount(9, nonPrecisionClusterCounts); 

    /** decode the Trigger Type */
    bool triggerType = (secondClusterWord >> 21 ) & 0x1;
    if ( triggerType ) rawCollection->set_triggerType();

    /** decode the sampling phase */
    bool samplingPhase = (secondClusterWord >> 20) & 0x1;
    if ( samplingPhase ) rawCollection->set_samplingPhase();

    /** decode the first bit summary */
    uint8_t firstBitSummary = (secondClusterWord >> 16 ) & 0xF;
    rawCollection->set_firstBitSummary( firstBitSummary );


    //decode the number of data words of all clusters in this RPU - should not be used
    uint16_t clusterDataWords = secondClusterWord & 0xFFFF;
    log << MSG::DEBUG << "  Cluster Data Words = " 
      << clusterDataWords << endreq;

    //the total cluster counts
    uint32_t totalClusterCounts = nonPrecisionClusterCounts+precisionClusterCounts;
    log << MSG::DEBUG 
      << "  Total summed Cluster Count for precision and non-precision layers = " 
      << totalClusterCounts << endreq;

    if ( totalClusterCounts == 0 ) {  /** probably no errors but still no clusters found */
      log << MSG::DEBUG  << "  No Cluster found - skipping to next RPU " << endreq;

      int numSkip = rpuSize-counter;
      for(int i = 0; i < numSkip; i++){
        word++;
        counter++;
      }
      return(StatusCode::SUCCESS);
    }

    /** start the cluster count */
    uint16_t clusterCount = 1;
    while ( clusterCount <= totalClusterCounts ) {
      log << MSG::DEBUG << "Cluster " << clusterCount << " out of " << totalClusterCounts << endreq;
      log << MSG::DEBUG << "(word counter at " << counter << ")" << endreq;

      /** get the address of the first channel */
      uint32_t address = *(++word);
      counter++;
      log << MSG::DEBUG << "  cluster location word " <<  MSG::hex << address << MSG::dec << endreq;
      Identifier channelId = m_cscRodDecoderTool->getChannelId( address, m_detdescr);
      int stationId        = m_cscIdHelper->stationName(channelId);
      int currentLayer     = m_cscIdHelper->wireLayer(channelId);
      int orientation      = m_cscIdHelper->measuresPhi(channelId);


      /** decode the cluster time and the cluster width */
      uint32_t time_width = *(++word);
      counter++;
      log << MSG::DEBUG << "  cluster time and size word " << MSG::hex << time_width << MSG::dec << endreq;
      uint16_t time       = (time_width >> 16) & 0xFFF;
      uint16_t width      = time_width & 0xFFFF;
      
      //check for potential error
      if(width > 192)
        m_isClusterWordsUnrealistic = true;

      bool isTimeComputed = (time_width >> 28 ) & 0x1;  

      uint16_t totalSampleWords = width*numSamples/2 + (width*numSamples)%2;
      

      /** decode the ADC samples */
      std::list<uint32_t>::const_iterator sampleStart = (++word);
      counter++;
      for (int j=0; j<totalSampleWords; ++j) {
        word++;
        counter++;
      }

      std::vector<uint32_t> wordVect(sampleStart,word);
      std::vector<uint16_t> amplitude;
      m_cscRodDecoderTool->getSamples( wordVect, amplitude );

      //We've overshot where we want word to be at at the end of the loop
      //Might want to redesign loop at some point to make less cumbersome.
      word--;
      counter--;

      clusterCount += 1;

      /** create the raw hit insert it into the collection */
      uint16_t spuID = 0x0;       
      if ( orientation == 0 ) spuID = static_cast<uint16_t>( (stationId-50)*5 + currentLayer-1);
      else spuID = static_cast<uint16_t>( ( (stationId-50)+1)*5 - 1);

      uint32_t hashId      = m_cscRodDecoderTool->getHashId(address, m_detdescr);
      CscRawData * rawData = new CscRawData(amplitude, address, idColl, time, spuID, width, hashId);
      if ( isTimeComputed ) rawData->set_isTimeComputed();
      rawCollection->push_back(rawData);

      log <<MSG::DEBUG << "  cluster width, the time, nSample words & 1st Channel ID: " 
        << width << " " << time 
        << " " << totalSampleWords << " " << m_cscIdHelper->show_to_string(channelId) 
        << endreq;

    }//end cluster loop

    /////////////////////////////////////////////////////////////////
    log <<MSG::DEBUG << "  Total Cluster count = " << (clusterCount -1)
      << " for RPU ID " << rpuID << endreq;

    /** First check that we've processed this RPU correctly */
    if ( counter > rpuSize ) {
      log << MSG::WARNING << "ROB Fragment has counter " << counter << " which should not exceed summed RPU sizes " << rpuSize << " Discarded!!" << endreq;
      rawCollection->erase(rawCollection->begin(), rawCollection->end());
    } 

    /** check for partial errors  and extra ghost words at the end */ 
    if ( counter < rpuSize ) {
      log << MSG::DEBUG <<  "  " << (rpuSize-counter) << "  ghost words at the end of this RPU - skipping ..." << endreq;
      int numSkip = rpuSize-counter;
      std::cout << "pre-skip " << std::hex << *word <<std::dec<< std::endl;
      for(int i = 0; i < numSkip; i++){
        word++;
        std::cout <<i << ": Skipping " << std::hex << *word <<std::dec<< std::endl;
        counter++;
      }
    }
     
  }//end if(there is error)
  log << MSG::DEBUG << "Counted " << counter << " words this RPU." << endreq;

  return StatusCode::SUCCESS;
}//end extractRPU
