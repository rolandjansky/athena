/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CscROD_Decoder.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/DataHandle.h"
#include "EventInfo/TagInfo.h"

#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "eformat/Issue.h"

#include <algorithm> 

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ; 
using OFFLINE_FRAGMENTS_NAMESPACE::PointerType ; 
using eformat::helper::SourceIdentifier; 

/** constructor */ 
Muon::CscROD_Decoder::CscROD_Decoder
( const std::string& type, const std::string& name,const IInterface* parent )
:  base_class(type,name,parent),
   m_cabling( "CSCcablingSvc" ,name) 
{
  declareProperty("IsCosmics", m_isCosmic = false);
  declareProperty("IsOldCosmics", m_isOldCosmic = false);
}

/** destructor 
*/ 
Muon::CscROD_Decoder::~CscROD_Decoder() 
{}

StatusCode Muon::CscROD_Decoder::initialize() 
{

  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode status = svcLocator->service("MessageSvc", msgSvc);
  if ( status.isFailure() ) {
    ATH_MSG_ERROR ( "Failed to find MessageSvc " );
    return status;
  }

  MsgStream log(msgSvc, "CscROD_Decoder::initialize"); 

  // get the cabling service
  if ( m_cabling.retrieve().isFailure() )
  {
    ATH_MSG_ERROR ( " Cannot get CablingSvc " );
    return StatusCode::FAILURE;
  }

  // Get the CSC id helper from the detector store
  status = detStore()->retrieve(m_cscHelper, "CSCIDHELPER");
  if (status.isFailure()) {
    ATH_MSG_FATAL ( "Could not get CscIdHelper !" );
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG ( " Found the CscIdHelper. " );
  }
 
  m_hid2re.set( &(*m_cabling), m_cscHelper );

  if ( m_isCosmic ) {
     m_hid2re.set_isCosmic();
     if ( m_isOldCosmic ) m_hid2re.set_isOldCosmic();
  }
 
  return StatusCode::SUCCESS;
}



void Muon::CscROD_Decoder::fillCollection(const xAOD::EventInfo& eventInfo,
                                          const ROBFragment& robFrag,
                                          CscRawDataContainer& rdoIDC) const
{
  ATH_MSG_DEBUG ( "in CscROD_Decode::fillCollection()" );

  // Check the ROB and ROD fragment . E.P.
  try {
    robFrag.check ();
  }
  catch (eformat::Issue &ex) {
    ATH_MSG_DEBUG ( ex.what () );
    return;  // error in fragment - we search for no collection
  }

  uint32_t nstat = robFrag.nstatus ();
  if (nstat) {
    const uint32_t *it;
    robFrag.status (it);
    if (*it) {
      ATH_MSG_DEBUG ( "Error in ROB status word: 0x" << std::hex << *it << std::dec );
      return;
    }
  }
  
  eformat::helper::Version ver(robFrag.rod_version());
  const uint16_t& rodMinorVersion= ver.minor_version();
  
  ATH_MSG_DEBUG ( "in CscROD_Decoder::fillCollection :ROD version "
      << MSG::hex << rodMinorVersion << MSG::dec );

  if ( rodMinorVersion == 0x0200 )
     this->rodVersion2(robFrag, rdoIDC);
  else if ( (rodMinorVersion >> 8) == 4){ // minor ver. 0400 Oct. 2014
     this->rodVersion2(robFrag, rdoIDC);
  }
  else if (rodMinorVersion == 0x1 || // ROD version for CTB and DC3 - for backward compatibility
           eventInfo.eventType(xAOD::EventInfo::IS_TESTBEAM)) 
     this->rodVersion1(robFrag, rdoIDC);
  else if (rodMinorVersion == 0x0) 
     ATH_MSG_DEBUG ( "Empty ROB - doing nothing " );
  else //ROD version for DC2 - old and obselete; for backward compatibility
     this->rodVersion0(robFrag, rdoIDC);  

  return;
}


