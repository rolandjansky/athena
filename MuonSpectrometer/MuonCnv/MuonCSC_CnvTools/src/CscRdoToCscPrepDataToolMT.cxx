/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Ketevi A. Assamagan, Woochun Park
/// BNL, April 03 2005

/// algorithm to decode RDO into PrepRawData

#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"

#include "CscRdoToCscPrepDataToolMT.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "EventPrimitives/EventPrimitives.h" 

#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "CscCalibTools/ICscCalibTool.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"

#include "GaudiKernel/ThreadLocalContext.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;


CscRdoToCscPrepDataToolMT::CscRdoToCscPrepDataToolMT
(const std::string& type, const std::string& name, const IInterface* parent)
  : CscRdoToCscPrepDataToolCore(type, name, parent) {
    declareProperty("CscStripPrdContainerCacheKey", m_prdContainerCacheKey, "Optional external cache for the CSC RDO container");
}  

CscRdoToCscPrepDataToolMT::~CscRdoToCscPrepDataToolMT(){}

StatusCode CscRdoToCscPrepDataToolMT::initialize(){
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( CscRdoToCscPrepDataToolCore::initialize() );
  ATH_CHECK( m_prdContainerCacheKey.initialize( !m_prdContainerCacheKey.key().empty() ) );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode CscRdoToCscPrepDataToolMT::finalize() {
  return CscRdoToCscPrepDataToolCore::finalize();
}


StatusCode CscRdoToCscPrepDataToolMT::decode(std::vector<IdentifierHash>& givenIdhs, std::vector<IdentifierHash>& decodedIdhs) {
  // WARNING : Trigger Part is not finished.
  unsigned int sizeVectorRequested = givenIdhs.size();
  ATH_MSG_DEBUG ( "decode for " << sizeVectorRequested << " offline collections called" );

  // clear output vector of selected data collections containing data
  decodedIdhs.clear();

  /// Recording the PRD container in StoreGate
  SG::WriteHandle< Muon::CscStripPrepDataContainer > outputHandle (m_outputCollectionKey);

  // Caching of PRD container
  const bool externalCachePRD = !m_prdContainerCacheKey.key().empty();
  if (!externalCachePRD) {
    // without the cache we just record the container
    StatusCode status = outputHandle.record(std::make_unique<Muon::CscStripPrepDataContainer>(m_idHelperSvc->cscIdHelper().module_hash_max()));
    if (status.isFailure() || !outputHandle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of CSC PrepData Container at " << m_outputCollectionKey.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Created container " << m_outputCollectionKey.key());
  } 
  else {
    // use the cache to get the container
    SG::UpdateHandle<CscStripPrepDataCollection_Cache> update(m_prdContainerCacheKey);
    if (!update.isValid()){
      ATH_MSG_FATAL("Invalid UpdateHandle " << m_prdContainerCacheKey.key());
      return StatusCode::FAILURE;
    }
    StatusCode status = outputHandle.record(std::make_unique<Muon::CscStripPrepDataContainer>(update.ptr()));
    if (status.isFailure() || !outputHandle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of CSC PrepData Container using cache " 
        << m_prdContainerCacheKey.key() << " - " <<m_outputCollectionKey.key()); 
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Created container using cache for " << m_prdContainerCacheKey.key());
  }
  // Pass the container from the handle
  m_outputCollection = outputHandle.ptr();


  if (sizeVectorRequested == 0) {
    m_fullEventDone=true;
    ATH_MSG_DEBUG ( "decoding the entire event " );
  } else {
    m_fullEventDone=false;
  }

  // retrieve the pointer to the RDO container
  // this will just get the pointer from memory if the container is already recorded in SG 
  // or 
  // will activate the TP converter for reading from pool root the RDO container and recording it in SG

  auto rdoContainerHandle = SG::makeHandle(m_rdoContainerKey);
  if (!rdoContainerHandle.isValid()) {
    ATH_MSG_WARNING ( "No CSC RDO container in StoreGate!" );
    return StatusCode::SUCCESS;
  }	
  const CscRawDataContainer* rdoContainer = rdoContainerHandle.cptr();

  ATH_MSG_DEBUG ( "Retrieved " << rdoContainer->size() << " CSC RDOs." );
  // here the RDO container is in SG and its pointer rdoContainer is initialised 
  // decoding 
  if (sizeVectorRequested)  {
    // seeded decoding
    for (unsigned int i=0; i<sizeVectorRequested; ++i) {
      if (CscRdoToCscPrepDataToolMT::decode(rdoContainer, givenIdhs[i],decodedIdhs).isFailure()) {
        ATH_MSG_ERROR ( "Unable to decode CSC RDO " << i << "th into CSC PrepRawData" );
        return StatusCode::FAILURE;
      }
    }
  } else {
    // unseeded decoding
    if (CscRdoToCscPrepDataToolMT::decode(rdoContainer, decodedIdhs).isFailure()) {
      ATH_MSG_ERROR ( "Unable to decode CSC RDO " );
      return StatusCode::FAILURE;
    }	
  }
  
  return StatusCode::SUCCESS;
}  

StatusCode CscRdoToCscPrepDataToolMT::decode(const CscRawDataContainer* rdoContainer, IdentifierHash givenHashId, std::vector<IdentifierHash>& decodedIdhs) {
  
    
  IdContext cscContext = m_idHelperSvc->cscIdHelper().module_context();
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
  
  // if CSC decoding is switched off stop here
  if( !m_decodeData ) {
    ATH_MSG_DEBUG ( "Stored empty container; Decoding CSC RDO into CSC PrepRawData is switched off" );
    return StatusCode::SUCCESS;
  }

  //These collections can be empty for the trigger
  if(!m_outputCollection || m_outputCollection->size()==0){
    ATH_MSG_DEBUG("Stored empty collection.");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG ( "Decoding CSC RDO into CSC PrepRawData" );
  /// create the CSC RDO decoder
  //**********************************************
  // retrieve specific collection for the givenID
  uint16_t idColl = 0xffff;
  m_cabling->hash2CollectionId(givenHashId,idColl);
  const CscRawDataCollection * rawCollection  = rdoContainer->indexFindPtr(idColl);
  if (nullptr ==  rawCollection) {
    ATH_MSG_DEBUG ( "Specific CSC RDO collection retrieving failed for collection hash = " << idColl );
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG ( "Retrieved " << rawCollection->size() << " CSC RDOs.");
  //return if the input raw collection is empty (can happen for seeded decoding in trigger)
  if(rawCollection->size()==0) return StatusCode::SUCCESS;

  //************************************************
  IdentifierHash cscHashId;

  unsigned int samplingTime = rawCollection->rate();
  unsigned int numSamples   = rawCollection->numSamples();
  bool samplingPhase        = rawCollection->samplingPhase();
  std::vector<float> charges;
  charges.reserve(4);
  std::vector<uint16_t> samples;
  samples.reserve(4);

  if ( int(samplingTime) != int(m_cscCalibTool->getSamplingTime())) {
    ATH_MSG_WARNING ( " CSC sampling time from Collection is NOT consistant to calibTool parameter!!!!!!! " );
  }
  
  // For each Rdo, loop over RawData, converter RawData to PrepRawData
  // Retrieve/create PrepRawData collection, and insert PrepRawData into collection
  CscRawDataCollection::const_iterator itD   =  rawCollection->begin(); 
  CscRawDataCollection::const_iterator itD_e =  rawCollection->end();

  // Each RDO Collection goes into a PRD Collection, but we need to know what hash the PRD container is
  // and this is determined by the stationID, no the RDO hash ID
  ATH_MSG_DEBUG("CSC RDO ID " << rawCollection->identify() << " with hashID " << rawCollection->identifyHash() );
  // Just use the first iterator entry as stationID does not change between data inside a single container
  Identifier stationId = m_cscRdoDecoderTool->stationIdentifier( (const CscRawData *)(*itD) );
  if (m_idHelperSvc->cscIdHelper().get_hash(stationId, cscHashId, &cscContext)) {
    ATH_MSG_WARNING ( "Unable to get CSC digiti collection hash id "
                       << "context begin_index = " << cscContext.begin_index()
                       << " context end_index  = " << cscContext.end_index()
                       << " the identifier is "     );
    stationId.show();
  }
  ATH_MSG_DEBUG("Create CSC PRD Collection with hash " << cscHashId << " (givenHashId is " << givenHashId << ")");
  std::unique_ptr<CscStripPrepDataCollection> collection = nullptr;
  CscStripPrepDataContainer::IDC_WriteHandle lock = m_outputCollection->getWriteHandle( cscHashId );
  // Note that if the hash check above works, we should never reach this step where the lock is present
  if( lock.alreadyPresent() ) {
     ATH_MSG_DEBUG ( "CSC PRD collection already exist with collection hash = " << cscHashId << " collection filling is skipped!");
     decodedIdhs.push_back(givenHashId);
     return StatusCode::SUCCESS;
  }
  else{
    ATH_MSG_DEBUG ( "CSC PRD collection does not exist - creating a new one with hash = " << cscHashId );
    collection = std::make_unique<CscStripPrepDataCollection>( cscHashId );
    collection->setIdentifier(stationId);
  }

  for (; itD!=itD_e; ++itD) { 
    const CscRawData * data = (*itD);
    uint16_t width          = data->width();
    uint16_t totalSamples   = (data->samples()).size();
    uint32_t hashOffset     = data->hashId();

    ATH_MSG_DEBUG ( " Size of online cluster in this RawData: " 
        << " Width = " << width << " Samples = " << totalSamples
        << " stationId : " << stationId << "  hashOffset : " << hashOffset);

    for (unsigned int j=0; j<width; ++j) {
      const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(data, j);
      ATH_MSG_DEBUG ( "        LOOP over width  " << j <<  " " << channelId );

      const CscReadoutElement * descriptor = muDetMgr->getCscReadoutElement(channelId);
      //calculate local positions on the strip planes
      if ( !descriptor ) {
        ATH_MSG_WARNING ( "Invalid descriptor for " << m_idHelperSvc->cscIdHelper().show_to_string(channelId)
                          << " Skipping channel " );
        continue;
      } else if (!descriptor->containsId(channelId)) {
        ATH_MSG_WARNING ("Identifier from the cabling service <"
                         <<m_idHelperSvc->cscIdHelper().show_to_string(channelId)
                         <<"> inconsistent with the geometry of detector element <"
                         <<m_idHelperSvc->cscIdHelper().show_to_string(descriptor->identify())
                         <<">  =>>ignore this hit");
        continue;
      }
      
      float timeOfFirstSample = 0.0;
      bool extractSamples = data->samples(j, numSamples, samples);
      if (!extractSamples) {
        ATH_MSG_WARNING ( "Unable to extract samples for strip " << j 
                          << " Online Cluster width = " << width 
                          << " for number of Samples = " << numSamples 
                          << " continuing ..." ); 
        continue;
      }   
      
      IdentifierHash stripHash;
      if (m_idHelperSvc->cscIdHelper().get_channel_hash(channelId, stripHash)) {
        ATH_MSG_WARNING ( "Unable to get CSC strip hash id");
        channelId.show();
      }
      
      bool adctocharge = m_cscCalibTool->adcToCharge(samples, stripHash, charges);
      if ( !adctocharge ) {
        ATH_MSG_WARNING ( " CSC conversion ADC to Charge failed " 
                          << "CSC PrepData not build ... skipping " );
        continue;
      }
      if ( samples.size() >=4 ) 
        ATH_MSG_DEBUG ( "ADC: " << m_idHelperSvc->cscIdHelper().show_to_string(channelId) 
                        << " " << samples[0] << " " << samples[1] << " " << samples[2] << " " << samples[3]
                        << " Charges: "  
                        << " " << charges[0] << " " << charges[1] << " " << charges[2] << " " << charges[3] ); 
      
      int measuresPhi    = m_idHelperSvc->cscIdHelper().measuresPhi(channelId);

      Amg::Vector2D localWirePos1( descriptor->xCoordinateInTrackingFrame(channelId ),0.);
      
      int chamberLayer   = m_idHelperSvc->cscIdHelper().chamberLayer(channelId);
      float stripWidth   = descriptor->cathodeReadoutPitch( chamberLayer, measuresPhi );
      double errPos      = stripWidth / sqrt(12.0);

      AmgSymMatrix(2) covariance;
      covariance.setIdentity();
      covariance *= errPos*errPos;
      Amg::MatrixX * errClusterPos = new Amg::MatrixX(covariance);

      /** new CscStripPrepRawData */
      CscStripPrepData * newPrepData = new CscStripPrepData(channelId,
                                                            cscHashId,
                                                            localWirePos1,
                                                            errClusterPos,
                                                            descriptor,
                                                            charges,
                                                            timeOfFirstSample,
                                                            samplingTime);
      
      if (samplingPhase) newPrepData->set_samplingPhase();
      newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
      collection->push_back(newPrepData);

    }
  }
  // Record the container
  StatusCode status_lock = lock.addOrDelete(std::move( collection ) );
  if (status_lock.isFailure()) {
    ATH_MSG_ERROR ( "Could not insert CscStripPrepdataCollection into CscStripPrepdataContainer..." );
    return StatusCode::FAILURE;
  }
  else{
    decodedIdhs.push_back(cscHashId);
  }
  return StatusCode::SUCCESS;
}

//************** Process for all in case of Offline
StatusCode CscRdoToCscPrepDataToolMT::decode(const CscRawDataContainer* rdoContainer, std::vector<IdentifierHash>& decodedIdhs)
{
  
  typedef CscRawDataContainer::const_iterator collection_iterator;
  
  IdContext cscContext = m_idHelperSvc->cscIdHelper().module_context();
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
 
  // if CSC decoding is switched off stop here
  if( !m_decodeData ) {
    ATH_MSG_DEBUG ( "Stored empty container. "
                    << "Decoding CSC RDO into CSC PrepRawData is switched off" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG ( "Decoding CSC RDO into CSC PrepRawData" );

  collection_iterator rdoColl     = rdoContainer->begin(); 
  collection_iterator lastRdoColl = rdoContainer->end();
  std::vector<float> charges;
  charges.reserve(4);
  std::vector<uint16_t> samples;
  samples.reserve(4);

  IdentifierHash cscHashId;
  for (; rdoColl!=lastRdoColl; ++rdoColl) {
    if ( (*rdoColl)->size() > 0 ) {
      ATH_MSG_DEBUG ( " Number of RawData in this rdo " << (*rdoColl)->size() );
      
      const  CscRawDataCollection* cscCollection = *rdoColl; 
      unsigned int samplingTime = cscCollection->rate();
      unsigned int numSamples   = cscCollection->numSamples();
      bool samplingPhase        = cscCollection->samplingPhase();

      if ( int(samplingTime) != int(m_cscCalibTool->getSamplingTime())) {
        ATH_MSG_WARNING ( " CSC sampling time from Collection is NOT consistant to calibTool parameter!!!!!!! " );
      }
      // For each Rdo, loop over RawData, converter RawData to PrepRawData
      // Retrieve/create PrepRawData collection, and insert PrepRawData into collection
      CscRawDataCollection::const_iterator itD   =  cscCollection->begin(); 
      CscRawDataCollection::const_iterator itD_e =  cscCollection->end();

      // Each RDO Collection goes into a PRD Collection, but we need to know what hash the PRD container is
      // and this is determined by the stationID, no the RDO hash ID
      ATH_MSG_DEBUG("CSC RDO ID " << (*rdoColl)->identify() << " with hashID " << (*rdoColl)->identifyHash() );
      // Just use the first iterator entry as stationID does not change between data inside a single container
      Identifier stationId = m_cscRdoDecoderTool->stationIdentifier( (const CscRawData *)(*itD) );
      if (m_idHelperSvc->cscIdHelper().get_hash(stationId, cscHashId, &cscContext)) {
        ATH_MSG_WARNING ( "Unable to get CSC digiti collection hash id "
                           << "context begin_index = " << cscContext.begin_index()
                           << " context end_index  = " << cscContext.end_index()
                           << " the identifier is "     );
        stationId.show();
      }

      ATH_MSG_DEBUG("Create CSC PRD Collection with hash " << cscHashId);
      std::unique_ptr<CscStripPrepDataCollection> collection = nullptr;
      CscStripPrepDataContainer::IDC_WriteHandle lock = m_outputCollection->getWriteHandle( cscHashId );
      if( lock.alreadyPresent() ) {
         ATH_MSG_DEBUG ( "CSC PRD collection already exist with collection hash = " << cscHashId << " collection filling is skipped!");
         continue;
      }
      else{
        ATH_MSG_DEBUG ( "CSC PRD collection does not exist - creating a new one with hash = " << cscHashId );
        collection = std::make_unique<CscStripPrepDataCollection>( cscHashId );
        collection->setIdentifier(stationId);
      }

      // This loops over the RDO data, decodes and puts into the PRD collection      
      for (; itD!=itD_e; ++itD) { 
        const CscRawData * data = (*itD);
        uint16_t width        = data->width();
        uint16_t totalSamples = (data->samples()).size();
        uint32_t hashOffset   = data->hashId();

        ATH_MSG_DEBUG ( "DecodeAll*Size of online cluster in this RawData: " 
                        << " Width = " << width << " Samples = " << totalSamples
                        << " stationId : " << stationId << "  hashOffset : " << hashOffset);

        for (unsigned int j=0; j<width; ++j) {
          const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(data, j);
          ATH_MSG_DEBUG ( "DecodeAll**LOOP over width  " << j <<  " " << channelId );

          const CscReadoutElement * descriptor = muDetMgr->getCscReadoutElement(channelId);
          //calculate local positions on the strip planes
          if ( !descriptor ) {
            ATH_MSG_WARNING ( "Invalid descriptor for " << m_idHelperSvc->cscIdHelper().show_to_string(channelId)
                              << " Skipping channel " );
            continue;
          }
          else if (!descriptor->containsId(channelId)) {
            ATH_MSG_WARNING ("Identifier from the cabling service <"
                             <<m_idHelperSvc->cscIdHelper().show_to_string(channelId)
                             <<"> inconsistent with the geometry of detector element <"
                             <<m_idHelperSvc->cscIdHelper().show_to_string(descriptor->identify())
                             <<">  =>>ignore this hit");
            continue;
          }
          
          float timeOfFirstSample = 0.0;
          bool extractSamples = data->samples(j, numSamples, samples);
          if (!extractSamples) {
            ATH_MSG_WARNING ( "Unable to extract samples for strip " << j 
                              << " Online Cluster width = " << width 
                              << " for number of Samples = " << numSamples 
                              << " continuing ..." ); 
            continue;
          }  
          
          IdentifierHash stripHash;
          if (m_idHelperSvc->cscIdHelper().get_channel_hash(channelId, stripHash)) {
            ATH_MSG_WARNING ( "Unable to get CSC strip hash id");
            channelId.show();
          }


          Identifier channelIdFromHash;
          m_idHelperSvc->cscIdHelper().get_id(stripHash, channelIdFromHash, &cscContext);
          
          
          bool adctocharge = m_cscCalibTool->adcToCharge(samples, stripHash, charges);
          if ( !adctocharge ) {
            ATH_MSG_WARNING ( " CSC conversion ADC to Charge failed " 
                              << "CSC PrepData not build ... skipping " );
            continue;
          }
          if ( samples.size() >=4 ) 
            ATH_MSG_DEBUG ( "DecodeAll*** ADC: " << m_idHelperSvc->cscIdHelper().show_to_string(channelId) << " "
                            << (int) stripHash << " "
                            << m_idHelperSvc->cscIdHelper().show_to_string(channelIdFromHash) 
                            << " " << samples[0] << " " << samples[1] << " " << samples[2] << " " << samples[3]
                            << " Charges: "  
                            << " " << charges[0] << " " << charges[1] << " " << charges[2] << " " << charges[3] ); 
          if (m_idHelperSvc->cscIdHelper().get_hash(stationId, cscHashId, &cscContext)) {
            ATH_MSG_WARNING ( "Unable to get CSC hash id from CSC RDO collection "
                              << "context begin_index = " << cscContext.begin_index()
                              << " context end_index  = " << cscContext.end_index() 
                              << " the identifier is "      );
            stationId.show();
          }
                    
          // Check if this strip is already decoded.. Then we don't have to decode it again
          bool IsThisStripDecoded =0; 
          for ( CscStripPrepDataCollection::const_iterator idig=collection->begin();
                idig!=collection->end(); ++idig ) {
            const CscStripPrepData& dig = **idig;
            Identifier did = dig.identify();
            if (did ==channelId) {
              IsThisStripDecoded =1;
              break;
            }
          }
          if (IsThisStripDecoded) continue;

          int measuresPhi    = m_idHelperSvc->cscIdHelper().measuresPhi(channelId);

          Amg::Vector2D localWirePos1( descriptor->xCoordinateInTrackingFrame(channelId ),0.);

 
          int chamberLayer   = m_idHelperSvc->cscIdHelper().chamberLayer(channelId);
          float stripWidth   = descriptor->cathodeReadoutPitch( chamberLayer, measuresPhi );
          double errPos      = stripWidth / sqrt(12.0);
                
          AmgSymMatrix(2) covariance;
          covariance.setIdentity();
          covariance *= errPos*errPos;
          Amg::MatrixX * errClusterPos = new Amg::MatrixX(covariance);

          /** new CscPrepRawData */
          CscStripPrepData * newPrepData = new CscStripPrepData(  channelId,
                                                                  cscHashId,
                                                                  localWirePos1,
                                                                  errClusterPos,
                                                                  descriptor,
                                                                  charges,
                                                                  timeOfFirstSample,
                                                                  samplingTime);
          
          if (samplingPhase) newPrepData->set_samplingPhase();
          newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
          collection->push_back(newPrepData);

        }
      }
      // Record the container after looping through all the RDO data in this RDO collection
      StatusCode status_lock = lock.addOrDelete(std::move( collection ) );
      if (status_lock.isFailure()) {
        ATH_MSG_ERROR ( "Could not insert CscStripPrepdataCollection into CscStripPrepdataContainer..." );
        return StatusCode::FAILURE;
      }
      decodedIdhs.push_back(cscHashId);
    }
  }
  return  StatusCode::SUCCESS;
}