////////////////////////////////
//// To access CscRODReadOut.... 
////
uint32_t Muon::CscROD_Decoder::getHashId(const uint32_t word, std::string /*detdescr*/) const {
  CscRODReadOut rodReadOut;
  /** set the CSC Id Helper */
  rodReadOut.set(m_cscHelper);
  rodReadOut.setChamberBitVaue(1);

  rodReadOut.decodeAddress( word );
  Identifier moduleId  = rodReadOut.decodeAddress();
  //Identifier channelId = rodReadOut.decodeAddress(moduleId);
  return rodReadOut.hashIdentifier(moduleId);
}

////
Identifier Muon::CscROD_Decoder::getChannelId(const uint32_t word, std::string /*detdescr*/) const {
  CscRODReadOut rodReadOut;
  /** set the CSC Id Helper */
  rodReadOut.set(m_cscHelper);
  rodReadOut.setChamberBitVaue(1);

  rodReadOut.decodeAddress( word );
  Identifier moduleId  = rodReadOut.decodeAddress();
  return rodReadOut.decodeAddress(moduleId);
}

////
void Muon::CscROD_Decoder::getSamples(const std::vector<uint32_t>& words, std::vector<uint16_t>& samples) const {
  CscRODReadOut rodReadOut;
  /** set the CSC Id Helper */
  rodReadOut.set(m_cscHelper);
  rodReadOut.setChamberBitVaue(1);

  for (unsigned int j=0; j<words.size(); ++j) {
    rodReadOut.decodeAmplitude(words[j]);
    samples.push_back(rodReadOut.getAmp1());
    samples.push_back(rodReadOut.getAmp2());
  }

}


void Muon::CscROD_Decoder::rodVersion2(const ROBFragment& robFrag,  CscRawDataContainer& rdoIDC) const {


  ATH_MSG_DEBUG ( " " ) ;
  ATH_MSG_DEBUG ( "===================================================" );
  ATH_MSG_DEBUG ( "in CscROD_Decode::fillCollection() - ROD version 2" );

  /** find the det descr tag version from the event type */
  const DataHandle<TagInfo> tagInfo;
  StatusCode status = detStore()->retrieve(tagInfo);
  if (status.isFailure()) {
    ATH_MSG_ERROR ( "Could not retrieve tag info  from TDS. - abort ..." ); 
   return;
  }

  std::string detdescr = "";
  tagInfo->findTag("GeoAtlas", detdescr);
  ATH_MSG_DEBUG ( "DetDescr tag = " << detdescr );
  
  const uint32_t& detev_type = robFrag.rod_detev_type();

  CscRODReadOut rodReadOut;

  /** set the CSC Id Helper */
  rodReadOut.set(m_cscHelper);
  rodReadOut.setChamberBitVaue(1);

  /** get some information */
  SourceIdentifier sid (robFrag.rod_source_id()); 
  uint16_t subDetectorId = sid.subdetector_id();
  uint32_t onlineRodId   = sid.module_id();
  ATH_MSG_DEBUG ( "Online ROD id is 0x" << MSG::hex << onlineRodId << MSG::dec );

  uint16_t rodId         = 0xFFFF;
  bool check             = m_cabling->offlineId(onlineRodId, rodId);
  if ( !check ) ATH_MSG_ERROR ( "enable to convert online ROD Id to offline ROD Id - "
		     << "check the cabling service " );
  uint16_t idColl        = m_cabling->collectionId(subDetectorId, rodId);

  ATH_MSG_DEBUG ( "Online ROD / ROD / collection / subDetector IDs are 0x" << MSG::hex << onlineRodId << MSG::dec << " "
                  << rodId << " " << idColl << " " << subDetectorId );

  //assert (subDetectorId == 0x6A || subDetectorId == 0x69); // 50 or 51

  // Create the Csc container and use the cache to get it
  std::unique_ptr<CscRawDataCollection> rawCollection(nullptr);
  CscRawDataContainer::IDC_WriteHandle lock = rdoIDC.getWriteHandle( idColl );
  if( lock.alreadyPresent() ) {
    ATH_MSG_DEBUG ( "CSC RDO collection already exist with collection hash = " << idColl << " converting is skipped!");
  }
  else{
    ATH_MSG_DEBUG ( "CSC RDO collection does not exist - creating a new one with hash = " << idColl );
    rawCollection = std::make_unique<CscRawDataCollection>(idColl);
  }

  /** set the ROD id and the subDector id */
  rawCollection->setSubDetectorId(subDetectorId);
  rawCollection->setRodId(onlineRodId);
  rawCollection->setOnlineId(idColl);

  /** the detector event type */
  rawCollection->set_eventType( detev_type );

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
  uint16_t calAmplitude = rawCollection->calAmplitude(); 

  /** the samplint time */
  uint16_t samplingTime = rawCollection->rate();

  /** number of samples */
  uint16_t numSamples = detev_type & 0xFF;

  ATH_MSG_DEBUG ( "Event Type: " << MSG::hex << detev_type << MSG::dec );
  ATH_MSG_DEBUG ( "Sampling Time: " << samplingTime << "  Number of Samples: " << numSamples );
  ATH_MSG_DEBUG ( "Is Calibration Enabled?: " << cal << "  Calibration Amplitude: " << calAmplitude );
  ATH_MSG_DEBUG ( "Calibration Layer: " << calLayer << "  Latency: " << latency );
  ATH_MSG_DEBUG ( "Is neutron rejection ON?: " << ne << "  Is sparsified data?: "  << isSparsified );

  /** number of RPU in the ROD */ // Two RPU per one ROD
  uint16_t numberRPU = 0;

  /** total word counter */
  uint32_t counter = 0;
  uint32_t rpuSize = 0;

  const unsigned int size = robFrag.rod_ndata();
  ATH_MSG_DEBUG ( "CscROD_Decoder Total words received = " << size );

  //  uint32_t wordPos   = 0;
  PointerType vint; //OFFLINE_FRAGMENTS_NAMESPACE::
  robFrag.rod_data( vint );
  
  bool isHeaderWordNull = false; // header is 0 so couldn't retrieve rpuID and size.
  bool isClusterWordsUnrealistic = false; // too many cluster words causes unrealistic strip position
  while ( counter < size ) {

    /** RPU header */
    uint32_t header = vint[counter];
    ATH_MSG_DEBUG ( "RPU Header word 0x" << MSG::hex << header << MSG::dec );
    if (!header) { // rpu size and rpu id couldn't be figured out...
      isHeaderWordNull = true;
      break;
    }
    counter += 1;

    /** RPU ID */
    uint16_t rpuID_raw = (header >> 24) & 0xFF;
    ATH_MSG_DEBUG ( "RPU ID original = " << rpuID_raw );
    // ROD version 3 (SLAC made) return 13 instead of 11 as of March 2010
    uint16_t rpuID;
    if (rpuID_raw == 13) {
      rpuID = uint16_t(rpuID_raw-2); // make 13 to 11... 
    } else {
      rpuID = rpuID_raw;
    }
    
    ATH_MSG_DEBUG ( "RPU ID Updated = " << rpuID );

    rawCollection->addRPU ( rpuID );

    /** Data Type */ 
    uint16_t dataType = (header >> 16) & 0xFF;
    rawCollection->addDataType ( dataType );

    /** Size in word of this RPU */
    uint32_t rSize = header & 0xFFFF;
    rpuSize += rSize;
    ATH_MSG_DEBUG ( "RPU size = " << rSize );

    /** increase RPU count event if has bad or no data */
    numberRPU++;

    /** if the data is good before deciding how to proceed */
    /** The meaning for the Type TTTT is:
        0: no error, but can be empty.
        1: discarded: there is just a header, no data and no ghostwords.
        2: Error: no data, only ghost words
        3: Partial error: some data and ghost words. This is a new feature that is
           planned, but not yet implemented. */
    if ( dataType == 1 ) continue; /** in this case we have a discard -> continue to the next RPU */
    else if ( dataType == 2) {     /** this is an error condition -> for now skip the error words and continue */
      counter += rSize-1;          /** 8 status words for each SPU and for each time slice - but may change with time */
      ATH_MSG_DEBUG ( "Error condition encounted on RPU ID = " << rpuID 
	   << " continuing ..." );
      continue;  
    } else {                      /** we have good data -> please decode! but in this case there could be partial error 
                                      in which case there are some errors and some could data */

      if ( rSize == 1 ) {  /** probably there is no error but still can be no data */
        ATH_MSG_DEBUG ( "TTTT Type = " << dataType << " with no data and no errors. " 
             << " Continue to next RPU ..." );
        continue;  
      }
 
      /** This should be the SCA address */
      uint32_t scaAddress = vint[counter];
      ATH_MSG_DEBUG ( "SCA Address = " << scaAddress );
      rawCollection->set_scaAddress( scaAddress );
      counter +=1;

      /** read the cluster counts for precision layers */
      uint32_t precisionClusters = vint[counter];
      ATH_MSG_DEBUG ( "Number of Precision Cluster word 0x" << MSG::hex << precisionClusters << MSG::dec );
      uint32_t precisionClusterCounts = 0;
      for (uint16_t k=0; k<4; k++) {
        uint16_t counts = (precisionClusters >> (24-k*8)) & 0xFF;
	precisionClusterCounts += counts;
        ATH_MSG_DEBUG ( "Number of precision Cluster Counts - " << "Layer Index = " 
             << k << " Cluster Counts = " << counts );
        unsigned int index = 0;
        if ( rpuID == 5) index = k;
        else if ( rpuID == 11 ) index = k+5;
        else ATH_MSG_ERROR ( "RPU ID out of range " << rpuID );
        rawCollection->set_spuCount(index, counts);
      }
      counter += 1;
           
      /** read the second cluster word */
      uint32_t secondClusterWord = vint[counter];
      ATH_MSG_DEBUG ( "Second cluster word 0x" << MSG::hex << secondClusterWord << MSG::dec );
      counter +=1;

      /** non precision cluster counts */
      uint16_t nonPrecisionClusterCounts = (secondClusterWord >> 24) & 0xFF;
      ATH_MSG_DEBUG ( "Summed Number of Clusters for non-precision layers "
           << nonPrecisionClusterCounts ); 
      if (rpuID == 5)  rawCollection->set_spuCount(4, nonPrecisionClusterCounts);
      if (rpuID == 11) rawCollection->set_spuCount(9, nonPrecisionClusterCounts); // all the counts are in position 5/10 for non-precision strips

      /** decode the Trigger Type */
      bool triggerType = (secondClusterWord >> 21 ) & 0x1;
      if ( triggerType ) rawCollection->set_triggerType();

      /** decode the sampling phase */
      bool samplingPhase = (secondClusterWord >> 20) & 0x1;
      if ( samplingPhase ) rawCollection->set_samplingPhase();

      /** decode the first bit summary */
      uint8_t firstBitSummary = (secondClusterWord >> 16 ) & 0xF;
      rawCollection->set_firstBitSummary( firstBitSummary );

      /** decode the number of data words of all clusters in this RPU - should not be used */
      uint16_t clusterDataWords = secondClusterWord & 0xFFFF;
      ATH_MSG_DEBUG ( "Cluster Data Words = " << clusterDataWords );

      /** the total cluster counts */
      uint32_t totalClusterCounts = nonPrecisionClusterCounts+precisionClusterCounts;
      ATH_MSG_DEBUG ( "Total summed Cluster Count for precision and non-precision layers = " << totalClusterCounts );

      if ( totalClusterCounts == 0 ) {  /** probably no errors but still no clusters found */
         ATH_MSG_DEBUG ( "No Cluster found - skipping to next RPU " );
         counter += rSize-4; /** -4 because 4 words in the RPU have been read so far */
         continue;
      }

      /** start the cluster count */
      uint16_t clusterCount = 0;

      while ( clusterCount < totalClusterCounts ) {
	/** get the address of the first channel */
	uint32_t address = vint[counter];

        ATH_MSG_DEBUG ( "cluster location word 0x" << MSG::hex << address << MSG::dec );

	rodReadOut.decodeAddress( address );
        Identifier moduleId  = rodReadOut.decodeAddress();
        Identifier channelId = rodReadOut.decodeAddress(moduleId);
        int stationId        = m_cscHelper->stationName(channelId);
        int currentLayer     = m_cscHelper->wireLayer(channelId);
        int orientation      = m_cscHelper->measuresPhi(channelId);
        int stripId          = m_cscHelper->strip(channelId);

	counter += 1;
  
	/** decode the clusdter time and the cluster width */
        uint32_t time_width = vint[counter];
        ATH_MSG_DEBUG ( " cluster time size word : stripId (CscIdHelper) 0x"
                        << MSG::hex << time_width << MSG::dec << " " << stripId );
	counter += 1;
        uint16_t time       = (time_width >> 16) & 0xFFF;
        uint16_t width      = time_width & 0xFFFF;
        if (width>192) {
          isClusterWordsUnrealistic = true;
          break;
        }
        bool isTimeComputed = (time_width >> 28 ) & 0x1;  

        uint16_t totalSampleWords = width*numSamples/2 + (width*numSamples)%2;

	/** decode the ADC samples */
	std::vector<uint16_t> amplitude;
	for (int j=0; j<totalSampleWords; ++j) {
	    rodReadOut.decodeAmplitude(vint[counter]);
	    amplitude.push_back(rodReadOut.getAmp1());
	    amplitude.push_back(rodReadOut.getAmp2());
	    counter += 1;
	}
	clusterCount += 1;
	
	/** create the raw hit insert it into the collection */
        uint16_t spuID = 0x0;       
        if ( orientation == 0 ) spuID = static_cast<uint16_t>( (stationId-50)*5 + currentLayer-1);
        else spuID = static_cast<uint16_t>( ( (stationId-50)+1)*5 - 1);
 
	uint32_t hashId      = rodReadOut.hashIdentifier(moduleId);

        // To get rid of unhealthy word Careful!! It should be located after all the counter to look at the next words...
        if (address==0) {
          ATH_MSG_INFO ("Location word is not available and CscRawData is discarded!!");
          continue;
        }

        CscRawData * rawData = new CscRawData(amplitude, address, idColl, time, spuID, width, hashId);
	if ( isTimeComputed ) rawData->set_isTimeComputed();

        rawCollection->push_back(rawData);
        ATH_MSG_DEBUG ( "    idColl clusHashid spuID stationId :: "
                        << idColl << " " << hashId << "  " << spuID << "  " << stationId << " :: measphi"
                        << orientation << " L" << currentLayer << " strId " << stripId << " nStr " << width
                        << " T" << time << " nSampWords " << totalSampleWords << " "
                        << m_cscHelper->show_to_string(channelId) );
      }
      
      ATH_MSG_DEBUG ( "****Total Cluster count = " << clusterCount 
                      << " for RPU ID " << rpuID );
      /** First check that we've processed this RPU correctly */
      // If any of two RPU has problem, delete both.....likely have a problem

      if ( counter > rpuSize ) {
        ATH_MSG_INFO ( "ROB Fragment with ID 0x" << std::hex << robFrag.rod_source_id() << std::dec
                       << " has word Counter =" << counter << " must not exceed summed RPU sizes ="
                       << rpuSize << " Discarded!!");

        rawCollection->erase(rawCollection->begin(), rawCollection->end()); // identical to the following lines (duplecate)
        //        for ( CscRawDataCollection::const_iterator idig=rawCollection->begin(); idig!=rawCollection->end(); ++idig ) {
        //          delete *idig;
        //        }
        // DONOT delete rawCollection because you added it..
        // delete rawCollection;  
        return;
      } 

      /** check for partial errors  and extra ghost words at the end */ 
      if ( counter < rpuSize ) {
        ATH_MSG_DEBUG ( "" << (rpuSize-counter) << "  ghost words at the end of this RPU - skipping ..." );
        counter = rpuSize;
      }
    } // if (dataType == 1) else
  } // while (counter<size)

  // To reject unhealthy rob
  if (isHeaderWordNull) {
    ATH_MSG_INFO ( " ROB Fragment with ID " << std::hex<<robFrag.rod_source_id()<<std::dec << " has null rpuID. Discarded!!" );
    rawCollection->erase(rawCollection->begin(), rawCollection->end());
    //    for ( CscRawDataCollection::const_iterator idig=rawCollection->begin(); idig!=rawCollection->end(); ++idig ) {
    //      delete *idig;
    //    }
    // DONOT delete rawCollection because you added it..
    //    delete rawCollection;
    //    return;
  }
  if (isClusterWordsUnrealistic) {
    ATH_MSG_INFO ( " ROB Fragment with ID 0x" << std::hex<<robFrag.rod_source_id() << std::dec
                   << " has too many cluster words. Discarded!!" );
    rawCollection->erase(rawCollection->begin(), rawCollection->end());
    //    for ( CscRawDataCollection::const_iterator idig=rawCollection->begin(); idig!=rawCollection->end(); ++idig ) {
    //      delete *idig;
    //    }
    // DONOT delete rawCollection because you added it..
    //    delete rawCollection;
    //    return;
  }

  if(rawCollection) {
    StatusCode status_lock = lock.addOrDelete(std::move( rawCollection ) );
    if (status_lock.isFailure()) {
      ATH_MSG_ERROR ( "Could not insert CscRawDataCollection into CscRawDataContainer..." );
      return;
    }
  }

  ATH_MSG_DEBUG ( "end of CscROD_Decode::fillCollection()" );
  return;
}

void Muon::CscROD_Decoder::rodVersion1(const ROBFragment& robFrag,  CscRawDataContainer& rdoIDC) const {  

  ATH_MSG_DEBUG ( "in CscROD_Decode::fillCollection() - ROD version 1" );

  CscRODReadOutV1 rodReadOut;

  // Csc Id Helper not needed :: initialize pointer to 0!
  rodReadOut.set(0);

  // get some information
  SourceIdentifier sid (robFrag.rod_source_id()); 
  
  uint16_t subDetectorId = sid.subdetector_id();
  uint16_t rodId         = sid.module_id();

  uint16_t idColl        = m_cabling->collectionId(subDetectorId, rodId);

  //assert (subDetectorId <= 1);
  
  // Create the Csc container and use the cache to get it
  std::unique_ptr<CscRawDataCollection> rawCollection(nullptr);
  CscRawDataContainer::IDC_WriteHandle lock = rdoIDC.getWriteHandle( idColl );
  if( lock.alreadyPresent() ) {
    ATH_MSG_DEBUG ( "CSC RDO collection already exist with collection hash = " << idColl << " converting is skipped!");
  }
  else{
    ATH_MSG_DEBUG ( "CSC RDO collection does not exist - creating a new one with hash = " << idColl );
    rawCollection = std::make_unique<CscRawDataCollection>(idColl);
  }

  // set the ROD id and the subDector id
  rawCollection->setSubDetectorId(subDetectorId);
  rawCollection->setRodId(rodId);
  rawCollection->setOnlineId(idColl);

  const unsigned int size = robFrag.rod_ndata();
  ATH_MSG_DEBUG ( "CscROD_Decoder Total words received = " << size );

  PointerType p; 
  robFrag.rod_data( p );

  // expected number of samples
  // this is the first word in the ROD data block
  uint32_t numSamples = p[0];
  rawCollection->setNumSamples( (uint16_t) numSamples );

  // latency - the second word
  uint16_t latency  = (uint16_t) p[1];
  rawCollection->setLatency(latency);
  
  // the sampling rate
  uint16_t samplingRate = (uint16_t) p[2];
  rawCollection->setSamplingRate(samplingRate);  
  //rodReadOut.setSamplingTime(1000.0/samplingRate);

  // the number of DPU is the fourth word
  uint32_t numDPU = p[3];
  rawCollection->setNumDPU( (uint16_t)numDPU );

  // size of the ROD data block
  // expected size of the ROD header and footer
  uint32_t rodHeader  = rodReadOut.getHeaderSize();
  uint32_t rodFooter  = rodReadOut.getFooterSize();

  // loop over data fragments and decode them to fill the collection v
  // move the DPU header
  bool dpuFragment = rodReadOut.isDPU(p[rodHeader]);
  if (!dpuFragment) {
    ATH_MSG_ERROR ( "expecting a DPU fragment, Aborting..." );
    return;
  }

  uint32_t i = rodHeader;

  uint32_t numberOfDPU = 0x0; 
  while (dpuFragment && i < (size-rodFooter) ) {

    // the DPU id follows the DPU header marker
    i += 1;  uint32_t dpuID = p[i];
   
    // number of channel blocks to follow
    i += 1; uint32_t numChannel = p[i];
   
    // the satus word
    i += 1;
    // uint32_t status[6];
    // for (int j=0; j < 6; j++) status[j] = p[i+j];

    // miscellaneous debugging word
    i += 6;
    // uint32_t misc[2];
    // for (int j=0; j < 2; j++) misc[j] = p[i+j];

    // increase counter appropriately
    i += 2;

    // now check that there is a valid number of channel blocks to read
    if (!rodReadOut.discard(numChannel)) {
       for (unsigned int j=0; j < numChannel; j++) {
         // decode the channel block data 
         if (rodReadOut.isAddress(p[i])) {
            rodReadOut.decodeAddress(p[i]);
  	     //std::cout << "Found a CSC strip address = " << rodReadOut.address() << std::endl;
            i += 1;
         } else 
           ATH_MSG_ERROR ( "expecting an address fragment" );
         std::vector<uint16_t> amplitude;
         for (unsigned int j=1; j<=numSamples; j++) {
           if ( (j%2) == 1 ) {
             rodReadOut.decodeAmplitude(p[i]);
             amplitude.push_back(rodReadOut.getAmp1());
             if (amplitude.size() < numSamples) amplitude.push_back(rodReadOut.getAmp2());
             i += 1;
           }
         }
         uint32_t address     = rodReadOut.address();
         CscRawData * rawData = new CscRawData(amplitude,address,idColl,dpuID);
         rawCollection->push_back(rawData);
      }
    } else { // this is a discard  
      ATH_MSG_INFO ( "this is a discard::numChannel = " 
           << MSG::hex  << numChannel );
    }
    // check that the new fragment is a DPU header block
    if (i < (size-rodFooter)) dpuFragment = rodReadOut.isDPU(p[i]);
    numberOfDPU++;
  }

  if(rawCollection) {
    StatusCode status_lock = lock.addOrDelete(std::move( rawCollection ) );
    if (status_lock.isFailure()) {
      ATH_MSG_ERROR ( "Could not insert CscRawDataCollection into CscRawDataContainer..." );
      return;
    }
  }

  return;

}

void Muon::CscROD_Decoder::rodVersion0(const ROBFragment& robFrag,  CscRawDataContainer& rdoIDC) const {  

  ATH_MSG_DEBUG ( "in CscROD_Decode::fillCollection() - ROD version 0" );

  CscRODReadOutV0 rodReadOut;

  // Csc Id Helper not needed :: initialize pointer to 0!
  rodReadOut.set(0);

  const unsigned int size = robFrag.rod_ndata();
  ATH_MSG_DEBUG ( "CscROD_Decoder Total words received = " << size );

  PointerType p;
  robFrag.rod_data( p );

  // get some information
  rodReadOut.decodeSourceID ( p[3] );
  uint16_t subDetectorId = rodReadOut.subDetectorId();
  uint16_t rodId         = rodReadOut.rodId();

  //assert (subDetectorId <= 1);

  uint16_t subId = (subDetectorId == 0x0) ? 0x6A : 0x69;
  
  uint16_t idColl        = m_cabling->collectionId(subId, rodId);

  // Create the Csc container and use the cache to get it
  std::unique_ptr<CscRawDataCollection> rawCollection(nullptr);
  CscRawDataContainer::IDC_WriteHandle lock = rdoIDC.getWriteHandle( idColl );
  if( lock.alreadyPresent() ) {
    ATH_MSG_DEBUG ( "CSC RDO collection already exist with collection hash = " << idColl << " converting is skipped!");
  }
  else{
    ATH_MSG_DEBUG ( "CSC RDO collection does not exist - creating a new one with hash = " << idColl );
    rawCollection = std::make_unique<CscRawDataCollection>(idColl);
  }

  // set the ROD id and the subDector id
  rawCollection->setSubDetectorId(subDetectorId);
  rawCollection->setRodId(rodId);
  rawCollection->setOnlineId(idColl);

  uint32_t rodHeader  = rodReadOut.getHeaderSize();
  uint32_t rodFooter  = rodReadOut.getFooterSize();

  // loop over data fragments and decode them to fill the collection v
  bool bodyFragment = rodReadOut.isBody(p[rodHeader]);
  if (!bodyFragment) {
    ATH_MSG_ERROR ( "expecting a body fragment, Aborting..." );
    return;
  }
  uint32_t i = rodHeader; 
  while (bodyFragment && i < (size-rodFooter) ) {
    if (rodReadOut.isAmplitude(p[i])) {
      rodReadOut.decodeAmplitude(p[i],1);
      i += 1;
    } else {
      ATH_MSG_ERROR ( "expecting an amplitude fragment" 
           );
    }
    if (rodReadOut.isAmplitude(p[i])) {
      rodReadOut.decodeAmplitude(p[i],2);
      i += 1;
    } else 
      ATH_MSG_ERROR ( "expecting an amplitude fragment" 
           );
    if (rodReadOut.isAddress(p[i])) {
      rodReadOut.decodeAddress(p[i]);
      i += 1;
    } else 
      ATH_MSG_ERROR ( "expecting an address fragment" 
           );
    /*   MN
    uint16_t * amplitude = new uint16_t[4];
    *amplitude           = rodReadOut.getAmp1();
    *(amplitude+1)       = rodReadOut.getAmp2();
    *(amplitude+2)       = rodReadOut.getAmp3();
    *(amplitude+3)       = rodReadOut.getAmp4();
    uint32_t address     = rodReadOut.address();
    CscRawData * rawData = new CscRawData(amplitude,address,idColl);
    */
    std::vector<uint16_t> 	amplitudes;
    amplitudes.push_back( rodReadOut.getAmp1() );
    amplitudes.push_back( rodReadOut.getAmp2() );
    amplitudes.push_back( rodReadOut.getAmp3() );
    amplitudes.push_back( rodReadOut.getAmp4() );
    CscRawData * rawData = new CscRawData(amplitudes, rodReadOut.address(), idColl);
  
    
    rawCollection->push_back(rawData);

    // check that the new fragment is body
    bodyFragment = rodReadOut.isBody(p[i]);
  }

  if(rawCollection) {
    StatusCode status_lock = lock.addOrDelete(std::move( rawCollection ) );
    if (status_lock.isFailure()) {
      ATH_MSG_ERROR ( "Could not insert CscRawDataCollection into CscRawDataContainer..." );
      return;
    }
  }

  return;

}


